/**
 * @file TextureManager.cpp
 * @brief Реализация менеджера текстур для загрузки, хранения и отрисовки графических ассет-файлов.
 */

#include "TextureManager.hpp"
#include <iostream>

/**
 * @brief Загружает изображение из файла и регистрирует его в базе менеджера.
 * 
 * Использует библиотеку SDL_image для чтения графического файла с диска, создает 
 * оптимизированную для видеокарты текстуру (SDL_Texture) и сохраняет её под уникальным текстовым ключом.
 * После создания текстуры исходная растровая поверхность (SDL_Surface) корректно удаляется из ОЗУ.
 * 
 * @param fileName Относительный или абсолютный путь к файлу изображения (PNG, JPG и др.).
 * @param tag Уникальный строковый идентификатор (ключ), закрепляемый за этой текстурой.
 * @param renderer Указатель на активный контекст рендеринга SDL.
 * @return true Если изображение успешно загружено и сохранено в кэш.
 * @return false Если файл не найден, поврежден или произошла ошибка создания текстуры.
 */
bool TextureManager::load(std::string fileName, std::string tag,
                          SDL_Renderer* renderer) {
    // Шаг 1: Загрузка изображения в оперативную память в виде сырых пикселей
    SDL_Surface* tmpSurface = IMG_Load(fileName.c_str());
    if (tmpSurface == 0) {
        return false;
    }
    
    // Шаг 2: Перенос пиксельных данных в видеопамять (GPU) для быстрого вывода
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tmpSurface);
    
    // Шаг 3: Обязательное освобождение оперативной памяти ОЗУ
    SDL_DestroySurface(tmpSurface);
    
    // Шаг 4: Регистрация готовой текстуры в ассоциативном массиве
    if (texture != 0) {
        textureMap_[tag] = texture;
        return true;
    }
    return false;
}

/**
 * @brief Отрисовывает изображение целиком в заданных экранных координатах.
 * 
 * Находит текстуру по её тегу и выводит её на экран. Метод поддерживает 
 * масштабирование и зеркальное отражение спрайта.
 * 
 * @param tag Уникальный строковый ключ текстуры, которую нужно отрисовать.
 * @param x Экранная координата X (левый верхний угол назначения).
 * @param y Экранная координата Y (левый верхний угол назначения).
 * @param width Ширина области отрисовки на экране (и ширина считывания из исходного файла).
 * @param height Высота области отрисовки на экране (и высота считывания из исходного файла).
 * @param renderer Указатель на активный контекст рендеринга SDL.
 * @param flip Режим зеркального отображения текстуры (по умолчанию без изменений: SDL_FLIP_NONE).
 */
void TextureManager::draw(std::string tag, float x, float y, float width,
                          float height, SDL_Renderer* renderer,
                          SDL_FlipMode flip) {
    SDL_FRect srcRect; ///< Прямоугольник-источник (какую часть изображения вырезать)
    SDL_FRect dstRect; ///< Прямоугольник-назначение (куда на экране поместить)

    // Считываем всё изображение, начиная с левого верхнего угла (0, 0)
    srcRect.x = srcRect.y = 0;
    srcRect.w = dstRect.w = width;
    srcRect.h = dstRect.h = height;
    
    // Задаем целевую позицию вывода на экране
    dstRect.x = x;
    dstRect.y = y;
    
    // Рендеринг текстуры с поддержкой трансформаций (поворот на 0 градусов, без смещения центра)
    SDL_RenderTextureRotated(renderer, textureMap_[tag], &srcRect, &dstRect, 0,
                             NULL, flip);
}

/**
 * @brief Отрисовывает конкретный кадр из текстурного атласа (спрайт-листа).
 * 
 * Вычисляет точное смещение внутри сетки изображений спрайт-листа на основе переданного 
 * номера кадра (по горизонтали) и номера строки (по вертикали), после чего выводит полученный кадр на экран.
 * 
 * @param tag Уникальный строковый ключ текстуры-атласа.
 * @param x Экранная координата X для вывода кадра.
 * @param y Экранная координата Y для вывода кадра.
 * @param width Ширина одного кадра в пикселях.
 * @param height Высота одного кадра в пикселях.
 * @param currentRow Номер строки на спрайт-листе (счет начинается с 1).
 * @param currentFrame Номер текущего кадра в строке (счет начинается с 0).
 * @param renderer Указатель на активный контекст рендеринга SDL.
 * @param flip Режим зеркального отображения кадра.
 */
void TextureManager::drawFrame(std::string tag, float x, float y, float width,
                               float height, int currentRow, int currentFrame,
                               SDL_Renderer* renderer, SDL_FlipMode flip) {
    SDL_FRect srcRect; ///< Область вырезания конкретного кадра из атласа
    SDL_FRect dstRect; ///< Целевое положение кадра на игровом экране
    
    // Вычисление смещения по X (номер кадра умножаем на ширину кадра)
    srcRect.x = width * currentFrame;
    // Вычисление смещения по Y (номер строки уменьшаем на 1, так как строки считаются с единицы)
    srcRect.y = height * (currentRow - 1);
    
    srcRect.w = dstRect.w = width;
    srcRect.h = dstRect.h = height;
    dstRect.x = x;
    dstRect.y = y;
    
    // Вывод изолированного кадра анимации в видеопоток
    SDL_RenderTextureRotated(renderer, textureMap_[tag], &srcRect, &dstRect, 0,
                             NULL, flip);
}