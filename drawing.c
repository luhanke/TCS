/******************************************************************
 *                    贪吃蛇游戏 - 界面绘制                          *
 *                    Snake Game - Drawing                           *
 ******************************************************************/

#include "snake_game.h"

void drawGame(HDC hdc) {
    RECT rect;
    GetClientRect(hWnd, &rect);
    
    FillRect(hdc, &rect, brushBg);
    
    int gameAreaWidth = game.width * CELL_SIZE;
    int gameAreaHeight = game.height * CELL_SIZE;
    int startX = (rect.right - gameAreaWidth) / 2;
    int startY = (rect.bottom - gameAreaHeight) / 2 - 20;
    
    RECT borderRect = {startX - 6, startY - 6, startX + gameAreaWidth + 6, startY + gameAreaHeight + 6};
    FillRect(hdc, &borderRect, brushBorder);
    
    RECT gameArea = {startX, startY, startX + gameAreaWidth, startY + gameAreaHeight};
    HBRUSH brushGameBg = CreateSolidBrush(RGB(20, 20, 40));
    FillRect(hdc, &gameArea, brushGameBg);
    DeleteObject(brushGameBg);
    
    for (int x = 0; x < game.width; x++) {
        for (int y = 0; y < game.height; y++) {
            if ((x + y) % 2 == 0) {
                RECT cellBg = {startX + x * CELL_SIZE, startY + y * CELL_SIZE,
                              startX + (x + 1) * CELL_SIZE, startY + (y + 1) * CELL_SIZE};
                HBRUSH brushCellBg = CreateSolidBrush(RGB(22, 22, 44));
                FillRect(hdc, &cellBg, brushCellBg);
                DeleteObject(brushCellBg);
            }
        }
    }
    
    RECT foodRect = {startX + game.food.x * CELL_SIZE + 4, startY + game.food.y * CELL_SIZE + 4,
                     startX + (game.food.x + 1) * CELL_SIZE - 4, startY + (game.food.y + 1) * CELL_SIZE - 4};
    FillRect(hdc, &foodRect, brushYellow);
    
    if (game.mode == 2) {
        for (int i = 0; i < MAX_POISON; i++) {
            if (game.poison[i].active) {
                RECT poisonRect = {startX + game.poison[i].pos.x * CELL_SIZE + 3,
                                  startY + game.poison[i].pos.y * CELL_SIZE + 3,
                                  startX + (game.poison[i].pos.x + 1) * CELL_SIZE - 3,
                                  startY + (game.poison[i].pos.y + 1) * CELL_SIZE - 3};
                FillRect(hdc, &poisonRect, brushPurple);
                
                SetBkMode(hdc, TRANSPARENT);
                SelectObject(hdc, hFontGame);
                SetTextColor(hdc, RGB(255, 255, 255));
                wchar_t poisonChar[] = L"X";
                DrawTextW(hdc, poisonChar, -1, &poisonRect, DT_CENTER | DT_VCENTER);
            }
        }
    }
    
    if (game.mode == 3) {
        for (int i = 0; i < MAX_STONES; i++) {
            if (game.stones[i].active) {
                RECT stoneRect = {startX + game.stones[i].pos.x * CELL_SIZE + 2,
                                  startY + game.stones[i].pos.y * CELL_SIZE + 2,
                                  startX + (game.stones[i].pos.x + 1) * CELL_SIZE - 2,
                                  startY + (game.stones[i].pos.y + 1) * CELL_SIZE - 2};
                FillRect(hdc, &stoneRect, brushGray);
            }
        }
        
        for (int i = 0; i < MAX_WALLS; i++) {
            if (game.walls[i].active) {
                RECT wallRect = {startX + game.walls[i].pos.x * CELL_SIZE,
                                startY + game.walls[i].pos.y * CELL_SIZE,
                                startX + (game.walls[i].pos.x + 1) * CELL_SIZE,
                                startY + (game.walls[i].pos.y + 1) * CELL_SIZE};
                FillRect(hdc, &wallRect, brushWall);
            }
        }
        
        for (int i = 0; i < MAX_POWERUPS; i++) {
            if (game.speedPacks[i].active) {
                RECT speedRect = {startX + game.speedPacks[i].pos.x * CELL_SIZE + 3,
                                  startY + game.speedPacks[i].pos.y * CELL_SIZE + 3,
                                  startX + (game.speedPacks[i].pos.x + 1) * CELL_SIZE - 3,
                                  startY + (game.speedPacks[i].pos.y + 1) * CELL_SIZE - 3};
                FillRect(hdc, &speedRect, brushCyan);
                
                SetBkMode(hdc, TRANSPARENT);
                SelectObject(hdc, hFontGame);
                SetTextColor(hdc, RGB(255, 255, 255));
                wchar_t speedChar[] = L">";
                RECT speedTextRect = {startX + game.speedPacks[i].pos.x * CELL_SIZE,
                                      startY + game.speedPacks[i].pos.y * CELL_SIZE,
                                      startX + (game.speedPacks[i].pos.x + 1) * CELL_SIZE,
                                      startY + (game.speedPacks[i].pos.y + 1) * CELL_SIZE};
                DrawTextW(hdc, speedChar, -1, &speedTextRect, DT_CENTER | DT_VCENTER);
            }
            if (game.invinciblePacks[i].active) {
                RECT invRect = {startX + game.invinciblePacks[i].pos.x * CELL_SIZE + 3,
                               startY + game.invinciblePacks[i].pos.y * CELL_SIZE + 3,
                               startX + (game.invinciblePacks[i].pos.x + 1) * CELL_SIZE - 3,
                               startY + (game.invinciblePacks[i].pos.y + 1) * CELL_SIZE - 3};
                FillRect(hdc, &invRect, brushBlue);
                
                SetBkMode(hdc, TRANSPARENT);
                SelectObject(hdc, hFontGame);
                SetTextColor(hdc, RGB(255, 255, 255));
                wchar_t invChar[] = L"I";
                RECT invTextRect = {startX + game.invinciblePacks[i].pos.x * CELL_SIZE,
                                    startY + game.invinciblePacks[i].pos.y * CELL_SIZE,
                                    startX + (game.invinciblePacks[i].pos.x + 1) * CELL_SIZE,
                                    startY + (game.invinciblePacks[i].pos.y + 1) * CELL_SIZE};
                DrawTextW(hdc, invChar, -1, &invTextRect, DT_CENTER | DT_VCENTER);
            }
        }
    }
    
    for (int i = game.snake.length - 1; i >= 0; i--) {
        int sx = game.snake.body[i].x;
        int sy = game.snake.body[i].y;
        
        HBRUSH snakeBrush;
        if (i == 0) {
            if (game.snake.isInvincible) {
                snakeBrush = brushBlue;
            } else if (game.snake.isSpeedUp) {
                snakeBrush = brushCyan;
            } else {
                snakeBrush = brushGreen;
            }
        } else {
            if (game.snake.isInvincible) {
                snakeBrush = brushBlue;
            } else if (game.snake.isSpeedUp) {
                snakeBrush = brushCyan;
            } else {
                snakeBrush = (i % 2 == 0) ? brushGreen : brushDarkGreen;
            }
        }
        
        RECT bodyRect = {startX + sx * CELL_SIZE + 2, startY + sy * CELL_SIZE + 2,
                        startX + (sx + 1) * CELL_SIZE - 2, startY + (sy + 1) * CELL_SIZE - 2};
        FillRect(hdc, &bodyRect, snakeBrush);
    }
    
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, hFontInfo);
    SetTextColor(hdc, RGB(255, 255, 255));
    
    wchar_t scoreText[32];
    wsprintfW(scoreText, L"得分: %d", game.snake.score);
    TextOutW(hdc, 30, 25, scoreText, wcslen(scoreText));
    
    wchar_t modeText[32];
    if (game.mode == 1) wsprintfW(modeText, L"模式: 简单");
    else if (game.mode == 2) wsprintfW(modeText, L"模式: 普通");
    else wsprintfW(modeText, L"模式: 困难");
    TextOutW(hdc, 30, 55, modeText, wcslen(modeText));
    
    if (game.mode == 3) {
        wchar_t statusText[32];
        if (game.snake.isSpeedUp) {
            DWORD remaining = (game.snake.speedUpEndTime - GetTickCount()) / 1000;
            wsprintfW(statusText, L"加速: %lu秒", remaining);
            SetTextColor(hdc, RGB(0, 255, 255));
            TextOutW(hdc, 30, 85, statusText, wcslen(statusText));
        } else if (game.snake.isInvincible) {
            DWORD remaining = (game.snake.invincibleEndTime - GetTickCount()) / 1000;
            wsprintfW(statusText, L"无敌: %lu秒", remaining);
            SetTextColor(hdc, RGB(80, 120, 255));
            TextOutW(hdc, 30, 85, statusText, wcslen(statusText));
        }
    }
    
    SetTextColor(hdc, RGB(180, 180, 180));
    wchar_t controlHint[] = L"WASD/方向键:移动  |  P:暂停  |  Q:退出";
    TextOutW(hdc, 30, rect.bottom - 35, controlHint, wcslen(controlHint));
}

