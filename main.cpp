#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <deque>

static const int SCREEN_WIDTH = sf::VideoMode::getDesktopMode().width;
static const int SCREEN_HEIGHT = sf::VideoMode::getDesktopMode().height;
static const int CELL_SIZE = 90;
static const float SNAKE_SPEED = 0.2f;

enum cellState {
    empty,
    fruit,
    snake,
    wall
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct HeadCoords {
    int x;
    int y;
};

class Snake {
public:
    HeadCoords position;
    Direction currDirection;

    HeadCoords getHeadPos() {
        return position;
    }
};

class Board {
public:
    std::unique_ptr<std::unique_ptr<cellState[]>[]> grid;
    int rows;
    int cols;
    Snake sn;
    HeadCoords fruitCoord;
    std::deque<HeadCoords> segments;

    Board() {
        rows = SCREEN_HEIGHT / CELL_SIZE;
        cols = SCREEN_WIDTH / CELL_SIZE;

        grid = std::make_unique<std::unique_ptr<cellState[]>[]>(rows);
        for (int i = 0; i < rows; ++i) {
            grid[i] = std::make_unique<cellState[]>(cols);
        }

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                grid[i][j] = empty;
            }
        }

        for (int i = 0; i < rows; ++i) {
            grid[i][0] = wall;
            grid[i][cols - 1] = wall;
        }
        for (int i = 0; i < cols; ++i) {
            grid[0][i] = wall;
            grid[rows - 1][i] = wall;
        }

        grid[rows / 2][cols / 2] = snake;
        HeadCoords init;
        init.x = rows / 2;
        init.y = cols / 2;
        segments.push_back(init);
        sn.position.x = rows / 2;
        sn.position.y = cols / 2;
        sn.currDirection = RIGHT;

        std::srand(std::time(nullptr));
        placeNewFruit();
    }

    void draw(sf::RenderWindow& window) {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                sf::RectangleShape shape(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                shape.setPosition(j * CELL_SIZE, i * CELL_SIZE);
                shape.setOutlineThickness(1);
                shape.setOutlineColor(sf::Color::Black);
                switch (grid[i][j]) {
                    case empty:
                        shape.setFillColor(sf::Color::Green);
                        break;
                    case wall:
                        shape.setFillColor(sf::Color(150, 75, 0));
                        break;
                    case fruit:
                        shape.setFillColor(sf::Color::Red);
                        break;
                    case snake:
                        shape.setFillColor(sf::Color::Blue);
                        break;
                    default:
                        shape.setFillColor(sf::Color::Black);
                        break;
                }
                window.draw(shape);
            }
        }
    }

    bool setSnakePos(int x, int y) {
        if (grid[x][y] == wall || grid[x][y] == snake) {
            return false;
        }

        if (grid[x][y] == fruit) {
            placeNewFruit();
        } else {
            HeadCoords prev = segments.back();
            segments.pop_back();
            grid[prev.x][prev.y] = empty;
        }

        HeadCoords newSegment;
        newSegment.x = x;
        newSegment.y = y;
        segments.push_front(newSegment);
        grid[x][y] = snake;
        sn.position.x = x;
        sn.position.y = y;

        return true;
    }

    bool setSnakeNew() {
        int x = getSnakePos().x;
        int y = getSnakePos().y;
        switch (sn.currDirection) {
            case UP:
                return setSnakePos(x - 1, y);
            case DOWN:
                return setSnakePos(x + 1, y);
            case LEFT:
                return setSnakePos(x, y - 1);
            case RIGHT:
                return setSnakePos(x, y + 1);
            default:
                return true;
        }
    }

    HeadCoords getSnakePos() {
        return sn.position;
    }

    void setFruitPos(int x, int y) {
        if (x >= 0 && x < rows && y >= 0 && y < cols && grid[x][y] == empty) {
            grid[x][y] = fruit;
            fruitCoord.x = x;
            fruitCoord.y = y;
        }
    }

    void placeNewFruit() {
        int x, y;
        do {
            x = std::rand() % rows;
            y = std::rand() % cols;
        } while (grid[x][y] != empty);
        setFruitPos(x, y);
    }

    void setSnakeDir(Direction newDirection) {
        if ((newDirection == UP && sn.currDirection != DOWN) ||
            (newDirection == DOWN && sn.currDirection != UP) ||
            (newDirection == LEFT && sn.currDirection != RIGHT) ||
            (newDirection == RIGHT && sn.currDirection != LEFT)) {
            sn.currDirection = newDirection;
        }
    }
};

int main() {
    Board board;

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Snake", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    sf::Clock clock;
    sf::Time elapsed = sf::Time::Zero;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        elapsed += clock.restart();

        if (elapsed.asSeconds() >= SNAKE_SPEED) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                board.setSnakeDir(LEFT);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                board.setSnakeDir(RIGHT);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                board.setSnakeDir(UP);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                board.setSnakeDir(DOWN);
            }

            bool continueGame = board.setSnakeNew();
            if (!continueGame) {
                window.close(); 
            }

            elapsed = sf::Time::Zero;
        }

        window.clear();
        board.draw(window);
        window.display();
    }

    return 0;
}

