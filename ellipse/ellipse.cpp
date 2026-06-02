#include "../graphics.h"
#include <cmath>
#include <algorithm>
#include "Utility.cpp"
using namespace std;


// For ellipse we use 4-way symmetry instead of 8-way
void set4Pixels(HDC hdc, int xc, int yc, int x, int y, COLORREF color) {
    SetPixel(hdc, xc + x, yc + y, color);
    SetPixel(hdc, xc - x, yc + y, color);
    SetPixel(hdc, xc + x, yc - y, color);
    SetPixel(hdc, xc - x, yc - y, color);
}


void drawDirect_Cartesian_Ellipse(HDC hdc, int xc, int yc, int a, int b, COLORREF color) {
    // Region 1: slope < 1, step in x
    int x = 0;
    int y = b;
    while ((double)x / a <= (double)y / b) {
        set4Pixels(hdc, xc, yc, x, y, color);
        x++;
        y = round(b * sqrt(1.0 - (double)(x * x) / (a * a)));
    }

    // Region 2: slope > 1, step in y
    x = a;
    y = 0;
    while ((double)x / a > (double)y / b) {
        set4Pixels(hdc, xc, yc, x, y, color);
        y++;
        x = round(a * sqrt(1.0 - (double)(y * y) / (b * b)));
    }
}


void directPolar_Ellipse(HDC hdc, int xc, int yc, int a, int b, COLORREF color) {
    double theta = 0, dtheta = 1.0 / max(a, b);

    while (theta <= M_PI / 2.0) {
        int x = round(a * cos(theta));
        int y = round(b * sin(theta));
        set4Pixels(hdc, xc, yc, x, y, color);
        theta += dtheta;
    }
}


void iterativePolar_Ellipse(HDC hdc, int xc, int yc, int a, int b, COLORREF color) {
    double theta = 0, dtheta = 1.0 / max(a, b);
    double cosd = cos(dtheta);
    double sind = sin(dtheta);

    double x = a, y = 0;

    while (theta <= M_PI / 2.0) {
        set4Pixels(hdc, xc, yc, round(x), round(y), color);
        double xnew = x * cosd - (a / (double)b) * y * sind;
        y = (b / (double)a) * x * sind + y * cosd;
        x = xnew;
        theta += dtheta;
    }
}


void firstOrderMidPoint_Ellipse(HDC hdc, int xc, int yc, int a, int b, COLORREF color) {
    long long a2 = (long long)a * a;
    long long b2 = (long long)b * b;

    int x = 0, y = b;

    // Region 1: while 2*b2*x <= 2*a2*y (slope condition)
    long long d1 = b2 - a2 * b + a2 / 4;
    while (2 * b2 * x <= 2 * a2 * y) {
        set4Pixels(hdc, xc, yc, x, y, color);
        if (d1 < 0) {
            d1 += 2 * b2 * x + 3 * b2;
        } else {
            d1 += 2 * b2 * x - 2 * a2 * y + 3 * b2 + 2 * a2;
            y--;
        }
        x++;
    }

    // Region 2
    long long d2 = b2 * (x + 0.5) * (x + 0.5) + a2 * (y - 1) * (y - 1) - a2 * b2;
    while (y >= 0) {
        set4Pixels(hdc, xc, yc, x, y, color);
        if (d2 > 0) {
            d2 += -2 * a2 * y + 3 * a2;
        } else {
            d2 += 2 * b2 * x - 2 * a2 * y + 3 * a2 + 2 * b2;
            x++;
        }
        y--;
    }
}


void secondOrderMidPoint_Ellipse(HDC hdc, int xc, int yc, int a, int b, COLORREF color) {
    long long a2 = (long long)a * a;
    long long b2 = (long long)b * b;

    int x = 0, y = b;

    // Region 1
    long long d1 = b2 - a2 * b + a2 / 4;
    long long c1 = 2 * b2;           // delta for x steps (df/dx increment)
    long long c2 = -2 * a2 * b + 2 * b2 + 2 * a2; // delta when y decreases

    while (2 * b2 * x <= 2 * a2 * y) {
        set4Pixels(hdc, xc, yc, x, y, color);
        if (d1 < 0) {
            d1 += c1 + b2;
            c2 += 2 * b2;
        } else {
            d1 += c2;
            c2 += 2 * b2 + 2 * a2;
            y--;
        }
        c1 += 2 * b2;
        x++;
    }

    // Region 2
    long long d2 = b2 * (x + 0.5) * (x + 0.5) + a2 * (y - 1) * (y - 1) - a2 * b2;
    long long c3 = 2 * a2;
    long long c4 = 2 * b2 * x - 2 * a2 * y + 2 * a2 + 2 * b2;

    while (y >= 0) {
        set4Pixels(hdc, xc, yc, x, y, color);
        if (d2 > 0) {
            d2 -= c3 - a2;
            c4 += 2 * a2;
        } else {
            d2 += c4;
            c4 += 2 * b2 + 2 * a2;
            x++;
        }
        c3 += 2 * a2;
        y--;
    }
}
