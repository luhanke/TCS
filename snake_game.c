#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SNAKE_MAX_LENGTH 100
#define MAX_POISON 5
#define MAX_STONES 10
#define MAX_WALLS 20
#define MAX_POWERUPS 3

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point pos;
    int active;
    DWORD spawnTime;
    DWORD duration;
} Item;

typedef struct {
    Point body[SNAKE_MAX_LENGTH];
    int length;
    int dir;
    int nextDir;
    int score;
    int baseSpeed;
    int isSpeedUp;
    DWORD speedUpEndTime;
    int isInvincible;
    DWORD invincibleEndTime;
} Snake;

typedef struct {
    int width;
    int height;
    int mode;
    Snake snake;
    Point food;
    DWORD foodSpawnTime;
    Item poison[MAX_POISON];
    Item stones[MAX_STONES];
    Item walls[MAX_WALLS];
    Item speedPacks[MAX_POWERUPS];
    Item invinciblePacks[MAX_POWERUPS];
    int poisonCount;
    int stoneCount;
    int wallCount;
    int speedPackCount;
    int invinciblePackCount;
    int gameOver;
    int paused;
    int started;
} Game;

Game game;
HWND hWndHBRUSH brushGreen, brushDarkGreen, brushRed, brushBlue, brushYellow, brushGray, brushCyan, brushPurple, brushBg, brushBorder, brushWall;
HFONT hFontTitle, hFontMenu, hFontInfo, hFontGame;
int cellSize = 25;
DWORD lastMoveTime = 0;
int menuMode = 1;
int clientWidth = 900;
int clientHeight = 700;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void createGdiObjects() {
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

void deleteGdiObjects() {
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

void spawnFood() {
    int valid = 0;
    int x, y, i;
    
    while (!valid) {
        x = rand() % (game.width - 2) + 1;
        y = rand() % (game.height - 2) + 1;
        valid = 1;
        
        for (i = 0; i < game.snake.length; i++) {
            if (game.snake.body[i].x == x && game.snake.body[i].y == y) {
                valid = 0;
                break;
            }
        }
    }
    
    game.food.x = x;
    game.food.y = y;
    game.foodSpawnTime = GetTickCount();
}

void initGame(int mode) {
    memset(&game, 0, sizeof(Game));
    game.mode = mode;
    game.gameOver = 0;
    game.paused = 0;
    game.started = 1;
    menuMode = 0;
    
    if (mode == 1) {
        game.width = 20;
        game.height = 16;
        game.snake.baseSpeed = 180;
    } else if (mode == 2) {
        game.width = 24;
        game.height = 18;
        game.snake.baseSpeed = 140;
    } else {
        game.width = 28;
        game.height = 20;
        game.snake.baseSpeed = 110;
    }
    
    game.snake.length = 3;
    game.snake.body[0].x = game.width / 2;
    game.snake.body[0].y = game.height / 2;
    game.snake.body[1].x = game.snake.body[0].x - 1;
    game.snake.body[1].y = game.snake.body[0].y;
    game.snake.body[2].x = game.snake.body[0].x - 2;
    game.snake.body[2].y = game.snake.body[0].y;
    game.snake.dir = 0;
    game.snake.nextDir = 0;
    game.snake.score = 0;
    game.snake.isSpeedUp = 0;
    game.snake.isInvincible = 0;
    
    spawnFood();
    lastMoveTime = GetTickCount();
}

void spawnPoison() {
    if (game.mode == 2 && game.poisonCount < MAX_POISON && rand() % 40 == 0) {
        int i;
        for (i = 0; i < MAX_POISON; i++) {
            if (!game.poison[i].active) {
                int x, y, j;
                int valid = 0;
                
                while (!valid) {
                    x = rand() % (game.width - 2) + 1;
                    y = rand() % (game.height - 2) + 1;
                    valid = 1;
                    
                    for (j = 0; j < game.snake.length; j++) {
                        if (game.snake.body[j].x == x && game.snake.body[j].y == y) {
                            valid = 0;
                            break;
                        }
                    }
                    
                    if (x == game.food.x && y == game.food.y) {
                        valid = 0;
                    }
                }
                
                game.poison[i].pos.x = x;
                game.poison[i].pos.y = y;
                game.poison[i].active = 1;
                game.poison[i].spawnTime = GetTickCount();
                game.poison[i].duration = 5000;
                game.poisonCount++;
                break;
            }
        }
    }
}

void spawnStones() {
    if (game.mode == 3 && game.stoneCount < MAX_STONES && rand() % 12 == 0) {
        int i;
        for (i = 0; i < MAX_STONES; i++) {
            if (!game.stones[i].active) {
                int x, y, j;
                int valid = 0;
                
                while (!valid) {
                    x = rand() % (game.width - 2) + 1;
                    y = rand() % (game.height - 2) + 1;
                    valid = 1;
                    
                    for (j = 0; j < game.snake.length; j++) {
                        if (game.snake.body[j].x == x && game.snake.body[j].y == y) {
                            valid = 0;
                            break;
                        }
                    }
                    
                    if (x == game.food.x && y == game.food.y) {
                        valid = 0;
                    }
                }
                
                game.stones[i].pos.x = x;
                game.stones[i].pos.y = y;
                game.stones[i].active = 1;
                game.stones[i].spawnTime = GetTickCount();
                game.stoneCount++;
                break;
            }
        }
    }
}

void spawnWalls() {
    if (game.mode == 3 && game.wallCount < MAX_WALLS - 3 && rand() % 18 == 0) {
        int i;
        for (i = 0; i < MAX_WALLS - 3; i++) {
            if (!game.walls[i].active) {
                int x, y, j, len;
                int valid = 0;
                
                while (!valid) {
                    x = rand() % (game.width - 6) + 2;
                    y = rand() % (game.height - 6) + 2;
                    valid = 1;
                    
                    for (j = 0; j < game.snake.length; j++) {
                        if (abs(game.snake.body[j].x - x) <= 4 && abs(game.snake.body[j].y - y) <= 4) {
                            valid = 0;
                            break;
                        }
                    }
                }
                
                len = rand() % 3 + 2;
                for (j = 0; j < len && (i + j) < MAX_WALLS; j++) {
                    game.walls[i + j].pos.x = x + j;
                    game.walls[i + j].pos.y = y;
                    game.walls[i + j].active = 1;
                    game.walls[i + j].spawnTime = GetTickCount();
                    game.walls[i + j].duration = 15000;
                    game.wallCount++;
                }
                break;
            }
        }
    }
}

void spawnSpeedPack() {
    if (game.mode == 3 && game.speedPackCount < MAX_POWERUPS && rand() % 30 == 0) {
        int i;
        for (i = 0; i < MAX_POWERUPS; i++) {
            if (!game.speedPacks[i].active) {
                int x, y, j;
                int valid = 0;
                
                while (!valid) {
                    x = rand() % (game.width - 2) + 1;
                    y = rand() % (game.height - 2) + 1;
                    valid = 1;
                    
                    for (j = 0; j < game.snake.length; j++) {
                        if (game.snake.body[j].x == x && game.snake.body[j].y == y) {
                            valid = 0;
                            break;
                        }
                    }
                    
                    if (x == game.food.x && y == game.food.y) {
                        valid = 0;
                    }
                }
                
                game.speedPacks[i].pos.x = x;
                game.speedPacks[i].pos.y = y;
                game.speedPacks[i].active = 1;
                game.speedPacks[i].spawnTime = GetTickCount();
                game.speedPacks[i].duration = 10000;
                game.speedPackCount++;
                break;
            }
        }
    }
}

void spawnInvinciblePack() {
    if (game.mode == 3 && game.invinciblePackCount < MAX_POWERUPS && rand() % 40 == 0) {
        int i;
        for (i = 0; i < MAX_POWERUPS; i++) {
            if (!game.invinciblePacks[i].active) {
                int x, y, j;
                int valid = 0;
                
                while (!valid) {
                    x = rand() % (game.width - 2) + 1;
                    y = rand() % (game.height - 2) + 1;
                    valid = 1;
                    
                    for (j = 0; j < game.snake.length; j++) {
                        if (game.snake.body[j].x == x && game.snake.body[j].y == y) {
                            valid = 0;
                            break;
                        }
                    }
                    
                    if (x == game.food.x && y == game.food.y) {
                        valid = 0;
                    }
                }
                
                game.invinciblePacks[i].pos.x = x;
                game.invinciblePacks[i].pos.y = y;
                game.invinciblePacks[i].active = 1;
                game.invinciblePacks[i].spawnTime = GetTickCount();
                game.invinciblePacks[i].duration = 10000;
                game.invinciblePackCount++;
                break;
            }
        }
    }
}

void updateItems() {
    DWORD now = GetTickCount();
    int i;
    
    if (game.mode == 2) {
        for (i = 0; i < MAX_POISON; i++) {
            if (game.poison[i].active && now - game.poison[i].spawnTime > game.poison[i].duration) {
                game.poison[i].active = 0;
                game.poisonCount--;
            }
        }
    }
    
    if (game.mode == 3) {
        for (i = 0; i < MAX_STONES; i++) {
            if (game.stones[i].active && now - game.stones[i].spawnTime > 8000) {
                game.stones[i].active = 0;
                game.stoneCount--;
            }
        }
        
        for (i = 0; i < MAX_WALLS; i++) {
            if (game.walls[i].active && now - game.walls[i].spawnTime > game.walls[i].duration) {
                game.walls[i].active = 0;
                game.wallCount--;
            }
        }
        
        for (i = 0; i < MAX_POWERUPS; i++) {
            if (game.speedPacks[i].active && now - game.speedPacks[i].spawnTime > game.speedPacks[i].duration) {
                game.speedPacks[i].active = 0;
                game.speedPackCount--;
            }
            if (game.invinciblePacks[i].active && now - game.invinciblePacks[i].spawnTime > game.invinciblePacks[i].duration) {
                game.invinciblePacks[i].active = 0;
                game.invinciblePackCount--;
            }
        }
    }
    
    if (game.mode >= 2) {
        if (now - game.foodSpawnTime > 20000) {
            game.snake.score -= 5;
            if (game.snake.score < 0) game.snake.score = 0;
            spawnFood();
        }
    }
    
    if (game.snake.isSpeedUp && now > game.snake.speedUpEndTime) {
        game.snake.isSpeedUp = 0;
    }
    
    if (game.snake.isInvincible && now > game.snake.invincibleEndTime) {
        game.snake.isInvincible = 0;
    }
}

void moveSnake() {
    int i;
    Point newHead;
    Point oldHead = game.snake.body[0];
    
    game.snake.dir = game.snake.nextDir;
    
    newHead.x = oldHead.x;
    newHead.y = oldHead.y;
    
    switch (game.snake.dir) {
        case 0: newHead.x++; break;
        case 1: newHead.x--; break;
        case 2: newHead.y--; break;
        case 3: newHead.y++; break;
    }
    
    if (game.snake.isInvincible) {
        if (newHead.x < 0) newHead.x = game.width - 1;
        if (newHead.x >= game.width) newHead.x = 0;
        if (newHead.y < 0) newHead.y = game.height - 1;
        if (newHead.y >= game.height) newHead.y = 0;
    } else {
        if (newHead.x < 0 || newHead.x >= game.width || newHead.y < 0 || newHead.y >= game.height) {
            game.gameOver = 1;
            return;
        }
    }
    
    for (i = 0; i < game.snake.length; i++) {
        if (game.snake.body[i].x == newHead.x && game.snake.body[i].y == newHead.y) {
            game.gameOver = 1;
            return;
        }
    }
    
    if (game.mode == 3 && !game.snake.isInvincible) {
        for (i = 0; i < MAX_STONES; i++) {
            if (game.stones[i].active && game.stones[i].pos.x == newHead.x && game.stones[i].pos.y == newHead.y) {
                game.gameOver = 1;
                return;
            }
        }
        
        for (i = 0; i < MAX_WALLS; i++) {
            if (game.walls[i].active && game.walls[i].pos.x == newHead.x && game.walls[i].pos.y == newHead.y) {
                game.gameOver = 1;
                return;
            }
        }
    }
    
    for (i = game.snake.length - 1; i > 0; i--) {
        game.snake.body[i] = game.snake.body[i - 1];
    }
    game.snake.body[0] = newHead;
    
    if (newHead.x == game.food.x && newHead.y == game.food.y) {
        game.snake.score += 10;
        if (game.snake.length < SNAKE_MAX_LENGTH - 1) {
            game.snake.length++;
        }
        spawnFood();
    } else {
        if (game.mode == 2) {
            for (i = 0; i < MAX_POISON; i++) {
                if (game.poison[i].active && game.poison[i].pos.x == newHead.x && game.poison[i].pos.y == newHead.y) {
                    game.poison[i].active = 0;
                    game.poisonCount--;
                    game.gameOver = 1;
                    return;
                }
            }
        }
        
        if (game.mode == 3) {
            for (i = 0; i < MAX_POWERUPS; i++) {
                if (game.speedPacks[i].active && game.speedPacks[i].pos.x == newHead.x && game.speedPacks[i].pos.y == newHead.y) {
                    game.snake.isSpeedUp = 1;
                    game.snake.speedUpEndTime = GetTickCount() + 20000;
                    game.speedPacks[i].active = 0;
                    game.speedPackCount--;
                    break;
                }
                if (game.invinciblePacks[i].active && game.invinciblePacks[i].pos.x == newHead.x && game.invinciblePacks[i].pos.y == newHead.y) {
                    game.snake.isInvincible = 1;
                    game.snake.invincibleEndTime = GetTickCount() + 20000;
                    game.invinciblePacks[i].active = 0;
                    game.invinciblePackCount--;
                    break;
                }
            }
        }
    }
}

void drawGame(HDC hdc) {
    RECT rect;
    GetClientRect(hWnd, &rect);
    
    FillRect(hdc, &rect, brushBg);
    
    int gameAreaWidth = game.width * cellSize;
    int gameAreaHeight = game.height * cellSize;
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
                RECT cellBg = {startX + x * cellSize, startY + y * cellSize,
                              startX + (x + 1) * cellSize, startY + (y + 1) * cellSize};
                HBRUSH brushCellBg = CreateSolidBrush(RGB(22, 22, 44));
                FillRect(hdc, &cellBg, brushCellBg);
                DeleteObject(brushCellBg);
            }
        }
    }
    
    RECT foodRect = {startX + game.food.x * cellSize + 4, startY + game.food.y * cellSize + 4,
                     startX + (game.food.x + 1) * cellSize - 4, startY + (game.food.y + 1) * cellSize - 4};
    FillRect(hdc, &foodRect, brushYellow);
    HBRUSH brushFoodGlow = CreateSolidBrush(RGB(255, 255, 150));
    RECT foodGlow = {startX + game.food.x * cellSize + 2, startY + game.food.y * cellSize + 2,
                     startX + (game.food.x + 1) * cellSize - 2, startY + (game.food.y + 1) * cellSize - 2};
    FillRect(hdc, &foodGlow, brushFoodGlow);
    DeleteObject(brushFoodGlow);
    
    if (game.mode == 2) {
        for (int i = 0; i < MAX_POISON; i++) {
            if (game.poison[i].active) {
                RECT poisonRect = {startX + game.poison[i].pos.x * cellSize + 3,
                                  startY + game.poison[i].pos.y * cellSize + 3,
                                  startX + (game.poison[i].pos.x + 1) * cellSize - 3,
                                  startY + (game.poison[i].pos.y + 1) * cellSize - 3};
                FillRect(hdc, &poisonRect, brushPurple);
                
                SetBkMode(hdc, TRANSPARENT);
                SelectObject(hdc, hFontGame);
                SetTextColor(hdc, RGB(255, 255, 255));
                wchar_t poisonChar[] = L"X";
                DrawTextW(hdc, poisonChar, -1, &poisonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
        }
    }
    
    if (game.mode == 3) {
        for (int i = 0; i < MAX_STONES; i++) {
            if (game.stones[i].active) {
                RECT stoneRect = {startX + game.stones[i].pos.x * cellSize + 2,
                                  startY + game.stones[i].pos.y * cellSize + 2,
                                  startX + (game.stones[i].pos.x + 1) * cellSize - 2,
                                  startY + (game.stones[i].pos.y + 1) * cellSize - 2};
                FillRect(hdc, &stoneRect, brushGray);
                
                HBRUSH brushStoneHighlight = CreateSolidBrush(RGB(140, 140, 160));
                RECT stoneHighlight = {startX + game.stones[i].pos.x * cellSize + 3,
                                      startY + game.stones[i].pos.y * cellSize + 3,
                                      startX + game.stones[i].pos.x * cellSize + 8,
                                      startY + game.stones[i].pos.y * cellSize + 8};
                FillRect(hdc, &stoneHighlight, brushStoneHighlight);
                DeleteObject(brushStoneHighlight);
            }
        }
        
        for (int i = 0; i < MAX_WALLS; i++) {
            if (game.walls[i].active) {
                RECT wallRect = {startX + game.walls[i].pos.x * cellSize,
                                startY + game.walls[i].pos.y * cellSize,
                                startX + (game.walls[i].pos.x + 1) * cellSize,
                                startY + (game.walls[i].pos.y + 1) * cellSize};
                FillRect(hdc, &wallRect, brushWall);
                
                HBRUSH brushFrame = CreateSolidBrush(RGB(100, 100, 150));
                FrameRect(hdc, &wallRect, brushFrame);
                DeleteObject(brushFrame);
            }
        }
        
        for (int i = 0; i < MAX_POWERUPS; i++) {
            if (game.speedPacks[i].active) {
                RECT speedRect = {startX + game.speedPacks[i].pos.x * cellSize + 3,
                                  startY + game.speedPacks[i].pos.y * cellSize + 3,
                                  startX + (game.speedPacks[i].pos.x + 1) * cellSize - 3,
                                  startY + (game.speedPacks[i].pos.y + 1) * cellSize - 3};
                FillRect(hdc, &speedRect, brushCyan);
                
                SetBkMode(hdc, TRANSPARENT);
                SelectObject(hdc, hFontGame);
                SetTextColor(hdc, RGB(255, 255, 255));
                wchar_t speedChar[] = L">";
                RECT speedTextRect = {startX + game.speedPacks[i].pos.x * cellSize,
                                      startY + game.speedPacks[i].pos.y * cellSize,
                                      startX + (game.speedPacks[i].pos.x + 1) * cellSize,
                                      startY + (game.speedPacks[i].pos.y + 1) * cellSize};
                DrawTextW(hdc, speedChar, -1, &speedTextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
            if (game.invinciblePacks[i].active) {
                RECT invRect = {startX + game.invinciblePacks[i].pos.x * cellSize + 3,
                               startY + game.invinciblePacks[i].pos.y * cellSize + 3,
                               startX + (game.invinciblePacks[i].pos.x + 1) * cellSize - 3,
                               startY + (game.invinciblePacks[i].pos.y + 1) * cellSize - 3};
                FillRect(hdc, &invRect, brushBlue);
                
                SetBkMode(hdc, TRANSPARENT);
                SelectObject(hdc, hFontGame);
                SetTextColor(hdc, RGB(255, 255, 255));
                wchar_t invChar[] = L"I";
                RECT invTextRect = {startX + game.invinciblePacks[i].pos.x * cellSize,
                                    startY + game.invinciblePacks[i].pos.y * cellSize,
                                    startX + (game.invinciblePacks[i].pos.x + 1) * cellSize,
                                    startY + (game.invinciblePacks[i].pos.y + 1) * cellSize};
                DrawTextW(hdc, invChar, -1, &invTextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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
        
        RECT bodyRect = {startX + sx * cellSize + 2, startY + sy * cellSize + 2,
                        startX + (sx + 1) * cellSize - 2, startY + (sy + 1) * cellSize - 2};
        FillRect(hdc, &bodyRect, snakeBrush);
        
        if (i == 0) {
            int eyeSize = cellSize / 5;
            int eyeOffset = cellSize / 4;
            int eyeInset = cellSize / 8;
            
            HBRUSH brushEyeWhite = CreateSolidBrush(RGB(255, 255, 255));
            SelectObject(hdc, brushEyeWhite);
            
            int dirOffsetX1 = 0, dirOffsetY1 = 0, dirOffsetX2 = 0, dirOffsetY2 = 0;
            switch (game.snake.dir) {
                case 0: dirOffsetX1 = eyeOffset; dirOffsetX2 = eyeOffset; break;
                case 1: dirOffsetX1 = -eyeOffset + cellSize; dirOffsetX2 = -eyeOffset + cellSize; break;
                case 2: dirOffsetY1 = eyeOffset; dirOffsetY2 = eyeOffset; break;
                case 3: dirOffsetY1 = -eyeOffset + cellSize; dirOffsetY2 = -eyeOffset + cellSize; break;
            }
            
            if (game.snake.dir < 2) {
                Ellipse(hdc, startX + sx * cellSize + dirOffsetX1, startY + sy * cellSize + eyeInset,
                       startX + sx * cellSize + dirOffsetX1 + eyeSize, startY + sy * cellSize + eyeInset + eyeSize);
                Ellipse(hdc, startX + sx * cellSize + dirOffsetX2, startY + sy * cellSize + cellSize - eyeInset - eyeSize,
                       startX + sx * cellSize + dirOffsetX2 + eyeSize, startY + sy * cellSize + cellSize - eyeInset);
            } else {
                Ellipse(hdc, startX + sx * cellSize + eyeInset, startY + sy * cellSize + dirOffsetY1,
                       startX + sx * cellSize + eyeInset + eyeSize, startY + sy * cellSize + dirOffsetY1 + eyeSize);
                Ellipse(hdc, startX + sx * cellSize + cellSize - eyeInset - eyeSize, startY + sy * cellSize + dirOffsetY2,
                       startX + sx * cellSize + cellSize - eyeInset, startY + sy * cellSize + dirOffsetY2 + eyeSize);
            }
            
            DeleteObject(brushEyeWhite);
        }
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
    const wchar_t* menuDesc[] = {L"基础玩法，休闲娱乐", L"毒苹果，食物限时", L"石头石墙，技能道具"};
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
    wchar_t controls[] = L"操作说明: WASD 或 方向键 选择/移动  |  Enter 确认  |  Q 退出";
    TextOutW(hdc, centerX - 280, 500, controls, wcslen(controls));
    
    RECT legendBg = {centerX - 250, 540, centerX + 250, 620};
    HBRUSH brushLegendBg = CreateSolidBrush(RGB(15, 15, 30));
    FillRect(hdc, &legendBg, brushLegendBg);
    DeleteObject(brushLegendBg);
    
    struct {
        HBRUSH brush;
        const wchar_t* text;
    } legend[] = {
        {brushYellow, L"食物 +10分"},
        {brushPurple, L"毒苹果 结束游戏"},
        {brushGray, L"石头 结束游戏"},
        {brushCyan, L"加速包"},
        {brushBlue, L"无敌包"}
    };
    
    for (int i = 0; i < 5; i++) {
        RECT colorBox = {centerX - 230 + (i % 5) * 100, 545, centerX - 210 + (i % 5) * 100, 565};
        FillRect(hdc, &colorBox, legend[i].brush);
        
        SetTextColor(hdc, RGB(180, 180, 180));
        RECT textRect = {centerX - 230 + (i % 5) * 100, 570, centerX - 120 + (i % 5) * 100, 590};
        DrawTextW(hdc, legend[i].text, -1, &textRect, DT_CENTER | DT_VCENTER);
    }
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
    
    HBRUSH brushOverlayBorder = CreateSolidBrush(RGB(150, 50, 50));
    FrameRect(hdc, &overlayBg, brushOverlayBorder);
    DeleteObject(brushOverlayBorder);
    
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
    
    HBRUSH brushOverlayBorder = CreateSolidBrush(RGB(80, 120, 200));
    FrameRect(hdc, &overlayBg, brushOverlayBorder);
    DeleteObject(brushOverlayBorder);
    
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
        CW_USEDEFAULT, CW_USEDEFAULT,
        clientWidth, clientHeight,
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
            } else if (game.gameOver) {
                int centerY = 350;
                if (mouseX >= centerX - 200 && mouseX <= centerX + 200 &&
                    mouseY >= centerY + 30 && mouseY <= centerY + 110) {
                    menuMode = 1;
                    InvalidateRect(hwnd, NULL, TRUE);
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
                        case 'W':
                        case 'w':
                        case VK_UP:
                            if (game.snake.dir != 3) game.snake.nextDir = 2;
                            break;
                        case 'S':
                        case 's':
                        case VK_DOWN:
                            if (game.snake.dir != 2) game.snake.nextDir = 3;
                            break;
                        case 'A':
                        case 'a':
                        case VK_LEFT:
                            if (game.snake.dir != 0) game.snake.nextDir = 1;
                            break;
                        case 'D':
                        case 'd':
                        case VK_RIGHT:
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
                if (game.snake.isSpeedUp) {
                    speed = speed * 2 / 3;
                }
                
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
