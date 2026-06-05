/******************************************************************
 *                    贪吃蛇游戏 - GDI对象管理                        *
 *                    Snake Game - GDI Management                   *
 ******************************************************************/

#include "snake_game.h"

void createGdiObjects(void) {
    brushGreen = CreateSolidBrush(RGB(50, 200, 50));
    brushDarkGreen = CreateSolidBrush(RGB(30, 150, 30));
    brushRed = CreateSolidBrush(RGB(255, 80, 80));
    brushBlue = CreateSolidBrush(RGB(80, 120, 255));
    brushYellow = CreateSolidBrush(RGB(255, 220, 50));
    brushGray = CreateSolidBrush(RGB(100, 100, 120));
    brushCyan = CreateSolidBrush(RGB(50, 255, 255));
    brushPurple = CreateSolidBrush(RGB(180, 50, 180));
    brushBg = CreateSolidBrush(RGB(15, 15, 35));
    brushBorder = CreateSolidBrush(RGB(60, 60, 100));
    brushWall = CreateSolidBrush(RGB(45, 45, 75));
    
    hFontTitle = CreateFontW(56, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");
    hFontMenu = CreateFontW(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");
    hFontInfo = CreateFontW(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");
    hFontGame = CreateFontW(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");
}

void deleteGdiObjects(void) {
    DeleteObject(brushGreen);
    DeleteObject(brushDarkGreen);
    DeleteObject(brushRed);
    DeleteObject(brushBlue);
    DeleteObject(brushYellow);
    DeleteObject(brushGray);
    DeleteObject(brushCyan);
    DeleteObject(brushPurple);
    DeleteObject(brushBg);
    DeleteObject(brushBorder);
    DeleteObject(brushWall);
    
    DeleteObject(hFontTitle);
    DeleteObject(hFontMenu);
    DeleteObject(hFontInfo);
    DeleteObject(hFontGame);
}
