/******************************************************************
 *                    贪吃蛇游戏 - 游戏逻辑                          *
 *                    Snake Game - Game Logic                        *
 ******************************************************************/

#include "snake_game.h"

void updateItems(void) {
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
            if (game.speedPacks[i].active && now - game.speedPacks[i].spawnTime > 10000) {
                game.speedPacks[i].active = 0;
                game.speedPackCount--;
            }
            if (game.invinciblePacks[i].active && now - game.invinciblePacks[i].spawnTime > 10000) {
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

void moveSnake(void) {
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
