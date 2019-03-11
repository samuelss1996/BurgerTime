#pragma once
#include "Entity.h"
#include "PlayerEntity.h"

class PlayerEntity;
enum EnemyType { SAUSAGE, EGG, CUCUMBER };

class EnemyEntity : public Entity {
	Coordinate* initialPosition;
	CharacterAction action;
	PlayerEntity* player;

	int deadMillisecs;
	bool hasMoved;
	bool canMove;

public:
	EnemyEntity(Engine* engine, Coordinate* position, EnemyType enemyType, PlayerEntity* player, std::vector<Entity*>* ingredients);

	virtual void update(double dt);

	void respawn();
	CharacterAction getAction();

private:
	void move();
	bool isInIntersection();
};

