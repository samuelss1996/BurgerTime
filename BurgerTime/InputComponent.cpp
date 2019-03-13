#include "InputComponent.h"
#include "Engine.h"

InputComponent::InputComponent(Engine* engine, Entity* entity) : Component(engine, entity) {
	this->enabled = true;
}

void InputComponent::update(double dt) {
	if (this->enabled) {
		if (engine->getKeyStatus(SDLK_ESCAPE) || engine->getKeyStatus(SDLK_q)) this->engine->getMessageDispatcher()->send(EXIT);

		if (engine->getKeyStatus(SDLK_a) || engine->getKeyStatus(SDLK_LEFT)) this->engine->getMessageDispatcher()->send(MOVE_LEFT);
		if (engine->getKeyStatus(SDLK_d) || engine->getKeyStatus(SDLK_RIGHT)) this->engine->getMessageDispatcher()->send(MOVE_RIGHT);
		if (engine->getKeyStatus(SDLK_w) || engine->getKeyStatus(SDLK_UP)) this->engine->getMessageDispatcher()->send(MOVE_UP);
		if (engine->getKeyStatus(SDLK_s) || engine->getKeyStatus(SDLK_DOWN)) this->engine->getMessageDispatcher()->send(MOVE_DOWN);
		
		if (engine->getKeyStatus(SDLK_SPACE)) this->engine->getMessageDispatcher()->send(MAIN_ACTION);
	}
}

void InputComponent::onKeyDown(SDL_Keycode key) {
	if (this->enabled) {
		if (key == SDLK_n) this->engine->getMessageDispatcher()->send(SWITCH_NIGHT_MODE);
		if (key == SDLK_r) this->engine->getMessageDispatcher()->send(RESET_GAME);
		if (key == SDLK_l) this->engine->getMessageDispatcher()->send(LOAD_NEW_LEVEL);
	}
}

void InputComponent::onKeyUp(SDL_Keycode key) {
	if (this->enabled) {

	}
}

void InputComponent::setEnabled(bool enabled) {
	this->enabled = enabled;
}
