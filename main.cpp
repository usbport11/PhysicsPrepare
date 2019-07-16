#include "stdafx.h"
#include "classes/system/Shader.h"
#include "classes/system/Scene.h"
#include "classes/system/FPSController.h"
#include "classes/buffers/StaticBuffer.h"
#include "classes/physic/PhysicWorld.h"
#include "classes/physic/objects/Hero.h"
#include "classes/physic/objects/Enemy.h"

bool Pause;
bool keys[1024] = {0};
int WindowWidth = 800, WindowHeight = 600;
bool EnableVsync = 1;
GLFWwindow* window;
stFPSController FPSController;

MShader Shader;
MScene Scene;

MPhysicWorld PhysicWorld;
MHero* Hero;
MEnemy* Enemy;
MStaticBuffer StaticBuffer;
glm::vec2 QuadVelocity = glm::vec2(5.0, 5.0);

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void mousepos_callback(GLFWwindow* window, double x, double y) {
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}
	
	if(action == GLFW_PRESS)
    	keys[key] = true;
    else if (action == GLFW_RELEASE)
    	keys[key] = false;
}

void MoveKeysPressed() {
	b2Vec2 CurrentVelocity = b2Vec2(0.0f, 0.0f);
	if(keys[GLFW_KEY_LEFT] || keys[GLFW_KEY_RIGHT] || keys[GLFW_KEY_UP] || keys[GLFW_KEY_DOWN]) {
		if(keys[GLFW_KEY_LEFT]) {
			CurrentVelocity.x = -QuadVelocity.x;
		}
		if(keys[GLFW_KEY_RIGHT]) {
			CurrentVelocity.x = QuadVelocity.x;
		}
		if(keys[GLFW_KEY_UP]) {
			CurrentVelocity.y = QuadVelocity.y;
		}
		if(keys[GLFW_KEY_DOWN]) {
			CurrentVelocity.y = -QuadVelocity.y;
		}
	}
	Hero->SetVelocity(CurrentVelocity); 
}

bool InitApp() {
	LogFile<<"Starting application"<<endl;    
    glfwSetErrorCallback(error_callback);
    
    if(!glfwInit()) return false;
    window = glfwCreateWindow(WindowWidth, WindowHeight, "TestApp", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return false;
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mousepos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwMakeContextCurrent(window);
    if(glfwExtensionSupported("WGL_EXT_swap_control")) {
    	LogFile<<"Window: V-Sync supported. V-Sync: "<<EnableVsync<<endl;
		glfwSwapInterval(EnableVsync);//0 - disable, 1 - enable
	}
	else LogFile<<"Window: V-Sync not supported"<<endl;
    LogFile<<"Window created: width: "<<WindowWidth<<" height: "<<WindowHeight<<endl;

	//glew
	GLenum Error = glewInit();
	if(GLEW_OK != Error) {
		LogFile<<"Window: GLEW Loader error: "<<glewGetErrorString(Error)<<endl;
		return false;
	}
	LogFile<<"GLEW initialized"<<endl;
	
	if(!CheckOpenglSupport()) return false;

	//shaders
	if(!Shader.CreateShaderProgram("shaders/main.vertexshader.glsl", "shaders/main.fragmentshader.glsl")) return false;
	if(!Shader.AddUnifrom("MVP", "MVP")) return false;
	LogFile<<"Shaders loaded"<<endl;

	//scene
	if(!Scene.Initialize(&WindowWidth, &WindowHeight)) return false;
	LogFile<<"Scene initialized"<<endl;

	//randomize
    srand(time(NULL));
    LogFile<<"Randomized"<<endl;ll
    
    //other initializations
    if(!PhysicWorld.Create(0.0f, (float)1/60, 8, 3, 0.01f)) return false;
    PhysicWorld.AddWall(b2Vec2(0, 0), b2Vec2(0, 400));
    PhysicWorld.AddWall(b2Vec2(0, 0), b2Vec2(400, 0));
    PhysicWorld.AddWall(b2Vec2(400, 0), b2Vec2(400, 400));
    PhysicWorld.AddWall(b2Vec2(0, 400), b2Vec2(400, 400));
    Hero = new MHero;
    Hero->Set(glm::vec2(50, 50), glm::vec2(50, 50), glm::vec3(1, 1, 1));
    if(!PhysicWorld.AddPhysicQuad((MPhysicQuad*)Hero)) return false;
    Enemy = new MEnemy;
    Enemy->Set(glm::vec2(300, 300), glm::vec2(50, 50), glm::vec3(0, 0, 1));
    if(!PhysicWorld.AddPhysicQuad((MPhysicQuad*)Enemy)) return false;
    PhysicWorld.FillQuadBuffer();
    
    StaticBuffer.Initialize();
    StaticBuffer.SetPrimitiveType(GL_LINES);
    StaticBuffer.AddVertex(glm::vec2(0, 0), glm::vec3(0, 1, 0));
    StaticBuffer.AddVertex(glm::vec2(0, 400), glm::vec3(0, 1, 0));
    StaticBuffer.AddVertex(glm::vec2(0, 0), glm::vec3(0, 1, 0));
    StaticBuffer.AddVertex(glm::vec2(400, 0), glm::vec3(0, 1, 0));
    StaticBuffer.AddVertex(glm::vec2(400, 0), glm::vec3(0, 1, 0));
    StaticBuffer.AddVertex(glm::vec2(400, 400), glm::vec3(0, 1, 0));
    StaticBuffer.AddVertex(glm::vec2(0, 400), glm::vec3(0, 1, 0));
    StaticBuffer.AddVertex(glm::vec2(400, 400), glm::vec3(0, 1, 0));
    StaticBuffer.Dispose();
	
	//turn off pause
	Pause = false;
    
    return true;
}

void PreRenderStep() {
	if(Pause) return;
	MoveKeysPressed();
	PhysicWorld.Step();
	PhysicWorld.UpdateObjects();
}

void RenderStep() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(Shader.ProgramId);
	glUniformMatrix4fv(Shader.Uniforms["MVP"], 1, GL_FALSE, Scene.GetDynamicMVP());
	
	//draw functions
	PhysicWorld.DrawQuadBuffer();
	StaticBuffer.Begin();
	StaticBuffer.Draw();
	StaticBuffer.End();
}

void ClearApp() {
	//clear funstions
	PhysicWorld.Close();
	StaticBuffer.Close();
	
	memset(keys, 0, 1024);
	Shader.Close();
	LogFile<<"Application: closed"<<endl;
}

int main(int argc, char** argv) {
	LogFile<<"Application: started"<<endl;
	if(!InitApp()) {
		ClearApp();
		glfwTerminate();
		LogFile.close();
		return 0;
	}
	FPSController.Initialize(glfwGetTime());
	while(!glfwWindowShouldClose(window)) {
		FPSController.FrameStep(glfwGetTime());
    	FPSController.FrameCheck();
    	PreRenderStep();
		RenderStep();
        glfwSwapBuffers(window);
        glfwPollEvents();
	}
	ClearApp();
    glfwTerminate();
    LogFile.close();
}
