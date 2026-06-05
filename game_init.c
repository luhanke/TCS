/******************************************************************
 *                    贪吃蛇游戏 - 游戏初始化                        *
 *                    Snake Game - Game Initialization               *
 ******************************************************************/

#include "snake_game.h"
#include <stdlib.h>

void spawnFood(void) {
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
    
    spawnFood();
    lastMoveTime = GetTickCount();
}