void drawMenu(HDC hdc) {
    RECT rect;
    GetClientRect(hWnd, &rect);
    
    HBRUSH brushMenuBg = CreateSolidBrush(RGB(10, 10, 25));
    FillRect(hdc, &rect, brushMenuBg);
    DeleteObject(brushMenuBg);
    
    int centerX = rect.right / 2;
    
    RECT titleBg = {centerX - 300, 60, centerX + 300, 160};
    HBRUSH brushTitleBg = CreateSolidBrush(RGB(20, 30, 60));
    FillRect(hdc, &titleBg, brushTitleBg);
    DeleteObject(brushTitleBg);
    
    SelectObject(hdc, hFontTitle);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(50, 255, 50));
    
    wchar_t title[] = L"贪 吃 蛇 游 戏";
    RECT titleRect = {centerX - 300, 70, centerX + 300, 150};
    DrawTextW(hdc, title, -1, &titleRect, DT_CENTER | DT_VCENTER);
    
    const wchar_t* menuItems[] = {L"1. 简 单 模 式", L"2. 普 通 模 式", L"3. 困 难 模 式", L"Q. 退 出 游 戏"};
    const wchar_t* menuDesc[] = {L"基础玩法", L"毒苹果+限时", L"石头+技能"};
    COLORREF menuColors[] = {RGB(100, 220, 100), RGB(255, 180, 80), RGB(255, 80, 80), RGB(180, 180, 180)};
    
    for (int i = 0; i < 4; i++) {
        RECT itemBg = {centerX - 180, 200 + i * 65, centerX + 180, 255 + i * 65};
        
        if (menuMode == i + 1) {
            HBRUSH brushSelected = CreateSolidBrush(RGB(40, 60, 100));
            FillRect(hdc, &itemBg, brushSelected);
            DeleteObject(brushSelected);
            
            HBRUSH brushBorderMenu = CreateSolidBrush(RGB(100, 150, 255));
            FrameRect(hdc, &itemBg, brushBorderMenu);
            DeleteObject(brushBorderMenu);
        }
        
        SelectObject(hdc, hFontMenu);
        SetTextColor(hdc, (menuMode == i + 1) ? RGB(100, 200, 255) : menuColors[i]);
        RECT itemRect = {centerX - 160, 205 + i * 65, centerX + 160, 250 + i * 65};
        DrawTextW(hdc, menuItems[i], -1, &itemRect, DT_CENTER | DT_VCENTER);
        
        if (i < 3) {
            SelectObject(hdc, hFontInfo);
            SetTextColor(hdc, RGB(150, 150, 150));
            RECT descRect = {centerX - 160, 235 + i * 65, centerX + 160, 255 + i * 65};
            DrawTextW(hdc, menuDesc[i], -1, &descRect, DT_CENTER | DT_VCENTER);
        }
    }
    
    SelectObject(hdc, hFontInfo);
    SetTextColor(hdc, RGB(120, 120, 140));
    wchar_t controls[] = L"操作: WASD/方向键  |  Enter确认  |  Q退出";
    TextOutW(hdc, centerX - 200, 500, controls, wcslen(controls));
}

