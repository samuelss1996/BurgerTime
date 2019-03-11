#pragma once
#include "Entity.h"
#include "PlayerEntity.h"

class PlayerEntity;
enum EnemyType { SAUSAGE };

class EnemyEntity : public Entity {
	Coordinate* initialPosition;
	CharacterAction action;
	int deadMillisecs;

public:
	EnemyEntity(Engine* engine, Coordinate* position, EnemyType enemyType, PlayerEntity* player, std::vector<Entity*>* ingredients);

	virtual void update(double dt);

	void reset();
	CharacterAction getAction();

	~EnemyEntity();

private:
	void respawn();
};
