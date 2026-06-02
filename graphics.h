#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <windows.h>

// Function pointer type for our drawing functions
typedef void (*DrawFunction)(HDC);

// The core runner that handles all the Windows API boilerplate
void RunGraphicsWindow(const char* title, DrawFunction drawFunc);

// Shape function declarations
void DrawDDALine(HDC hdc, int x0, int y0, int x1, int y1);
// void DrawCircle(HDC hdc, int xc, int yc, int r); // For later

#endif
