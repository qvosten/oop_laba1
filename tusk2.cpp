#include <iostream>
#include <vector>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <random>
#include <stack>
#include <queue>


enum class Direction {
    Up,
    Down,
    Left,
    Right
};


struct Position {
    int x{ 0 };
    int y{ 0 };

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Position& other) const {
        return !(*this == other);
    }

    bool operator<(const Position& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

template <typename T>
class MazeCell {
private:
    Position pos;
    T data;
    bool has_passage[4]{ false, false, false, false };
public:
    MazeCell(Position pos, T data = T()) : pos(pos), data(data) {}

    Position get_position() const { return pos; }

    T get_data() const { return data; }
    void set_data(const T& new_data) { data = new_data; }

    bool can_move(Direction dir) const {
        return has_passage[static_cast<int>(dir)];
    }

    void set_passage(Direction dir, bool state = true) {
        has_passage[static_cast<int>(dir)] = state;
    }
};

template <typename T>
class Maze {
private:
    int width;
    int height;
    std::vector<std::vector<MazeCell<T>>> grid;

public:
    Maze(int w, int h, const T& default_data = T()) : width(w), height(h) {
        if (w < 2 || h < 2) {
            throw std::invalid_argument("Maze dimensions must be at least 2x2.");
        }

        grid.reserve(height);
        for (int y = 0; y < height; ++y) {
            std::vector<MazeCell<T>> row;
            row.reserve(width);
            for (int x = 0; x < width; ++x) {
                row.emplace_back(Position{ x, y }, default_data);
            }
            grid.push_back(row);
        }
    }

    int get_width() const {
        return width;
    }
    int get_height() const {
        return height;
    }

    bool is_valid_position(Position pos) const {
        return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height;
    }

    MazeCell<T>& get_cell(Position pos) {
        if (!is_valid_position(pos)) {
            throw std::out_of_range("Position out of maze bounds.");
        }
        return grid[pos.y][pos.x];
    }

    const MazeCell<T>& get_cell(Position pos) const {
        if (!is_valid_position(pos)) {
            throw std::out_of_range("Position out of maze bounds.");
        }
        return grid[pos.y][pos.x];
    }

    Position get_neighbor_pos(Position pos, Direction dir) const {
        switch (dir) {
        case Direction::Up:    return Position{ pos.x, pos.y - 1 };
        case Direction::Down:  return Position{ pos.x, pos.y + 1 };
        case Direction::Left:  return Position{ pos.x - 1, pos.y };
        case Direction::Right: return Position{ pos.x + 1, pos.y };
        }
        return pos;
    }

    Direction get_opposite_direction(Direction dir) {
        switch (dir) {
        case Direction::Up:    return Direction::Down;
        case Direction::Down:  return Direction::Up;
        case Direction::Left:  return Direction::Right;
        case Direction::Right: return Direction::Left;
        }
        return dir;
    }

    void add_passage(Position p1, Direction dir) {
        if (!is_valid_position(p1)) {
            throw std::out_of_range("Source position out of bounds.");
        }

        Position p2 = get_neighbor_pos(p1, dir);
        if (!is_valid_position(p2)) {
            throw std::invalid_argument("Cannot create passage leading out of bounds.");
        }

        grid[p1.y][p1.x].set_passage(dir, true);
        grid[p2.y][p2.x].set_passage(get_opposite_direction(dir), true);
    }

    bool has_passage(Position p1, Direction dir) const {
        if (!is_valid_position(p1)) return false;
        return get_cell(p1).can_move(dir);
    }
};

class MazeGenerator {
public:
    template <typename T>
    void generate_dfs(Maze<T>& maze, unsigned int seed) {
        int width = maze.get_width();
        int height = maze.get_height();

        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
        std::stack<Position> stack;

        Position start{ 0, 0 };
        visited[start.y][start.x] = true;
        stack.push(start);

        std::mt19937 rng(seed);
        Direction directions[] = { Direction::Up, Direction::Down, Direction::Left, Direction::Right };

        while (!stack.empty()) {
            Position current = stack.top();

            std::vector<std::pair<Position, Direction>> unvisited_neighbors;
            for (Direction dir : directions) {
                Position neighbor = maze.get_neighbor_pos(current, dir);
                if (maze.is_valid_position(neighbor) && !visited[neighbor.y][neighbor.x]) {
                    unvisited_neighbors.push_back({ neighbor, dir });
                }
            }

            if (!unvisited_neighbors.empty()) {
                std::uniform_int_distribution<size_t> dist(0, unvisited_neighbors.size() - 1);
                auto [next_pos, dir] = unvisited_neighbors[dist(rng)];

                maze.add_passage(current, dir);

                visited[next_pos.y][next_pos.x] = true;
                stack.push(next_pos);
            }
            else {
                stack.pop();
            }
        }
    }

    template <typename T>
    void generate_dfs(Maze<T>& maze) {
        std::random_device rd;
        generate_dfs(maze, rd());
    }
};

class MazeSolver {
public:
    template <typename T>
    std::vector<Position> find_path_bfs(const Maze<T>& maze, Position start, Position end) const {
        if (!maze.is_valid_position(start) || !maze.is_valid_position(end)) {
            return {};
        }

        int width = maze.get_width();
        int height = maze.get_height();

        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
        std::vector<std::vector<Position>> parent(height, std::vector<Position>(width, Position{ -1, -1 }));

        std::queue<Position> q;

        q.push(start);
        visited[start.y][start.x] = true;

        bool found = false;
        Direction directions[] = { Direction::Up, Direction::Down, Direction::Left, Direction::Right };

        while (!q.empty()) {
            Position current = q.front();
            q.pop();

            if (current == end) {
                found = true;
                break;
            }

            for (Direction dir : directions) {
                if (maze.has_passage(current, dir)) {
                    Position neighbor = maze.get_neighbor_pos(current, dir);
                    if (maze.is_valid_position(neighbor) && !visited[neighbor.y][neighbor.x]) {
                        visited[neighbor.y][neighbor.x] = true;
                        parent[neighbor.y][neighbor.x] = current;
                        q.push(neighbor);
                    }
                }
            }
        }

        std::vector<Position> path;
        if (!found) {
            return path;
        }

        Position curr = end;
        while (!(curr == Position{ -1, -1 })) {
            path.push_back(curr);
            curr = parent[curr.y][curr.x];
        }

        std::reverse(path.begin(), path.end());
        return path;
    }
};


template <typename T>
void print_maze(const Maze<T>& maze, const std::vector<Position>& path = {}) {
    int w = maze.get_width();
    int h = maze.get_height();

    auto is_in_path = [&](Position p) {
        for (const auto& pos : path) {
            if (pos == p) return true;
        }
        return false;
        };

    for (int x = 0; x < w; ++x) std::cout << "+---";
    std::cout << "+\n";

    for (int y = 0; y < h; ++y) {
        std::cout << "|";
        for (int x = 0; x < w; ++x) {
            Position current{ x, y };
            bool in_path = is_in_path(current);

            if (!path.empty() && current == path.front()) {
                std::cout << " S "; // Старт
            }
            else if (!path.empty() && current == path.back()) {
                std::cout << " E "; // Конец
            }
            else if (in_path) {
                std::cout << " * "; // Путь
            }
            else {
                std::cout << "   ";
            }

            if (x < w - 1) {
                if (maze.has_passage(current, Direction::Right)) {
                    std::cout << " ";
                }
                else {
                    std::cout << "|";
                }
            }
        }
        std::cout << "|\n";

        if (y < h - 1) {
            for (int x = 0; x < w; ++x) {
                Position current{ x, y };
                std::cout << "+";
                if (maze.has_passage(current, Direction::Down)) {
                    std::cout << "   ";
                }
                else {
                    std::cout << "---";
                }
            }
            std::cout << "+\n";
        }
    }

    for (int x = 0; x < w; ++x) std::cout << "+---";
    std::cout << "+\n";
}

int main() {
    try {
        std::cout << "=== Test 1: Dimension validation & Exceptions ===" << std::endl;
        try {
            Maze<int> invalid_maze(1, 5); // Должно вызвать исключение (< 2x2)
        }
        catch (const std::invalid_argument& e) {
            std::cout << "[PASS] Caught expected exception: " << e.what() << std::endl;
        }

        std::cout << "\n=== Test 2: Maze Generation (DFS) ===" << std::endl;
        int width = 7;
        int height = 5;

        Maze<int> my_maze(width, height, 0);

        MazeGenerator generator;
        generator.generate_dfs(my_maze, 42);

        std::cout << "Generated Maze (" << width << "x" << height << "):" << std::endl;
        print_maze(my_maze);

        std::cout << "\n=== Test 3: Pathfinding (BFS) ===" << std::endl;
        MazeSolver solver;
        Position start{ 0, 0 };
        Position end{ width - 1, height - 1 };

        std::vector<Position> path = solver.find_path_bfs(my_maze, start, end);

        if (!path.empty()) {
            std::cout << "[PASS] Path found successfully! Length: " << path.size() << " steps.\n" << std::endl;
            std::cout << "Maze with shortest path (S = Start, E = End, * = Path):" << std::endl;
            print_maze(my_maze, path);
        }
        else {
            std::cout << "[FAIL] Path not found!" << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Unhandled exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
