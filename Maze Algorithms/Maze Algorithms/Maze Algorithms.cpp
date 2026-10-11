// ToanMaze_CompareV13.cpp - Fixed for C++14, cleaned and functional V13
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <functional>

using namespace std;

const int SIZE = 100;
const int WALL = 1;
const int PATH = 0;
const vector<pair<int, int>> DIRS = { {-1,0},{1,0},{0,-1},{0,1} };

struct Node {
    int x, y, g, h;
    Node* parent;
    Node(int x, int y, int g, int h, Node* p = nullptr) : x(x), y(y), g(g), h(h), parent(p) {}
    int f() const { return g + h; }
};

struct Compare { bool operator()(Node* a, Node* b) { return a->f() > b->f(); } };

bool inBounds(int x, int y) {
    return x >= 0 && y >= 0 && x < SIZE && y < SIZE;
}

int heuristic(pair<int, int> a, pair<int, int> b) {
    return abs(a.first - b.first) + abs(a.second - b.second);
}

vector<pair<int, int>> reconstruct(Node* end) {
    vector<pair<int, int>> path;
    while (end) {
        path.push_back({ end->x, end->y });
        end = end->parent;
    }
    reverse(path.begin(), path.end());
    return path;
}

vector<pair<int, int>> astar_classic(const vector<vector<int>>& maze, pair<int, int> start, pair<int, int> end) {
    priority_queue<Node*, vector<Node*>, Compare> open;
    vector<vector<int>> g_cost(SIZE, vector<int>(SIZE, INT_MAX));
    vector<vector<bool>> visited(SIZE, vector<bool>(SIZE, false));
    open.push(new Node(start.first, start.second, 0, heuristic(start, end)));
    g_cost[start.first][start.second] = 0;

    while (!open.empty()) {
        Node* cur = open.top(); open.pop();
        if (visited[cur->x][cur->y]) continue;
        visited[cur->x][cur->y] = true;

        if (make_pair(cur->x, cur->y) == end)
            return reconstruct(cur);

        for (auto d : DIRS) {
            int nx = cur->x + d.first;
            int ny = cur->y + d.second;
            int new_g = cur->g + 1;

            if (inBounds(nx, ny) && maze[nx][ny] == PATH && new_g < g_cost[nx][ny]) {
                g_cost[nx][ny] = new_g;
                open.push(new Node(nx, ny, new_g, heuristic({ nx, ny }, end), cur));
            }
        }
    }
    return {};
}

vector<pair<int, int>> AFastV1_DualSearch(const vector<vector<int>>& maze, pair<int, int> start, pair<int, int> end) {
    struct NodeX {
        int x, y, g, h;
        NodeX* parent;
        NodeX(int x, int y, int g, int h, NodeX* p = nullptr) : x(x), y(y), g(g), h(h), parent(p) {}
        int f() const { return g + h; }
    };

    auto reconstruct = [](NodeX* n1, NodeX* n2) {
        vector<pair<int, int>> p1, p2;
        while (n1) { p1.push_back({ n1->x, n1->y }); n1 = n1->parent; }
        while (n2) { p2.push_back({ n2->x, n2->y }); n2 = n2->parent; }
        reverse(p1.begin(), p1.end());
        p2.erase(p2.begin());
        p1.insert(p1.end(), p2.begin(), p2.end());
        return p1;
        };

    auto cmp = [](NodeX* a, NodeX* b) { return a->f() > b->f(); };
    priority_queue<NodeX*, vector<NodeX*>, decltype(cmp)> open1(cmp), open2(cmp);
    vector<vector<int>> g1(SIZE, vector<int>(SIZE, INT_MAX)), g2(SIZE, vector<int>(SIZE, INT_MAX));
    vector<vector<NodeX*>> seen1(SIZE, vector<NodeX*>(SIZE, nullptr)), seen2(SIZE, vector<NodeX*>(SIZE, nullptr));

    NodeX* s = new NodeX(start.first, start.second, 0, heuristic(start, end));
    NodeX* e = new NodeX(end.first, end.second, 0, heuristic(end, start));
    open1.push(s); open2.push(e);
    g1[s->x][s->y] = 0; g2[e->x][e->y] = 0;

    while (!open1.empty() && !open2.empty()) {
        auto expand = [&](priority_queue<NodeX*, vector<NodeX*>, decltype(cmp)>& open,
            vector<vector<int>>& g,
            vector<vector<NodeX*>>& seen,
            vector<vector<NodeX*>>& other_seen,
            pair<int, int> goal,
            bool& meet_found,
            vector<pair<int, int>>& result) {
                if (open.empty()) return;
                NodeX* cur = open.top(); open.pop();
                if (seen[cur->x][cur->y]) return;
                seen[cur->x][cur->y] = cur;

                if (other_seen[cur->x][cur->y]) {
                    meet_found = true;
                    result = reconstruct(cur, other_seen[cur->x][cur->y]);
                    return;
                }

                for (auto d : DIRS) {
                    int nx = cur->x + d.first, ny = cur->y + d.second;
                    if (!inBounds(nx, ny) || maze[nx][ny] == WALL) continue;
                    int ng = cur->g + 1;
                    if (ng < g[nx][ny]) {
                        g[nx][ny] = ng;
                        open.push(new NodeX(nx, ny, ng, heuristic({ nx, ny }, goal), cur));
                    }
                }
            };

        bool meet = false;
        vector<pair<int, int>> result;
        expand(open1, g1, seen1, seen2, end, meet, result);
        if (meet) return result;
        expand(open2, g2, seen2, seen1, start, meet, result);
        if (meet) return result;
    }

    return {};
}

