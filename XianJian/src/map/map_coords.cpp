#include "map/map_coords.h"
#include "map/map.h"

// ============================================================
// 判断某格是否可走
// ============================================================
bool isWalkable(const vector<string>& data, int x, int y) {
    if (x < 0 || x >= (int)data.size()) return false;
    if (y < 0 || y >= (int)data[x].size()) return false;
    char c = data[x][y];
    if (c == '*' || c == '#' || c == '~' || c == 'X' || c == 'F') return false;
    return true;
}

// ============================================================
// 判断某格是否被 NPC 或存活怪物占据
// ============================================================
bool isOccupied(int x, int y) {
    for (auto& npc : currentMap.npcs) {
        if (npc.x == x && npc.y == y) return true;
    }
    for (auto& m : currentMap.mobs) {
        if (m.alive && m.x == x && m.y == y) return true;
    }
    return false;
}

// ============================================================
// 可走 + 未占据
// ============================================================
bool isFreeAndWalkable(const vector<string>& data, int x, int y) {
    return isWalkable(data, x, y) && !isOccupied(x, y);
}

// ============================================================
// 在指定区域随机找可走格
// ============================================================
bool findRandomWalkable(const vector<string>& data,
                        int xMin, int xMax, int yMin, int yMax,
                        int& outX, int& outY,
                        bool avoidOccupied,
                        int maxTries) {
    int h = (int)data.size();
    if (h == 0) return false;
    int w = (int)data[0].size();

    xMin = max(1, xMin);
    xMax = min(h - 2, xMax);
    yMin = max(1, yMin);
    yMax = min(w - 2, yMax);
    if (xMin > xMax || yMin > yMax) return false;

    for (int t = 0; t < maxTries; t++) {
        int x = xMin + rand() % (xMax - xMin + 1);
        int y = yMin + rand() % (yMax - yMin + 1);
        if (isWalkable(data, x, y)) {
            if (!avoidOccupied || !isOccupied(x, y)) {
                outX = x;
                outY = y;
                return true;
            }
        }
    }
    return false;
}

// ============================================================
// 从上到下、从左到右扫描，找第一个可走格
// ============================================================
bool findFirstWalkable(const vector<string>& data, int& outX, int& outY) {
    for (int x = 1; x < (int)data.size() - 1; x++) {
        for (int y = 1; y < (int)data[x].size() - 1; y++) {
            if (isWalkable(data, x, y)) {
                outX = x;
                outY = y;
                return true;
            }
        }
    }
    return false;
}

// ============================================================
// 找离 (fromX, fromY) 最远的可走格，用作出口
// ============================================================
bool findFarthestWalkable(const vector<string>& data,
                          int fromX, int fromY,
                          int& outX, int& outY,
                          bool avoidOccupied) {
    int bestDist = -1;
    int bestX = -1, bestY = -1;

    for (int x = 1; x < (int)data.size() - 1; x++) {
        for (int y = 1; y < (int)data[x].size() - 1; y++) {
            if (!isWalkable(data, x, y)) continue;
            if (avoidOccupied && isOccupied(x, y)) continue;
            if (x == fromX && y == fromY) continue;
            int d = abs(x - fromX) + abs(y - fromY);
            if (d > bestDist) {
                bestDist = d;
                bestX = x;
                bestY = y;
            }
        }
    }

    if (bestX == -1) return false;
    outX = bestX;
    outY = bestY;
    return true;
}

// ============================================================
// 校验并修正地图坐标
// ============================================================
void validateMapCoords() {
    int h = (int)currentMap.data.size();
    if (h == 0) return;
    int w = (int)currentMap.data[0].size();

    // --- 校验出生点 ---
    bool pxOk = isWalkable(currentMap.data, currentMap.px, currentMap.py);
    bool pxFree = !isOccupied(currentMap.px, currentMap.py);

    if (!pxOk || !pxFree) {
        int xMin = 1, xMax = max(1, h / 3);
        int yMin = 1, yMax = w - 2;
        int nx = -1, ny = -1;
        if (findRandomWalkable(currentMap.data, xMin, xMax, yMin, yMax, nx, ny)) {
            currentMap.px = nx;
            currentMap.py = ny;
        } else if (findFirstWalkable(currentMap.data, nx, ny)) {
            currentMap.px = nx;
            currentMap.py = ny;
        } else {
            currentMap.px = 1;
            currentMap.py = 1;
        }
    }

    // --- 校验出口 ---
    bool exOk = isWalkable(currentMap.data, currentMap.ex, currentMap.ey);
    bool exFree = !isOccupied(currentMap.ex, currentMap.ey);
    bool exSameAsPx = (currentMap.ex == currentMap.px
                       && currentMap.ey == currentMap.py);

    if (!exOk || !exFree || exSameAsPx) {
        int nx = -1, ny = -1;
        if (findFarthestWalkable(currentMap.data,
                                 currentMap.px, currentMap.py,
                                 nx, ny)) {
            currentMap.ex = nx;
            currentMap.ey = ny;
        } else if (findFirstWalkable(currentMap.data, nx, ny)) {
            currentMap.ex = nx;
            currentMap.ey = ny;
        } else {
            currentMap.ex = currentMap.px;
            currentMap.ey = currentMap.py + 1;
        }
    }

    // --- 最终保险 ---
    if (currentMap.ex == currentMap.px && currentMap.ey == currentMap.py) {
        for (int d = 0; d < 4; d++) {
            int nx = currentMap.px + dx[d];
            int ny = currentMap.py + dy[d];
            if (isWalkable(currentMap.data, nx, ny) && !isOccupied(nx, ny)) {
                currentMap.ex = nx;
                currentMap.ey = ny;
                break;
            }
        }
    }
}