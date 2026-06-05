/******************************************************************
 *                    贪吃蛇游戏 - 窗口消息处理                      *
 *                    Snake Game - Window Processing                  *
 ******************************************************************/

#include "snake_game.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SETFOCUS:
            SetFocus(hwnd);
            return 0;
        
        case WM_LBUTTONDOWN: {
            int mouseX = LOWORD(lParam);
            int mouseY = HIWORD(lParam);
            int centerX = 450;
            
            if (menuMode) {
                for (int i = 0; i < 4; i++) {
                    int itemTop = 200 + i * 65;
                    int itemBottom = 255 + i * 65;
                    if (mouseX >= centerX - 180 && mouseX <= centerX + 180 &&
                        mouseY >= itemTop && mouseY <= itemBottom) {
                        if (i < 3) {
                            initGame(i + 1);
                        } else {
                            PostQuitMessage(0);
                        }
                        InvalidateRect(hwnd, NULL, TRUE);
                        break;
                    }
                }
            }
            SetFocus(hwnd);
            return 0;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            if (menuMode) {
                drawMenu(hdc);
            } else if (game.gameOver) {
                drawGameOver(hdc);
            } else if (game.paused) {
                drawPause(hdc);
            } else {
                drawGame(hdc);
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_KEYDOWN: {
            if (menuMode) {
                if (wParam == '1') {
                    initGame(1);
                    InvalidateRect(hwnd, NULL, TRUE);
                } else if (wParam == '2') {
                    initGame(2);
                    InvalidateRect(hwnd, NULL, TRUE);
                } else if (wParam == '3') {
                    initGame(3);
                    InvalidateRect(hwnd, NULL, TRUE);
                } else if (wParam == 'Q' || wParam == 'q') {
                    PostQuitMessage(0);
                } else if (wParam == VK_UP && menuMode > 1) {
                    menuMode--;
                    InvalidateRect(hwnd, NULL, TRUE);
                } else if (wParam == VK_DOWN && menuMode < 4) {
                    menuMode++;
                    InvalidateRect(hwnd, NULL, TRUE);
                } else if (wParam == VK_RETURN) {
                    if (menuMode >= 1 && menuMode <= 3) {
                        initGame(menuMode);
                        InvalidateRect(hwnd, NULL, TRUE);
                    } else if (menuMode == 4) {
                        PostQuitMessage(0);
                    }
                }
            } else if (game.gameOver) {
                if (wParam == 'R' || wParam == 'r') {
                    menuMode = 1;
                    InvalidateRect(hwnd, NULL, TRUE);
                } else if (wParam == 'Q' || wParam == 'q') {
                    menuMode = 1;
                    InvalidateRect(hwnd, NULL, TRUE);
                }
            } else {
                if (wParam == 'P' || wParam == 'p') {
                    game.paused = !game.paused;
                    InvalidateRect(hwnd, NULL, TRUE);
                } else if (wParam == 'Q' || wParam == 'q') {
                    menuMode = 1;
                    InvalidateRect(hwnd, NULL, TRUE);
                } else if (!game.paused) {
                    switch (wParam) {
                        case 'W': case 'w': case VK_UP:
                            if (game.snake.dir != 3) game.snake.nextDir = 2;
                            break;
                        case 'S': case 's': case VK_DOWN:
                            if (game.snake.dir != 2) game.snake.nextDir = 3;
                            break;
                        case 'A': case 'a': case VK_LEFT:
                            if (game.snake.dir != 0) game.snake.nextDir = 1;
                            break;
                        case 'D': case 'd': case VK_RIGHT:
                            if (game.snake.dir != 1) game.snake.nextDir = 0;
                            break;
                    }
                }
            }
            break;
        }
        
        case WM_TIMER: {
            if (game.started && !game.gameOver && !game.paused) {
                DWORD now = GetTickCount();
                int speed = game.snake.baseSpeed;
                if (game.snake.isSpeedUp) speed = speed * 2 / 3;
                
                if (now - lastMoveTime >= speed) {
                    moveSnake();
                    spawnPoison();
                    if (game.mode == 3) {
                        spawnStones();
                        spawnWalls();
                        spawnSpeedPack();
                        spawnInvinciblePack();
                    }
                    updateItems();
                    InvalidateRect(hwnd, NULL, FALSE);
                    lastMoveTime = now;
                }
            }
            break;
        }
        
        case WM_CREATE: {
            SetTimer(hwnd, 1, 16, NULL);
            break;
        }
        
        case WM_DESTROY: {
            KillTimer(hwnd, 1);
            PostQuitMessage(0);
            break;
        }
        
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    return 0;
}
