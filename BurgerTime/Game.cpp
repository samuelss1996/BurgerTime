#include "Game.h"
#include "Engine.h"
#include "Entity.h"
#include "Text.h"
#include "FpsCounterComponent.h"

Game::Game(Engine* engine) {
	this->engine = engine;
	this->entities = new std::vector<Entity*>();
}

void Game::init() {
	this->createFpsCounter();

	Entity::init();
}

void Game::update(float dt) {
	Entity::update(dt);

	for (auto it = this->entities->begin(); it != this->entities->end(); it++) {
		(*it)->update(dt);
	}
}

void Game::addEntity(Entity* entity) {
	this->entities->push_back(entity);
}

void Game::createFpsCounter() {
	Entity* fpsCounter = new Entity(Coordinate(20, 20));
	Text* text = new Text(this->engine->getRenderer(), "space_invaders.ttf", 25);

	fpsCounter->addComponent(new FpsCounterComponent(this->engine, fpsCounter, text));
	this->addEntity(fpsCounter);
}

Game::~Game() {
	Entity::~Entity();

	for (auto it = this->entities->begin(); it != this->entities->end(); it++) {
		delete *it;
	}

	delete this->entities;
}