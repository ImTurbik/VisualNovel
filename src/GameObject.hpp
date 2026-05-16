#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <string>

class GameObject {
protected:
    std::string texture_tag_;
    int currentFrame_;
    int currentRow_;
    float x_, y_;
    float w_, h_;

public:
    virtual void load(std::string tag, float x, float y, float w, float h);
    virtual void draw(SDL_Renderer* renderer);
    virtual void update();
    virtual void clean();

    SDL_FRect getBounds() const;
    std::string getTextureTag() const;
};