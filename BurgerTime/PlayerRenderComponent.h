#pragma once
#include "Component.h"
#include "Sprite.h"

class PlayerRenderComponent : public Component {
	Sprite* standingStill;
	Sprite* walkingLeft;
	Sprite* walkingRight;
	Sprite* upStairs;
	Sprite* downStairs;
	Sprite* celebrate;
	Sprite* die1;
	Sprite* die2;
	
	double deadTime;

public:
	PlayerRenderComponent(Engine* engine, Entity* entity);

	virtual void update(double dt);

	~PlayerRenderComponent();
};