// --- V13 FIXED ---

struct BidirNode {
    int x, y, g;
    BidirNode* parent;
    BidirNode(int x, int y, int g, BidirNode* p = nullptr) : x(x), y(y), g(g), parent(p) {}
};

vector<pair<int, int>> reconstruct_bidir(BidirNode* n1, BidirNode* n2) {
    vector<pair<int, int>> path1, path2;
    while (n1) {
        path1.push_back({ n1->x, n1->y });
        n1 = n1->parent;
    }
    while (n2) {
        path2.push_back({ n2->x, n2->y });
        n2 = n2->parent;
    }
    reverse(path1.begin(), path1.end());
    path2.erase(path2.begin()); // remove duplicate
    path1.insert(path1.end(), path2.begin(), path2.end());
    return path1;
}

vector<pair<int, int>> v13_pro(const vector<vector<int>>& maze, pair<int, int> start, pair<int, int> end) {
    // Chiến lược chia vùng: chia thành 3 vùng chính giữa start và end theo trục chéo
    // Tạm thời mình chỉ ưu tiên hóa theo vùng gần start & end hơn (có thể mở rộng nhiều vùng hơn)

    struct FastNode {
        int x, y, g, h;
        FastNode* parent;
        FastNode(int x, int y, int g, int h, FastNode* p = nullptr) : x(x), y(y), g(g), h(h), parent(p) {}
        int f() const { return g + h; }
    };

    auto reconstruct_fast = [](FastNode* n1, FastNode* n2) {
        vector<pair<int, int>> path1, path2;
        while (n1) { path1.push_back({ n1->x, n1->y }); n1 = n1->parent; }
        while (n2) { path2.push_back({ n2->x, n2->y }); n2 = n2->parent; }
        reverse(path1.begin(), path1.end());
        path2.erase(path2.begin());
        path1.insert(path1.end(), path2.begin(), path2.end());
        return path1;
    };

    priority_queue<FastNode*, vector<FastNode*>, function<bool(FastNode*, FastNode*)>> open_start(
        [](FastNode* a, FastNode* b) { return a->f() > b->f(); });
    priority_queue<FastNode*, vector<FastNode*>, function<bool(FastNode*, FastNode*)>> open_end(
        [](FastNode* a, FastNode* b) { return a->f() > b->f(); });

    vector<vector<int>> g_start(SIZE, vector<int>(SIZE, INT_MAX));
    vector<vector<int>> g_end(SIZE, vector<int>(SIZE, INT_MAX));
    vector<vector<FastNode*>> visited_start(SIZE, vector<FastNode*>(SIZE, nullptr));
    vector<vector<FastNode*>> visited_end(SIZE, vector<FastNode*>(SIZE, nullptr));

    FastNode* s = new FastNode(start.first, start.second, 0, heuristic(start, end));
    FastNode* e = new FastNode(end.first, end.second, 0, heuristic(end, start));
    open_start.push(s);
    open_end.push(e);
    g_start[start.first][start.second] = 0;
    g_end[end.first][end.second] = 0;

    while (!open_start.empty() && !open_end.empty()) {
        auto cur_s = open_start.top(); open_start.pop();
        if (visited_start[cur_s->x][cur_s->y]) continue;
        visited_start[cur_s->x][cur_s->y] = cur_s;

        if (visited_end[cur_s->x][cur_s->y]) {
            return reconstruct_fast(cur_s, visited_end[cur_s->x][cur_s->y]);
        }

        for (int i = 0; i < 4; ++i) {
            int nx = cur_s->x + DIRS[i].first;
            int ny = cur_s->y + DIRS[i].second;
            if (!inBounds(nx, ny) || maze[nx][ny] == WALL) continue;
            int ng = cur_s->g + 1;
            if (ng < g_start[nx][ny]) {
                g_start[nx][ny] = ng;
                open_start.push(new FastNode(nx, ny, ng, heuristic({ nx, ny }, end), cur_s));
            }
        }

        auto cur_e = open_end.top(); open_end.pop();
        if (visited_end[cur_e->x][cur_e->y]) continue;
        visited_end[cur_e->x][cur_e->y] = cur_e;

        if (visited_start[cur_e->x][cur_e->y]) {
            return reconstruct_fast(visited_start[cur_e->x][cur_e->y], cur_e);
        }

        for (int i = 0; i < 4; ++i) {
            int nx = cur_e->x + DIRS[i].first;
            int ny = cur_e->y + DIRS[i].second;
            if (!inBounds(nx, ny) || maze[nx][ny] == WALL) continue;
            int ng = cur_e->g + 1;
            if (ng < g_end[nx][ny]) {
                g_end[nx][ny] = ng;
                open_end.push(new FastNode(nx, ny, ng, heuristic({ nx, ny }, start), cur_e));
            }
        }
    }

    return {}; // fail
}


