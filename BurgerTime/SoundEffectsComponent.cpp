#include "SoundEffectsComponent.h"
#include "Engine.h"

SoundEffectsComponent::SoundEffectsComponent(Engine* engine, Entity* entity) : Component(engine, entity) {
	this->backgroundMusic = Mix_LoadMUS("resources/sounds/music.mp3");

	this->intro = Mix_LoadWAV("resources/sounds/intro.mp3");

	this->pepper = Mix_LoadWAV("resources/sounds/pepper.mp3");
	this->ingredientStep = Mix_LoadWAV("resources/sounds/ingredient_step.mp3");
	this->ingredientHit = Mix_LoadWAV("resources/sounds/ingredient_hit.mp3");
	this->squashed = Mix_LoadWAV("resources/sounds/squashed.mp3");

	this->performSubscriptions();

	Mix_PlayChannel(-1, this->intro, 0);
}

void SoundEffectsComponent::update(double dt) {
	if (this->hasReceived(GAME_STARTED)) {
		Mix_PlayMusic(this->backgroundMusic, -1);
	}

	if (this->hasReceived(PEPPER_THROWN)) {
		Mix_PlayChannel(-1, this->pepper, 0);
	}
	if (this->hasReceived(ON_INGREDIENT_1)) {
		Mix_PlayChannel(-1, this->ingredientStep, 0);
	}
	if (this->hasReceived(INGREDIENT_INGREDIENT_HIT)) {
		Mix_PlayChannel(-1, this->ingredientHit, 0);
	}
	if (this->hasReceived(ENEMY_SQUASHED)) {
		Mix_PlayChannel(-1, this->squashed, 0);
	}

	this->clearMessages();
}

void SoundEffectsComponent::performSubscriptions() {
	this->engine->getMessageDispatcher()->subscribe(GAME_STARTED, this);

	this->engine->getMessageDispatcher()->subscribe(PEPPER_THROWN, this);
	this->engine->getMessageDispatcher()->subscribe(ON_INGREDIENT_1, this);
	this->engine->getMessageDispatcher()->subscribe(INGREDIENT_INGREDIENT_HIT, this);
	this->engine->getMessageDispatcher()->subscribe(ENEMY_SQUASHED, this);
}

SoundEffectsComponent::~SoundEffectsComponent() {
	// TODO
	Mix_FreeMusic(this->backgroundMusic);
}