void drawGameOver(HDC hdc) {
    RECT rect;
    GetClientRect(hWnd, &rect);
    
    HBRUSH brushOverBg = CreateSolidBrush(RGB(15, 10, 10));
    FillRect(hdc, &rect, brushOverBg);
    DeleteObject(brushOverBg);
    
    int centerX = rect.right / 2;
    int centerY = rect.bottom / 2;
    
    RECT overlayBg = {centerX - 280, centerY - 120, centerX + 280, centerY + 120};
    HBRUSH brushOverlayBg = CreateSolidBrush(RGB(30, 15, 15));
    FillRect(hdc, &overlayBg, brushOverlayBg);
    DeleteObject(brushOverlayBg);
    
    SelectObject(hdc, hFontTitle);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 60, 60));
    
    wchar_t gameOverText[] = L"游 戏 结 束";
    RECT gameOverRect = {centerX - 200, centerY - 100, centerX + 200, centerY - 30};
    DrawTextW(hdc, gameOverText, -1, &gameOverRect, DT_CENTER | DT_VCENTER);
    
    SelectObject(hdc, hFontMenu);
    SetTextColor(hdc, RGB(255, 255, 255));
    
    wchar_t scoreText[32];
    wsprintfW(scoreText, L"最终得分: %d", game.snake.score);
    RECT scoreRect = {centerX - 200, centerY - 20, centerX + 200, centerY + 30};
    DrawTextW(hdc, scoreText, -1, &scoreRect, DT_CENTER | DT_VCENTER);
    
    SelectObject(hdc, hFontInfo);
    SetTextColor(hdc, RGB(180, 180, 180));
    wchar_t hint[] = L"按 R 重新开始    Q 返回菜单";
    RECT hintRect = {centerX - 200, centerY + 50, centerX + 200, centerY + 90};
    DrawTextW(hdc, hint, -1, &hintRect, DT_CENTER | DT_VCENTER);
}

