#include <iostream>
#include <vector>
#include <utility>
#include <ncurses.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <fstream>

using namespace std;

enum Direction { STOP = 0, UP, DOWN, LEFT, RIGHT };

class Snake {
private:
    vector<pair<int, int>> body;
    Direction dir;
    int speed;

public:
    Snake(int startX, int startY) {
        body.push_back(make_pair(startX, startY));
        dir = STOP;
        speed = 1;
    }

    void changeDirection(Direction newDir) {
        if ((dir == UP && newDir != DOWN) ||
            (dir == DOWN && newDir != UP) ||
            (dir == LEFT && newDir != RIGHT) ||
            (dir == RIGHT && newDir != LEFT) ||
            (dir == STOP))
            dir = newDir;
    }

    void move() {
        for (int i = body.size() - 1; i > 0; i--) {
            body[i].first = body[i - 1].first;
            body[i].second = body[i - 1].second;
        }

        switch (dir) {
        case UP:
            body[0].second--;
            break;
        case DOWN:
            body[0].second++;
            break;
        case LEFT:
            body[0].first--;
            break;
        case RIGHT:
            body[0].first++;
            break;
        }
    }

    void grow() {
        body.push_back(make_pair(body.back().first, body.back().second));
    }

    bool checkCollision(int width, int height) {
        if (body[0].first <= 0 || body[0].first >= width - 1 ||
            body[0].second <= 0 || body[0].second >= height - 1)
            return true;

        for (int i = 1; i < body.size(); i++) {
            if (body[0].first == body[i].first && body[0].second == body[i].second)
                return true;
        }

        return false;
    }

    vector<pair<int, int>> getBody() const { return body; }
    Direction getDirection() const { return dir; }
    pair<int, int> getHead() const { return body[0]; }
};

class Food {
private:
    pair<int, int> position;
    int width, height;

public:
    Food(int w, int h) : width(w), height(h) {
        srand(time(NULL));
        position.first = rand() % (width - 2) + 1;
        position.second = rand() % (height - 2) + 1;
    }

    void generateNew() {
        position.first = rand() % (width - 2) + 1;
        position.second = rand() % (height - 2) + 1;
    }

    pair<int, int> getPosition() const { return position; }
};

class GameController {
private:
    int width, height;
    int score;
    bool gameOver;
    Snake snake;
    Food food;
    int highScore;

    void drawBorder() {
        for (int i = 0; i < width; i++) {
            mvprintw(0, i, "#");
            mvprintw(height-1, i, "#");
        }
        for (int i = 1; i < height-1; i++) {
            mvprintw(i, 0, "#");
            mvprintw(i, width-1, "#");
        }
    }

public:
    GameController(int w, int h) : width(w), height(h), snake(w / 2, h / 2), food(w, h) {
        score = 0;
        gameOver = false;
        highScore = loadHighScore();
    }

    void draw() {
        clear();
        drawBorder();

        // رسم الطعام
        mvprintw(food.getPosition().second, food.getPosition().first, "F");

        // رسم الثعبان
        auto body = snake.getBody();
        for (size_t i = 0; i < body.size(); i++) {
            if (i == 0)
                mvprintw(body[i].second, body[i].first, "O");
            else
                mvprintw(body[i].second, body[i].first, "o");
        }

        // عرض النقاط
        mvprintw(height, 0, "Score: %d | High Score: %d", score, highScore);
        mvprintw(height+1, 0, "Controls: W (Up), A (Left), S (Down), D (Right), P (Pause), X (Exit)");
        refresh();
    }

    void input() {
        timeout(100);
        int ch = getch();
        switch (ch) {
        case 'w': snake.changeDirection(UP); break;
        case 's': snake.changeDirection(DOWN); break;
        case 'a': snake.changeDirection(LEFT); break;
        case 'd': snake.changeDirection(RIGHT); break;
        case 'p': pauseGame(); break;
        case 'x': gameOver = true; break;
        }
    }

    void logic() {
        snake.move();

        if (snake.getHead() == food.getPosition()) {
            snake.grow();
            food.generateNew();
            score += 10;
            if (score > highScore) {
                highScore = score;
                saveHighScore();
            }
        }

        if (snake.checkCollision(width, height))
            gameOver = true;
    }

    void pauseGame() {
        mvprintw(height/2, width/2 - 10, "Game Paused. Press any key to continue...");
        refresh();
        timeout(-1);
        getch();
        timeout(100);
    }

    int loadHighScore() {
        ifstream file("highscore.txt");
        if (file.is_open()) {
            int score;
            file >> score;
            file.close();
            return score;
        }
        return 0;
    }

    void saveHighScore() {
        ofstream file("highscore.txt");
        if (file.is_open()) {
            file << highScore;
            file.close();
        }
    }

    void run() {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0);

        while (!gameOver) {
            draw();
            input();
            logic();
            usleep(100000);
        }

        endwin();
        cout << "Game Over! Final Score: " << score << endl;
    }
};

int main() {
    const int WIDTH = 40;
    const int HEIGHT = 20;

    GameController game(WIDTH, HEIGHT);
    game.run();

    return 0;
}
