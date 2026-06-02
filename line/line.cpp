#include "../graphics.h"
#include <cmath>
#include <algorithm>

void DrawDDALine(HDC hdc, int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;

    int steps = std::max(std::abs(dx), std::abs(dy));

    float xInc = (float)dx / (float)steps;
    float yInc = (float)dy / (float)steps;

    float x = x0;
    float y = y0;

    for (int i = 0; i <= steps; i++) {
        SetPixel(hdc, (int)std::round(x), (int)std::round(y), RGB(0, 255, 0)); // Green line
        x += xInc;
        y += yInc;
    }
}


void DirectLine(HDC hdc, int x0, int y0, int x1, int y1) {
    // 1 & 2. Calculate differences
    int dx = x1 - x0;
    int dy = y1 - y0;

    // Handle vertical line edge-case to avoid division by zero
    if (dx == 0) {
        if (y0 > y1) std::swap(y0, y1);
        for (int y = y0; y <= y1; y++) {
            SetPixel(hdc, x0, y, RGB(0, 255, 0));
        }
        return;
    }

    // 3. Calculate slope (must cast to double)
    double slope = (double)dy / dx;

    // 4. Ensure we always draw from left to right
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    // 5. Plot loop
    for (int x = x0; x <= x1; x++) {
        // Equation: y = y0 + m * (x - x0)
        double y = y0 + slope * (x - x0);
        int yi = (int)std::round(y);

        SetPixel(hdc, x, yi, RGB(0, 255, 0)); // Green line
    }
}


void DirectLine2(HDC hdc, int x0, int y0, int x1, int y1) {
    int dx = x0 - x1;
    int dy = y0 - y1;

    if (abs(dy) <=abs(dx)){
        double slope = (double)dy / dx;
        if (x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        for (int x = x0 ; x <= x1; x++){
            int y = round (y0+(x-x0)*slope);
            SetPixel(hdc, x, y, RGB(0, 255, 0));
        }
    }else {
        double islope = (double)dx / dy;
        if (y0 > y1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        for (int y = y0 ; y <= y1; y++){
            int x = round (x0+(y-y0)*islope);
            SetPixel(hdc, x, y, RGB(0, 255, 0));
        }
    }

}


void DrawDDALine(HDC hdc, int x0, int y0, int x1, int y1) {
    int dx = x1-x0;
    int dy = y1 - y0;

    SetPixel(hdc, x0, y0, RGB(255, 0, 0));

    if (abs(dx) >= abs(dy)) {
        int x = x0; int xinc = dx > 0 ? 1 : -1;
        double y = y0, yinc = (double)dy / abs(dx) * xinc;

        while (x != x1) {
            x += xinc;
            y += yinc;
            SetPixel(hdc, x, round(y), RGB(255, 0, 0));
        }

    }else{
        int y = y0; int yinc = dy > 0 ? 1 : -1;
        double x = x0, xinc = (double)dx / abs(dy) * yinc;

        while (y != y1) {
            y += yinc;
            x += xinc;
            SetPixel(hdc, round(x), y, RGB(255, 0, 0));
        }
    }
}


void DrawBresenhamLine1(HDC hdc, int xs, int ys, int xe, int ye) {
    int y = ys;
    int x = xs;

    int dx = xe - xs;
    int dy = ye - ys;

    int d = dx - 2*dy;
    int change1 = 2*(dx-dy);
    int change2 = -2*dy;
    SetPixel(hdc, x, y, RGB(255, 0, 0));


    while (x < xe) {
        if (d < 0) {
            d += change1;
            y++;
        } else {
            d += change2;
        }
        x++;
        SetPixel(hdc, x, y, RGB(255, 0, 0));
    }


}

void DrawBresenhamLine2(HDC hdc, int xs, int ys, int xe, int ye) {
    int x = xs;
    int y = ys;

    int dx = xe - xs;
    int dy = ye - ys;

    int d       = 2 * dx - dy;        // initial decision: 2Δx - Δy
    int change1 = 2 * (dx - dy);      // d update when moving NE (x steps too)
    int change2 = 2 * dx;             // d update when moving N  (y only)

    SetPixel(hdc, x, y, RGB(255, 0, 0));

    while (y < ye) {
        if (d > 0) {
            d += change1;
            x++;                       // move NE: both x and y increment
        } else {
            d += change2;              // move N: y only
        }
        y++;
        SetPixel(hdc, x, y, RGB(255, 0, 0));
    }
}
