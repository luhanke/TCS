/******************************************************************
 *                    贪吃蛇游戏 - 主函数                            *
 *                    Snake Game - Main Function                     *
 ******************************************************************/

#include "snake_game.h"
#include <time.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SnakeGameClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    if (!RegisterClassW(&wc)) {
        MessageBoxW(NULL, L"注册窗口类失败", L"错误", MB_ICONERROR);
        return 0;
    }
    
    hWnd = CreateWindowExW(
        0, L"SnakeGameClass", L"贪吃蛇游戏",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL
    );
    
    if (!hWnd) {
        MessageBoxW(NULL, L"创建窗口失败", L"错误", MB_ICONERROR);
        return 0;
    }
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    SetFocus(hWnd);
    
    createGdiObjects();
    srand((unsigned int)time(NULL));
    
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    deleteGdiObjects();
    
    return 0;
}
