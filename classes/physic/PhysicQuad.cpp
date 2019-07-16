#include "../../stdafx.h"
#include "PhysicQuad.h"

MPhysicQuad::MPhysicQuad() {
	Body = NULL;
	ColorQuad = stColorQuad();
	NeedRemove = false;
	Shape = NULL;
}

bool MPhysicQuad::Set(glm::vec2 Position, glm::vec2 Size, glm::vec3 Color, b2BodyType BodyType, uint16 Category, uint16 Mask, bool Sensor) {
	if(Size.x <= 0 || Size.y <= 0) return false;
	
	ColorQuad = stColorQuad(Position, Size, Color);
	
	StartParameters.SetMainData(Position, Size, Color, BodyType);
	StartParameters.AddFixtureData(glm::vec2(Size.x * 0.5f, Size.y * 0.5f), Category, Mask, Sensor);
	
	return true;
}

void MPhysicQuad::SetVelocity(b2Vec2 Velocity) {
	Body->SetLinearVelocity(Velocity);
}

void MPhysicQuad::Update() {
	OnUpdate();
	
	for(int i=0; i<4; i++) {
		ColorQuad.v[i].x = Body->GetWorldPoint(Shape->GetVertex(i)).x / *pScale;
		ColorQuad.v[i].y = Body->GetWorldPoint(Shape->GetVertex(i)).y / *pScale;
	}
}

void MPhysicQuad::SetBody(b2Body* inBody) {
	Body = inBody;
}

void MPhysicQuad::SetShape(b2PolygonShape* inShape) {
	Shape = inShape;
}

void MPhysicQuad::SetScale(float* inpScale) {
	pScale = inpScale;
}

bool MPhysicQuad::GetNeedRemove() {
	return NeedRemove;
}

stColorQuad* MPhysicQuad::GetColorQuad() {
	return &ColorQuad;
}

b2Body* MPhysicQuad::GetBody() {
	return Body;
}

stPhysicQuadParameters MPhysicQuad::GetStartParameters() {
	return StartParameters;
}

void MPhysicQuad::OnUpdate(){}
void MPhysicQuad::OnBeginCollide(b2Fixture* MainFixture, b2Fixture* ObjectFixture) {}
void MPhysicQuad::OnEndCollide(b2Fixture* MainFixture, b2Fixture* ObjectFixture) {}
void MPhysicQuad::OnBeginWallCollide() {}
void MPhysicQuad::OnEndWallCollide() {}