void drawPause(HDC hdc) {
    RECT rect;
    GetClientRect(hWnd, &rect);
    
    HBRUSH brushPauseBg = CreateSolidBrush(RGB(10, 10, 30));
    FillRect(hdc, &rect, brushPauseBg);
    DeleteObject(brushPauseBg);
    
    int centerX = rect.right / 2;
    int centerY = rect.bottom / 2;
    
    RECT overlayBg = {centerX - 250, centerY - 80, centerX + 250, centerY + 80};
    HBRUSH brushOverlayBg = CreateSolidBrush(RGB(20, 30, 60));
    FillRect(hdc, &overlayBg, brushOverlayBg);
    DeleteObject(brushOverlayBg);
    
    SelectObject(hdc, hFontTitle);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(100, 200, 255));
    
    wchar_t pauseText[] = L"游 戏 暂 停";
    RECT pauseRect = {centerX - 180, centerY - 60, centerX + 180, centerY};
    DrawTextW(hdc, pauseText, -1, &pauseRect, DT_CENTER | DT_VCENTER);
    
    SelectObject(hdc, hFontInfo);
    SetTextColor(hdc, RGB(200, 200, 200));
    wchar_t hint[] = L"按 P 继续游戏    Q 返回菜单";
    RECT hintRect = {centerX - 200, centerY + 10, centerX + 200, centerY + 50};
    DrawTextW(hdc, hint, -1, &hintRect, DT_CENTER | DT_VCENTER);
}
