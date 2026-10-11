// Filename: ToanMaze.cpp
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <future>
#include <unordered_set>

using namespace std;

const int SIZE = 100;
const int WALL = 1;
const int PATH = 0;
const vector<pair<int, int>> DIRS = { {-1, 0}, {1, 0}, {0, 1}, {0, -1} };

struct Node {
    int x, y;
    int g, h;
    Node* parent;
    Node(int x, int y, int g, int h, Node* p = nullptr) : x(x), y(y), g(g), h(h), parent(p) {}
    int f() const { return g + h; }
};

struct Compare {
    bool operator()(Node* a, Node* b) { return a->f() > b->f(); }
};

int heuristic(pair<int, int> a, pair<int, int> b) {
    return abs(a.first - b.first) + abs(a.second - b.second);
}

vector<vector<int>> generateMaze(int size) {
    vector<vector<int>> maze(size, vector<int>(size));
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            maze[i][j] = (rand() % 100 < 30) ? WALL : PATH; // 30% wall
    maze[0][0] = PATH;
    maze[size - 1][size - 1] = PATH;
    return maze;
}

bool inBounds(int x, int y, int size) {
    return x >= 0 && y >= 0 && x < size && y < size;
}

vector<pair<int, int>> reconstructPath(Node* end) {
    vector<pair<int, int>> path;
    while (end) {
        path.push_back({ end->x, end->y });
        end = end->parent;
    }
    reverse(path.begin(), path.end());
    return path;
}

vector<pair<int, int>> astar(const vector<vector<int>>& maze, pair<int, int> start, pair<int, int> end, unordered_set<int>* blacklist = nullptr) {
    int size = maze.size();
    vector<vector<bool>> visited(size, vector<bool>(size, false));
    priority_queue<Node*, vector<Node*>, Compare> openSet;

    openSet.push(new Node(start.first, start.second, 0, heuristic(start, end)));

    while (!openSet.empty()) {
        Node* current = openSet.top(); openSet.pop();
        if (visited[current->x][current->y]) continue;
        visited[current->x][current->y] = true;

        if (make_pair(current->x, current->y) == end)
            return reconstructPath(current);

        for (auto d : DIRS) {
            int nx = current->x + d.first;
            int ny = current->y + d.second;
            int flatIdx = nx * size + ny;
            if (inBounds(nx, ny, size) && maze[nx][ny] == PATH && !visited[nx][ny]) {
                if (blacklist && blacklist->count(flatIdx)) continue;
                openSet.push(new Node(nx, ny, current->g + 1, heuristic({ nx, ny }, end), current));
            }
        }
    }
    return {}; // No path
}

int main() {
    srand(time(nullptr));
    auto maze = generateMaze(SIZE);
    pair<int, int> start = { 0, 0 }, end = { SIZE - 1, SIZE - 1 };

    // Async scout
    cout << "🧭 ToanMaze SCOUT: Calculating path (async)...\n";
    auto scout = async(launch::async, [&]() {
        return astar(maze, start, end); // scout có thể dùng metaAstarSmart nếu muốn
        });

    // Runner đợi scout
    auto path = scout.get();
    if (path.empty()) {
        cout << "❌ No path found by scout. Runner cannot proceed.\n";
    }
    else {
        cout << "✅ Path confirmed! 🏃 Runner is now following the route...\n";
        for (auto step : path) {
            cout << "→ Moving to (" << step.first << ", " << step.second << ")\n";
        }
        cout << "\n🏁 Runner reached the destination! Total steps: " << path.size() << "\n";
    }
    return 0;
}
