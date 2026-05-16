#include "Npc.hpp"
#include <iostream>

void Npc::load(std::string tag, float x, float y, float w, float h) {
    GameObject::load(tag, x, y, w, h);
}

void Npc::draw(SDL_Renderer* renderer) {
    GameObject::draw(renderer);
}

void Npc::update() {
    GameObject::update();
}

void Npc::clean() {
    GameObject::clean();
    std::cout << "NPC cleaned" << std::endl;
}