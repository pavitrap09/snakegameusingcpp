#include <conio.h>
#include <iostream>
#include <windows.h>
#include <vector>
#include <sstream> // For stringstream
using namespace std;

// Constants for game dimensions
const int width = 80;
const int height = 20;

// Enum for directions
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

// Base class for GameObject to demonstrate inheritance
class GameObject {
public:
    virtual void Move() = 0;
    virtual bool CheckCollision() = 0;
};

class Fruit : public GameObject {
public:
    int x, y;
    Fruit() {
        Respawn();
    }
    void Respawn() {
        x = rand() % width;
        y = rand() % height;
    }
    void Move() override {} // No movement logic for fruit
    bool CheckCollision() override { return false; } // No collision logic for fruit
};

class Snake : public GameObject {
public:
    int x, y;
    vector<int> tailX, tailY;
    Direction dir;

    Snake() {
        x = width / 2;
        y = height / 2;
        dir = STOP;
    }

    void Move() override {
        // Move the tail
        if (!tailX.empty()) {
            for (size_t i = tailX.size() - 1; i > 0; --i) {
                tailX[i] = tailX[i - 1];
                tailY[i] = tailY[i - 1];
            }
            tailX[0] = x;
            tailY[0] = y;
        }

        // Move the head
        switch (dir) {
            case LEFT: x--; break;
            case RIGHT: x++; break;
            case UP: y--; break;
            case DOWN: y++; break;
            default: break;
        }
    }

    bool CheckCollision() override {
        if (x >= width || x < 0 || y >= height || y < 0)
            return true;
        for (size_t i = 0; i < tailX.size(); ++i) {
            if (tailX[i] == x && tailY[i] == y)
                return true;
        }
        return false;
    }

    void Grow() {
        if (!tailX.empty()) {
            tailX.push_back(tailX.back());
            tailY.push_back(tailY.back());
        } else {
            tailX.push_back(x);
            tailY.push_back(y);
        }
    }
};

class Game {
private:
    Snake* snake;
    Fruit* fruit;
    bool isGameOver;
    int playerScore;
    int difficulty;
    string playerName;

public:
    Game() {
        isGameOver = false;
        playerScore = 0;
        snake = new Snake();
        fruit = new Fruit();
        difficulty = SetDifficulty();
    }

    ~Game() {
        delete snake;
        delete fruit;
    }

    void SetPlayerName() {
        cout << "Enter your name: ";
        cin >> playerName;
    }

    int SetDifficulty() {
        int choice;
        cout << "\nSET DIFFICULTY\n1: Easy\n2: Medium\n3: Hard\n"
             << "NOTE: if not chosen or pressed any other key, the difficulty will be automatically set to medium\n"
             << "Choose difficulty level: ";
        cin >> choice;
        switch (choice) {
        case 1: return 150; // Easy
        case 2: return 100; // Medium
        case 3: return 50;  // Hard
        default: return 100; // Default to Medium
        }
    }

    void Render() {
        // Use a stringstream to build the screen in memory
        stringstream ss;

        for (int i = 0; i < width + 2; i++) ss << "-";
        ss << endl;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j <= width; j++) {
                if (j == 0 || j == width) ss << "|";
                else if (i == snake->y && j == snake->x) ss << "O";
                else if (i == fruit->y && j == fruit->x) ss << "#";
                else {
                    bool printTail = false;
                    for (size_t k = 0; k < snake->tailX.size(); k++) {
                        if (snake->tailX[k] == j && snake->tailY[k] == i) {
                            ss << "o";
                            printTail = true;
                        }
                    }
                    if (!printTail) ss << " ";
                }
            }
            ss << endl;
        }

        for (int i = 0; i < width + 2; i++) ss << "-";
        ss << endl;

        ss << playerName << "'s Score: " << playerScore << endl;

        // Clear the console and output the built screen
        system("cls");
        cout << ss.str();
    }

    void Input() {
        if (_kbhit()) {
            switch (_getch()) {
                case 'a': if (snake->dir != RIGHT) snake->dir = LEFT; break;
                case 'd': if (snake->dir != LEFT) snake->dir = RIGHT; break;
                case 'w': if (snake->dir != DOWN) snake->dir = UP; break;
                case 's': if (snake->dir != UP) snake->dir = DOWN; break;
                case 'x': isGameOver = true; break;
            }
        }
    }

    void Update() {
        snake->Move();
        if (snake->CheckCollision()) isGameOver = true;
        if (snake->x == fruit->x && snake->y == fruit->y) {
            playerScore += 10;
            fruit->Respawn();
            snake->Grow();
        }
    }

    void Run() {
        SetPlayerName();
        while (!isGameOver) {
            Render();
            Input();
            Update();
            Sleep(difficulty);
        }
    }
};

int main() {
    Game game;
    game.Run();
    return 0;
}