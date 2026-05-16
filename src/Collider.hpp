#pragma once
#include <SDL3/SDL.h>

class Collider {
public:
    SDL_FRect bounds;

    Collider(float x, float y, float w, float h);

    static bool checkCollision(const SDL_FRect& rectA, const SDL_FRect& rectB);

    void drawDebug(SDL_Renderer* renderer);
};