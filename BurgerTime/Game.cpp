#include "Game.h"
#include "Engine.h"
#include "Entity.h"
#include "Text.h"
#include "FpsCounterComponent.h"
#include "InputComponent.h"
#include "BoxCollideComponent.h"
#include "PlayerEntity.h"
#include "RenderComponent.h"
#include "PlayerRenderComponent.h"
#include "LevelManager.h"
#include "DishFakeFloorEntity.h"
#include "ScoreCounterComponent.h"
#include "LivesTrackerEntity.h"
#include "FloorCollideComponent.h"
#include "TextRenderComponent.h"
#include "PepperCounterComponent.h"
#include "SoundEffectsComponent.h"
#include "PepperReloadEntity.h"

const bool SHOW_FPS = false;

Game::Game(Engine* engine) : Entity(engine) {
	this->chosenLevel = new std::string("resources/levels/default.bgtm");
}

void Game::init() {
	this->initFields();
	this->createFpsCounter();
	this->createGameComponents();
	this->createPlayer();
	this->createLevel();
	this->createHUD();
	this->performSubscriptions();

	this->addEntity(this->player);
	this->addEntity(this->player->getPepper());

	Entity::init();
}

void Game::update(double dt) {
	if (this->reset) {
		this->freeResources(true);
		this->init();
	}

	this->waitForIntro(dt);

	Entity::update(dt);

	for (auto it = this->entities->begin(); it != this->entities->end(); it++) {
		(*it)->update(dt);
	}
}

void Game::receive(Message message) {
	switch (message) {
		case EXIT:
			this->engine->stop();
			break;
		case SWITCH_NIGHT_MODE:
			this->lantern->setEnabled(!this->lantern->getEnabled());
			break;
		case INGREDIENT_FLOOR_HIT:
			this->increaseScore(50);
			break;
		case INGREDIENT_FINISHED:
			this->ingredientFinished();
			break;
		case PEPPER_THROWN:
			this->pepperThrown();
			break;
		case INCREASE_PEPPER:
			this->increasePepper();
			break;
		case ENEMY_SQUASHED:
			this->increaseScore(100);
			break;
		case ENEMY_ATTACK:
			this->freezeEnemies();
			break;
		case PLAYER_DIED:
			this->playerDied();
			break;
		case RESET_GAME:
			this->reset = true;
			break;
		case LOAD_NEW_LEVEL:
			this->loadNewLevel();
			break;
		case GAME_VICTORY:
			this->freezeEnemies();
			break;
	}
}

void Game::addEntity(Entity* entity) {
	this->entities->push_back(entity);
}

void Game::createPlayer() {
	Coordinate* playerPos = new Coordinate();

	this->player = new PlayerEntity(this->engine, playerPos, this->enemies, this);
	this->setWalkingEntityColliders(this->player, true);

	Sprite* lanternSprite = new Sprite(this->engine->getRenderer(), "resources/sprites/lantern.bmp");
	this->lantern = new Entity(this->engine, playerPos);
	this->lantern->addComponent(new RenderComponent(this->engine, this->lantern, lanternSprite));
}

void Game::createGameComponents() {
	this->input->setEnabled(false);

	this->addComponent(this->input);
	this->addComponent(new SoundEffectsComponent(this->engine, this));
}

void Game::createFpsCounter() {
	if (SHOW_FPS) {
		Entity* fpsCounter = new Entity(this->engine, new Coordinate(10, 10));
		Text* text = new Text(this->engine->getRenderer(), "resources/fonts/space_invaders.ttf", 8);

		fpsCounter->addComponent(new FpsCounterComponent(this->engine, fpsCounter, text));
		this->addEntity(fpsCounter);
	}
}

