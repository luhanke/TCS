/******************************************************************
 *                    贪吃蛇游戏 - 全局变量                          *
 *                    Snake Game - Global Variables                  *
 ******************************************************************/

#include "snake_game.h"

Game game;
HWND hWnd;
HBRUSH brushGreen, brushDarkGreen, brushRed, brushBlue, brushYellow;
HBRUSH brushGray, brushCyan, brushPurple, brushBg, brushBorder, brushWall;
HFONT hFontTitle, hFontMenu, hFontInfo, hFontGame;
DWORD lastMoveTime = 0;
int menuMode = 1;
