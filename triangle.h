#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <SDL2/SDL.h>

typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} Triangle;

void fill_triangle(SDL_Renderer *renderer, Triangle triangle);

Triangle triangle_sorted_by_y(Triangle t);

#endif // TRIANGLE_H_