void Game::createHUD() {
	this->addEntity(this->lantern);
	this->lantern->setEnabled(false);

	Entity* scoreText = new Entity(this->engine, new Coordinate(24, 0));
	scoreText->addComponent(new ScoreCounterComponent(this->engine, scoreText, this));
	this->addEntity(scoreText);

	LivesTrackerEntity* livesTracker = new LivesTrackerEntity(this->engine, new Coordinate(8, 232), this);
	this->addEntity(livesTracker);

	Entity* pepperText = new Entity(this->engine, new Coordinate(200, 0));
	pepperText->addComponent(new PepperCounterComponent(this->engine, pepperText, this));
	this->addEntity(pepperText);

	Entity* controlsText = new Entity(this->engine, new Coordinate(8, 20));
	controlsText->addComponent(new TextRenderComponent(this->engine, controlsText,
		new std::string("NIGHT <N, LB>     LOAD <L, START>     RESET <R, RB>"),
		new Text(this->engine->getRenderer(), "resources/fonts/space_invaders.ttf", 8)));
	this->addEntity(controlsText);

	this->gameOverText = new Entity(this->engine, new Coordinate(70, 120));
	this->gameOverText->addComponent(new TextRenderComponent(this->engine, this->gameOverText, new std::string("GAME OVER"),
		new Text(this->engine->getRenderer(), "resources/fonts/space_invaders.ttf", 16)));
	this->gameOverText->setEnabled(false);

	this->addEntity(this->gameOverText);
}

void Game::createLevel() {
	LevelManager manager(this);

	manager.loadLevel(this->chosenLevel->c_str());
	this->addEndingLimit();

	this->addEntity(new PepperReloadEntity(this->engine, this->player, this->stairs));
}

void Game::setWalkingEntityColliders(Entity* entity, bool isPlayer) {
	FloorCollideComponent* floorBox = new FloorCollideComponent(this->engine, entity, ON_FLOOR, floors, isPlayer);
	BoxCollideComponent* leftBox = new BoxCollideComponent(this->engine, entity, INTERSECT_LIMIT_LEFT, leftFloorsLimits);
	BoxCollideComponent* rightBox = new BoxCollideComponent(this->engine, entity, INTERSECT_LIMIT_RIGHT, rightFloorsLimits);
	BoxCollideComponent* stairsBox = new BoxCollideComponent(this->engine, entity, INTERSECT_STAIRS, stairs);
	BoxCollideComponent* upStairsBox = new BoxCollideComponent(this->engine, entity, INTERSECT_UP_STAIRS, upStairsLimits);
	BoxCollideComponent* downStairsBox = new BoxCollideComponent(this->engine, entity, INTERSECT_DOWN_STAIRS, downStairsLimits);

	entity->addComponent(floorBox);
	entity->addComponent(leftBox);
	entity->addComponent(rightBox);
	entity->addComponent(stairsBox);
	entity->addComponent(upStairsBox);
	entity->addComponent(downStairsBox);
}

void Game::addFloor(Coordinate* position, int type) {
	Entity* floor = new Entity(this->engine, position);
	Sprite* floorSprite = new Sprite(this->engine->getRenderer(),
		type == 0? "resources/sprites/floor1.bmp" : "resources/sprites/floor2.bmp");

	floor->setBoundingBox(new BoundingBox(new Coordinate(16, 2)));
	floor->addComponent(new RenderComponent(this->engine, floor, floorSprite));

	this->floors->push_back(floor);
	this->addEntity(floor);

	this->updateLimits(FLOOR, position);
}

void Game::addStair(Coordinate* position) {
	Entity* stair = new Entity(this->engine, position);
	Sprite* stairSprite = new Sprite(this->engine->getRenderer(), "resources/sprites/stairs.bmp");

	stair->setBoundingBox(new BoundingBox(new Coordinate(1, 16)));
	stair->addComponent(new RenderComponent(this->engine, stair, stairSprite));

	this->stairs->push_back(stair);
	this->addEntity(stair);

	this->updateLimits(STAIR, position);
}

void Game::addIngredient(Coordinate* position, Ingredient ingredient) {
	IngredientEntity* ingredient1 = new IngredientEntity(this->engine, position, this->player, ingredient, this->ingredients, this->floors);

	this->totalIngredients++;
	this->ingredients->push_back(ingredient1);
	this->addEntity(ingredient1);
}

void Game::addDish(Coordinate* position) {
	Entity* dish = new Entity(this->engine, position);
	Sprite* sprite = new Sprite(engine->getRenderer(), "resources/sprites/dish.bmp");

	Coordinate* fakeFloorPosition = new Coordinate(position->getX(), position->getY());
	DishFakeFloorEntity* fakeFloor = new DishFakeFloorEntity(this->engine, fakeFloorPosition);

	dish->addComponent(new RenderComponent(engine, dish, sprite));

	this->floors->push_back(fakeFloor);
	this->addEntity(dish);
}

