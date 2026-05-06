
#include "Player.hpp"
#include "InputHandler.hpp"

void Player::load(std::string tag, float x, float y, float w, float h) {
    GameObject::load(tag, x, y, w, h);
}

void Player::draw(SDL_Renderer* renderer) { GameObject::draw(renderer); }

void Player::update() {
    float speed = 0.1f;
    if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_W)) y_ -= speed;
    if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_S)) y_ += speed;
    if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_A)) x_ -= speed;
    if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_D)) x_ += speed;
}

void Player::clean() { GameObject::clean(); }