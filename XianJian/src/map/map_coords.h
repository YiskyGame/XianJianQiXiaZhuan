#ifndef MAP_MAP_COORDS_H
#define MAP_MAP_COORDS_H

#include "core/types.h"
#include <vector>
#include <string>
using namespace std;

// ============================================================
// 可走性 / 占据判定
// ============================================================
bool isWalkable(const vector<string>& data, int x, int y);
bool isOccupied(int x, int y);
bool isFreeAndWalkable(const vector<string>& data, int x, int y);

// ============================================================
// 查找合法坐标
// ============================================================
bool findRandomWalkable(const vector<string>& data,
                        int xMin, int xMax, int yMin, int yMax,
                        int& outX, int& outY,
                        bool avoidOccupied = true,
                        int maxTries = 200);

bool findFirstWalkable(const vector<string>& data, int& outX, int& outY);

bool findFarthestWalkable(const vector<string>& data,
                          int fromX, int fromY,
                          int& outX, int& outY,
                          bool avoidOccupied = true);

// ============================================================
// 校验并修正地图坐标
// ============================================================
void validateMapCoords();

#endif // MAP_MAP_COORDS_H