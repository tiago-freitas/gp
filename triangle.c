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

    return t;
}

float invslope(int x1, int x2, int y1, int y2)
{
    return (float) (x2 - x1) / (y2 - y1);
}

void fill_bottom_flat_triangle(SDL_Renderer *renderer, Triangle t) {
    const float islopeAB = invslope(t.x2, t.x1, t.y2, t.y1);
    const float islopeAC = invslope(t.x3, t.x1, t.y3, t.y1);
    
    float curx1 = t.x1;
    float curx2 = t.x1;

    for (int scanline = t.y1; scanline <= t.y2; scanline++) {
        scc(SDL_RenderDrawLine(
                    renderer,
                    (int) roundf(curx1),
                    scanline,
                    (int) roundf(curx2),
                    scanline));

        curx1 += islopeAB;
        curx2 += islopeAC;

    }
}

void fill_top_flat_triangle(SDL_Renderer *renderer, Triangle t) {
    const float islopeAC = invslope(t.x3, t.x1, t.y3, t.y1);
    const float islopeBC = invslope(t.x3, t.x2, t.y3, t.y2);
    
    float curx1 = t.x1;
    float curx2 = t.x2;
    for (int scanline = t.y1; scanline <= t.y3; scanline++) {
        scc(SDL_RenderDrawLine(
                    renderer,
                    (int) roundf(curx1),
                    scanline,
                    (int) roundf(curx2),
                    scanline));

        curx1 += islopeAC;
        curx2 += islopeBC;

    }
}


void fill_triangle(SDL_Renderer *renderer, Triangle t)
{
    t = triangle_sorted_by_y(t);

    if (t.y2 == t.y3)
        fill_bottom_flat_triangle(renderer, t);
    else if (t.y1 == t.y2)
        fill_top_flat_triangle(renderer, t);
    else {
        int x4 = t.x1 + ((float) (t.y2 - t.y1) / (t.y3 - t.y1)) * (t.x3 - t.x1);

        int y4 = t.y2;

        fill_bottom_flat_triangle(renderer, (Triangle) {
            t.x1, t.y1,
            t.x2, t.y2,
            x4, y4,
        });
        fill_top_flat_triangle(renderer, (Triangle) {
            t.x2, t.y2,
            x4, y4,
            t.x3, t.y3,
        });

        SDL_RenderDrawLine(renderer, t.x2, t.y2, x4, y4);
    }
}