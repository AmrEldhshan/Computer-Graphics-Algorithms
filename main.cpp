#include "graphics.h"
#include <cmath>

// Global pointer to hold the active drawing function
static DrawFunction g_CurrentDrawFunc = nullptr;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Execute the imported function call dynamically
            if (g_CurrentDrawFunc) {
                g_CurrentDrawFunc(hdc);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void RunGraphicsWindow(const char* title, DrawFunction drawFunc) {
    g_CurrentDrawFunc = drawFunc;
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const char CLASS_NAME[] = "GraphicsWindowClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return;

    ShowWindow(hwnd, SW_SHOWNORMAL);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// Your actual entry point where you control everything
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Import and wrapper your function call here
    auto MyDrawingScene = [](HDC hdc) {
        // Call your DDA line here (or any other shapes you import)
        DrawDDALine(hdc, 100, 100, 500, 400);
    };

    // Launches the window and renders your custom scene
    RunGraphicsWindow("Computer Graphics Workspace", MyDrawingScene);
    return 0;
}