void Game::addEnemy(Coordinate* position, EnemyType enemyType, double idleTime) {
	EnemyEntity* enemy = new EnemyEntity(this->engine, position, enemyType, idleTime, this->player, this->ingredients);

	this->setWalkingEntityColliders(enemy, false);
	this->enemies->push_back(enemy);
	this->addEntity(enemy);
}

void Game::addPlayer(Coordinate* position) {
	this->player->setInitialPosition(position);
}

void Game::notifyKeyDown(SDL_Keycode key) {
	this->input->onKeyDown(key);
}

void Game::notifyKeyUp(SDL_Keycode key) {
	this->input->onKeyUp(key);
}

void Game::notifyControllerDown(Uint8 button) {
	this->input->onControllerDown(button);
}

void Game::notifyControllerUp(Uint8 button) {
	this->input->onControllerUp(button);
}

void Game::ingredientFinished() {
	this->currentFinishedIngredients++;

	if (this->currentFinishedIngredients >= this->totalIngredients) {
		this->victory();
	}
}

void Game::pepperThrown() {
	this->pepper--;
}

void Game::increasePepper() {
	if (this->pepper < MAX_PEPPER) {
		this->pepper++;
	}
}

void Game::freezeEnemies() {
	for (Entity* enemy : *this->enemies) {
		((EnemyEntity*)enemy)->freeze();
	}
}

void Game::playerDied() {
	this->lives--;

	if (this->lives <= 0) {
		this->input->setEnabled(false);
		this->gameOverText->setEnabled(true);
		this->engine->getMessageDispatcher()->send(GAME_OVER);
	}
	else {
		this->player->respawn();

		for (Entity* enemy : *this->enemies) {
			((EnemyEntity*)enemy)->respawn();
		}
	}
}

void Game::increaseScore(int increase) {
	this->score += increase;
}

int Game::getScore() {
	return this->score;
}

void Game::increaseLives() {
	if (this->lives < MAX_LIVES) {
		this->lives++;
	}
}

int Game::getLives() {
	return this->lives;
}

int Game::getPepper() {
	return this->pepper;
}

void Game::updateLimits(Field newField, Coordinate* position) {
	bool isNewUnit = (newField != this->previousField)
		|| (newField == FLOOR && (position->getY() != this->previousFieldPosition->getY() 
			|| abs(position->getX() - this->previousFieldPosition->getX()) > 16))
		|| (newField == STAIR && (position->getX() != this->previousFieldPosition->getX()
			|| abs(position->getY() - this->previousFieldPosition->getY()) > 16));

	if (isNewUnit) {
		this->addStartingLimit(newField, position);
		this->addEndingLimit();
	}

	this->previousField = newField;

	this->previousFieldPosition->setX(position->getX());
	this->previousFieldPosition->setY(position->getY());
}

void Game::addStartingLimit(Field newField, Coordinate * position) {
	if (newField == FLOOR) {
		Coordinate* limitPosition = new Coordinate(position->getX() - 16, position->getY());
		this->createFloorLimit(limitPosition, 0);
	}
	else if (newField == STAIR) {
		Coordinate* limitPosition = new Coordinate(position->getX(), position->getY() - 24);
		this->createStairLimit(limitPosition, 0);
	}
}

void Game::addEndingLimit() {
	if (this->previousField == FLOOR) {
		Coordinate* limitPosition = new Coordinate(this->previousFieldPosition->getX() + 16, this->previousFieldPosition->getY());
		this->createFloorLimit(limitPosition, 1);
	}
	else if (this->previousField == STAIR) {
		Coordinate* limitPosition = new Coordinate(this->previousFieldPosition->getX(), this->previousFieldPosition->getY() + 9);
		this->createStairLimit(limitPosition, 1);
	}
}

void Game::createFloorLimit(Coordinate* position, int type) {
	Entity* limit = new Entity(this->engine, position);
	limit->setBoundingBox(new BoundingBox(new Coordinate(16, 2)));
	position->setY(position->getY() - 5);

	if (type == 0) {
		//limit->addComponent(new RenderComponent(this->engine, limit, new Sprite(this->engine->getRenderer(), "resources/sprites/cheese (1).bmp")));
		this->leftFloorsLimits->push_back(limit);
	}
	else {
		//limit->addComponent(new RenderComponent(this->engine, limit, new Sprite(this->engine->getRenderer(), "resources/sprites/meat (1).bmp")));
		this->rightFloorsLimits->push_back(limit);
	}

	//this->addEntity(limit);
}

