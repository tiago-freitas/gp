#include "./triangle.h"

void swap_ints(int *x, int *y)
{
    int t = *x;
    *x = *y;
    *y = t;
}

Triangle triangle_sorted_by_y(Triangle t)
{
    if (t.y1 > t.y2) { 
        swap_ints(&t.x1, &t.x2);
        swap_ints(&t.y1, &t.y2); 
    }
    if (t.y2 > t.y3) { 
        swap_ints(&t.x2, &t.x3);
        swap_ints(&t.y2, &t.y3); 
    }
    if (t.y1 > t.y2) { 
        swap_ints(&t.x1, &t.x2);
        swap_ints(&t.y1, &t.y2); 
    }
}

 * \param renderer The renderer which should draw a line.
 * \param x1 The x coordinate of the start point.
 * \param y1 The y coordinate of the start point.
 * \param x2 The x coordinate of the end point.
 * \param y2 The y coordinate of the end point.
 * \return 0 on success, or -1 on error
 *
 * \since This function is available since SDL 2.0.10.
 */

extern DECLSPEC int SDLCALL SDL_RenderDrawLineF(SDL_Renderer * renderer,
                                                float x1, float y1, float x2, float y2)
void fill_bottom_flat_triangle(SDL_Renderer *renderer, Triangle t) {
    float inc = (float) (t.x3 - t.x2) / (t.y2 - t.y1);
    for (size_t i = 0; i < t.y2 - t.y1; ++i) {
        scc(SDL_RenderDrawLine(
                    renderer,
                    CELL_WIDTH * x,
                    t.y1 + i,
                    CELL_WIDTH * x,
                    t.y1 + i));        
    }
}

void fill_triangle(SDL_Renderer *renderer, Triangle t)
{
    t = triangle_sorted_by_y(t);

    if (t.y2 == t.y3) {
        if (fill_bottom_flat_triangle(renderer, t) < 0) {
            return -1
        }
    } else if (t.y1 == t.y2) {
        if (fill_top_flat_triangle(renderer, t) < 0) {
            return -1;
        }
    } else {
        const Vec2f p4 = vec(t.x1 + ((t.y2 - t.y1) / (t.y3 - t.y1)) * (t.x3 - t.x1), t.y2);

        if (fill_bottom_flat_triangle(renderer, triangle(t.)) < 0) {
            return -1;
        }

        if (fill_top_flat_triangle(renderer, triangle()) < 0) {
            return -1;
        }
    }
}