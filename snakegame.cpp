#include <iostream>
#include "console.h"
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace console;

#define BOARD_WIDTH 15
#define BOARD_HEIGHT 15
#define MOVE_DELAY 15
#define WALL_VERTICAL_STRING "┃"
#define WALL_HORIZONTAL_STRING "━"
#define WALL_RIGHT_TOP_STRING "┓"
#define WALL_LEFT_TOP_STRING "┏"
#define WALL_RIGHT_BOTTOM_STRING "┛"
#define WALL_LEFT_BOTTOM_STRING "┗"
#define SNAKE_STRING "■"
#define SNAKE_BODY_STRING "■"
#define APPLE_STRING "●"
#define SCORE_POSITION_X (BOARD_WIDTH / 2)
#define SCORE_POSITION_Y (BOARD_HEIGHT + 1)
#define MAX_SNAKE_LENGTH ((BOARD_WIDTH - 2) * (BOARD_HEIGHT - 2))

int appleX = 0;
int appleY = 0;
int score = 0;
int snakeX[MAX_SNAKE_LENGTH];
int snakeY[MAX_SNAKE_LENGTH];
int snakeLength = 1; // 뱀의 초기 길이는 1

enum Direction { UP, DOWN, LEFT, RIGHT };
Direction snakeDirection = RIGHT;

bool gameOver = false;

bool isSnakeBody(int x, int y) {
    for (int i = 0; i < snakeLength; ++i) {
        if (snakeX[i] == x && snakeY[i] == y) {
            return true;
        }
    }
    return false;
}

void generateApplePosition() {
    int emptyCellsX[MAX_SNAKE_LENGTH];
    int emptyCellsY[MAX_SNAKE_LENGTH];
    int emptyCellsCount = 0;

    // 뱀이 없는 위치를 저장
    for (int x = 1; x < BOARD_WIDTH - 1; ++x) {
        for (int y = 1; y < BOARD_HEIGHT - 1; ++y) {
            if (!isSnakeBody(x, y)) {
                emptyCellsX[emptyCellsCount] = x;
                emptyCellsY[emptyCellsCount] = y;
                emptyCellsCount++;
            }
        }
    }

    // 랜덤한 위치에서 사과 생성
    int randomIndex = rand() % emptyCellsCount;
    appleX = emptyCellsX[randomIndex];
    appleY = emptyCellsY[randomIndex];
}

void handleInput() {
    if (console::key(K_LEFT) && snakeDirection != RIGHT)
        snakeDirection = LEFT;
    if (console::key(K_RIGHT) && snakeDirection != LEFT)
        snakeDirection = RIGHT;
    if (console::key(K_UP) && snakeDirection != DOWN)
        snakeDirection = UP;
    if (console::key(K_DOWN) && snakeDirection != UP)
        snakeDirection = DOWN;

    // ESC 키가 눌렸는지 확인하여 게임을 종료
    if (console::key(K_ESC)) {
        console::cls();
        console::draw(BOARD_WIDTH / 2 - 5, BOARD_HEIGHT / 2, "Game Over");
        console::wait();
        exit(0);
    }
}

void moveSnake() {
    // 뱀 꼬리 움직이기
    for (int i = snakeLength - 1; i > 0; --i) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    // 방향키 입력을 받아 뱀의 머리 움직이기
    switch (snakeDirection) {
    case UP:
        snakeY[0]--;
        break;
    case DOWN:
        snakeY[0]++;
        break;
    case LEFT:
        snakeX[0]--;
        break;
    case RIGHT:
        snakeX[0]++;
        break;
    }

    // 벽에 박으면 gameover
    if (snakeX[0] <= 0 || snakeX[0] >= BOARD_WIDTH - 1 || snakeY[0] <= 0 || snakeY[0] >= BOARD_HEIGHT - 1) {
        // Game over
        gameOver = true;
        return;
    }

    // 사과를 먹으면 +10점, 몸통추가
    if (snakeX[0] == appleX && snakeY[0] == appleY) {
        // 점수추가
        score += 10;
        // 몸통추가
        snakeLength++;
        // 새로운 사과 생성
        generateApplePosition();
    }

    // 뱀의 몸통에 박으면 gameover
    for (int i = 1; i < snakeLength; ++i) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            gameOver = true;
            return;
        }
    }
}

bool checkWin() {
    // 뱀의 길이가 최대 길이에 도달하면 승리
    return snakeLength == MAX_SNAKE_LENGTH;
}

void drawSnakeAndApple() {
    console::clear();
    // 벽 만들기
    console::draw(0, 0, WALL_LEFT_TOP_STRING);
    console::draw(BOARD_WIDTH - 1, 0, WALL_RIGHT_TOP_STRING);
    console::draw(0, BOARD_HEIGHT - 1, WALL_LEFT_BOTTOM_STRING);
    console::draw(BOARD_WIDTH - 1, BOARD_HEIGHT - 1, WALL_RIGHT_BOTTOM_STRING);
    for (int i = 1; i < BOARD_WIDTH - 1; ++i) {
        console::draw(i, 0, WALL_HORIZONTAL_STRING);
        console::draw(i, BOARD_HEIGHT - 1, WALL_HORIZONTAL_STRING);
    }
    for (int i = 1; i < BOARD_HEIGHT - 1; ++i) {
        console::draw(0, i, WALL_VERTICAL_STRING);
        console::draw(BOARD_WIDTH - 1, i, WALL_VERTICAL_STRING);
    }
    // 뱀 생성
    for (int i = 0; i < snakeLength; ++i) {
        console::draw(snakeX[i], snakeY[i], SNAKE_BODY_STRING);
    }
    // 사과 생성
    console::draw(appleX, appleY, APPLE_STRING);
    // 점수판 생성
    string scoreString = "Score: " + to_string(score);
    console::draw(SCORE_POSITION_X - scoreString.length() / 2, SCORE_POSITION_Y, scoreString);

    // 게임 상태에 따른 메시지 출력
    if (gameOver) {
        console::draw(BOARD_WIDTH / 2 - 4, BOARD_HEIGHT / 2, "You lose!");
        console::draw(BOARD_WIDTH / 2 - 7, BOARD_HEIGHT / 2 + 1, "Try again?(Enter)");
    } else if (checkWin()) {
        console::draw(BOARD_WIDTH / 2 - 4, BOARD_HEIGHT / 2, "You win!");
        console::draw(BOARD_WIDTH / 2 - 7, BOARD_HEIGHT / 2 + 1, "Try again?(Enter)");
    }
}

int main() {
    // 무작위 시드 초기화
    srand(time(NULL));

    // 콘솔 초기화
    console::init();

    // 사과 생성
    generateApplePosition();

    // 게임 시작
    while (true) {
        if (!gameOver) {
            handleInput();
            moveSnake();
        }
        drawSnakeAndApple();
        for (int i = 0; i < MOVE_DELAY; ++i) {
            console::wait();
        }
        if (gameOver || checkWin()) {
            if (console::key(K_ENTER)) {
                // 엔터키를 누르면 게임 초기화
                score = 0;
                snakeLength = 1;
                snakeX[0] = BOARD_WIDTH / 2;
                snakeY[0] = BOARD_HEIGHT / 2;
                generateApplePosition();
                gameOver = false;
            } else if (console::key(K_ESC)) {
                // ESC를 누르면 종료
                exit(0);
            }
        }
    }

    return 0;
}