void Game::createStairLimit(Coordinate* position, int type) {
	Entity* stairLimit = new Entity(this->engine, position);
	stairLimit->setBoundingBox(new BoundingBox(new Coordinate(16, 2)));

	if (type == 0) {
		//stairLimit->addComponent(new RenderComponent(this->engine, stairLimit, new Sprite(this->engine->getRenderer(), "resources/sprites/top (1).bmp")));
		this->upStairsLimits->push_back(stairLimit);
	}
	else {
		//stairLimit->addComponent(new RenderComponent(this->engine, stairLimit, new Sprite(this->engine->getRenderer(), "resources/sprites/bottom (1).bmp")));
		this->downStairsLimits->push_back(stairLimit);
	}

	//this->addEntity(stairLimit);
}

void Game::victory() {
	this->input->setEnabled(false);
	this->engine->getMessageDispatcher()->send(GAME_VICTORY);
}

void Game::initFields() {
	this->entities = new std::vector<Entity*>();
	this->floors = new std::vector<Entity*>();
	this->leftFloorsLimits = new std::vector<Entity*>();
	this->rightFloorsLimits = new std::vector<Entity*>();
	this->stairs = new std::vector<Entity*>();
	this->upStairsLimits = new std::vector<Entity*>();
	this->downStairsLimits = new std::vector<Entity*>();
	this->ingredients = new std::vector<Entity*>();
	this->enemies = new std::vector<Entity*>();

	this->input = new InputComponent(this->engine, this);
	this->player = nullptr;
	this->previousField = NO_FIELD;
	this->previousFieldPosition = new Coordinate();
	this->reset = false;

	this->score = 0;
	this->totalIngredients = 0;
	this->currentFinishedIngredients = 0;
	this->lives = INITIAL_LIVES;
	this->pepper = INITIAL_PEPPER;

	this->introTime = INTRO_DURATION_MILLISECS / 1000.0;
}

void Game::performSubscriptions() {
	this->engine->getMessageDispatcher()->subscribe(EXIT, this);
	this->engine->getMessageDispatcher()->subscribe(SWITCH_NIGHT_MODE, this);
	this->engine->getMessageDispatcher()->subscribe(INGREDIENT_FLOOR_HIT, this);
	this->engine->getMessageDispatcher()->subscribe(INGREDIENT_FINISHED, this);
	this->engine->getMessageDispatcher()->subscribe(PEPPER_THROWN, this);
	this->engine->getMessageDispatcher()->subscribe(INCREASE_PEPPER, this);
	this->engine->getMessageDispatcher()->subscribe(PLAYER_DIED, this);
	this->engine->getMessageDispatcher()->subscribe(ENEMY_SQUASHED, this);
	this->engine->getMessageDispatcher()->subscribe(ENEMY_ATTACK, this);
	this->engine->getMessageDispatcher()->subscribe(RESET_GAME, this);
	this->engine->getMessageDispatcher()->subscribe(LOAD_NEW_LEVEL, this);
	this->engine->getMessageDispatcher()->subscribe(GAME_VICTORY, this);
}

void Game::waitForIntro(double dt) {
	if (this->introTime > 0) {
		this->introTime -= dt;

		if (this->introTime <= 0) {
			this->input->setEnabled(true);
			this->engine->getMessageDispatcher()->send(GAME_STARTED);
		}
	}
}

void Game::loadNewLevel() {
	delete this->chosenLevel;

	this->chosenLevel = LevelManager(this).promptLevel();
	this->reset = true;
}

void Game::freeResources(bool freeComponents) {
	if (freeComponents) {
		for (auto it = this->components->begin(); it != this->components->end(); it++) {
			delete *it;
		}

		this->components->clear();
	}

	for (auto it = this->entities->begin(); it != this->entities->end(); it++) {
		delete *it;
	}

	this->engine->getMessageDispatcher()->clear();

	delete this->entities;
	delete this->floors;
	delete this->leftFloorsLimits;
	delete this->rightFloorsLimits;
	delete this->stairs;
	delete this->upStairsLimits;
	delete this->downStairsLimits;
	delete this->ingredients;
	delete this->enemies;
	delete this->previousFieldPosition;
}

Game::~Game() {
	this->freeResources(false);
	delete this->chosenLevel;
}