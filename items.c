/******************************************************************
 *                    贪吃蛇游戏 - 物品生成                          *
 *                    Snake Game - Item Spawning                    *
 ******************************************************************/

#include "snake_game.h"

void spawnPoison(void) {
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

void spawnStones(void) {
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

void spawnWalls(void) {
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

void spawnSpeedPack(void) {
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
                game.speedPackCount++;
                break;
            }
        }
    }
}

void spawnInvinciblePack(void) {
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
                game.invinciblePackCount++;
                break;
            }
        }
    }
}
