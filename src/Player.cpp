
#include "Player.hpp"
#include "InputHandler.hpp"

void Player::load(std::string tag, float x, float y, float w, float h) {
    GameObject::load(tag, x, y, w, h);
}

void Player::draw(SDL_Renderer* renderer) { GameObject::draw(renderer); }

void Player::update() {
    float speed = 0.08f;
    bool isMoving = false;

    if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_W)) {y_ -= speed; isMoving = true;};
    if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_S)) {y_ += speed; isMoving = true;};
    if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_A)) {x_ -= speed; isMoving = true;};
    if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_D)) {x_ += speed; isMoving = true;};

    if (isMoving) {
        int frame = (SDL_GetTicks() / 150) % 3;

        if (frame == 0) texture_tag_ = "player_idle";
        else if (frame == 1) texture_tag_ = "player_step1";
        else if (frame == 2) texture_tag_ = "player_step2";
    } else {
        texture_tag_ = "player_idle";
    }
}

void Player::clean() { GameObject::clean(); }