vector<pair<int, int>> simplifyPath(const vector<pair<int, int>>& path) {
    if (path.size() < 3) return path;
    vector<pair<int, int>> result = { path[0] };
    int dx = path[1].first - path[0].first;
    int dy = path[1].second - path[0].second;
    for (size_t i = 2; i < path.size(); ++i) {
        int ndx = path[i].first - path[i - 1].first;
        int ndy = path[i].second - path[i - 1].second;
        if (ndx != dx || ndy != dy) {
            result.push_back(path[i - 1]);
            dx = ndx; dy = ndy;
        }
    }
    result.push_back(path.back());
    return result;
}

vector<vector<int>> generateMaze() {
    vector<vector<int>> maze(SIZE, vector<int>(SIZE));
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            maze[i][j] = (rand() % 100 < 30) ? WALL : PATH;
    maze[0][0] = PATH;
    maze[SIZE - 1][SIZE - 1] = PATH;
    return maze;
}

bool isBlocked(const vector<vector<int>>& maze, pair<int, int> p) {
    for (auto d : DIRS) {
        int nx = p.first + d.first, ny = p.second + d.second;
        if (inBounds(nx, ny) && maze[nx][ny] == PATH) return false;
    }
    return true;
}

int main() {
    srand((unsigned)time(0));
    const int TESTS = 30;

    for (int i = 1; i <= TESTS; ++i) {
        cout << "\n\U0001F9EA Test " << i << "/" << TESTS << ":\n";
        auto maze = generateMaze();
        pair<int, int> start = { 0, 0 }, end = { SIZE - 1, SIZE - 1 };

        if (isBlocked(maze, end)) {
            cout << "- \u274C End bi chan. Skip test!\n";
            continue;
        }

        using namespace chrono;

        auto t1s = high_resolution_clock::now();
        auto path1 = astar_classic(maze, start, end);
        auto t1e = high_resolution_clock::now();

        auto t2s = high_resolution_clock::now();
        auto path2 = AFastV1_DualSearch(maze, start, end);
        auto t2e = high_resolution_clock::now();

        auto t3s = high_resolution_clock::now();
        auto path3 = v13_pro(maze, start, end);
        auto t3e = high_resolution_clock::now();

        path1 = simplifyPath(path1);
        path2 = simplifyPath(path2);
        path3 = simplifyPath(path3);

        cout << "- A* Classic Time: " << duration<double>(t1e - t1s).count() << "s, Found: " << !path1.empty() << ", Len: " << path1.size() << "\n";
        cout << "- AFastV1_DualSearch Time: " << duration<double>(t2e - t2s).count() << "s, Found: " << !path2.empty() << ", Len: " << path2.size() << "\n";
        cout << "- V13 Pro Time:  " << duration<double>(t3e - t3s).count() << "s, Found: " << !path3.empty() << ", Len: " << path3.size() << "\n";
    }
}
