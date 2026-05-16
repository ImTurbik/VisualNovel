#include "Collider.hpp"

Collider::Collider(float x, float y, float w, float h) {
    bounds.x = x;
    bounds.y = y;
    bounds.w = w;
    bounds.h = h;
}

bool Collider::checkCollision(const SDL_FRect& rectA, const SDL_FRect& rectB) {
    if (rectA.x + rectA.w <= rectB.x) return false;
    if (rectA.x >= rectB.x + rectB.w) return false;
    if (rectA.y + rectA.h <= rectB.y) return false;
    if (rectA.y >= rectB.y + rectB.h) return false;

    return true;
}

void Collider::drawDebug(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderRect(renderer, &bounds);
}