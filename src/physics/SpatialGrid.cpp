#include "SpatialGrid.h"
#include <algorithm>
#include <cmath>

SpatialGrid::SpatialGrid(float cellSize, float worldWidth, float worldHeight)
    : cellSize(cellSize)
{
    gridWidth = static_cast<int>(std::ceil(worldWidth / cellSize));
    gridHeight = static_cast<int>(std::ceil(worldHeight / cellSize));
    cells.resize(gridWidth * gridHeight);
}

void SpatialGrid::clear() {
    for (auto& cell : cells) {
        cell.clear();
    }
}

void SpatialGrid::insertBall(size_t ballIndex, const Vector2D& position) {
    int cx = getCellX(position.x);
    int cy = getCellY(position.y);

    if (cx >= 0 && cx < gridWidth && cy >= 0 && cy < gridHeight) {
        cells[getCellIndex(cx, cy)].push_back(ballIndex);
    }
}

void SpatialGrid::getPotentialCollisions(
    const std::vector<Ball>&,
    std::vector<std::pair<size_t, size_t>>& outPairs)
{
    outPairs.clear();

    // Check each cell and its neighbors
    for (int cy = 0; cy < gridHeight; ++cy) {
        for (int cx = 0; cx < gridWidth; ++cx) {
            const auto& cell = cells[getCellIndex(cx, cy)];

            // Check within same cell
            for (size_t i = 0; i < cell.size(); ++i) {
                for (size_t j = i + 1; j < cell.size(); ++j) {
                    outPairs.emplace_back(cell[i], cell[j]);
                }
            }

            // Check with adjacent cells (right, down, down-right, down-left)
            const int dx[] = {1, 0, 1, -1};
            const int dy[] = {0, 1, 1, 1};

            for (int d = 0; d < 4; ++d) {
                int nx = cx + dx[d];
                int ny = cy + dy[d];

                if (nx >= 0 && nx < gridWidth && ny >= 0 && ny < gridHeight) {
                    const auto& neighborCell = cells[getCellIndex(nx, ny)];

                    for (size_t i : cell) {
                        for (size_t j : neighborCell) {
                            outPairs.emplace_back(i, j);
                        }
                    }
                }
            }
        }
    }
}

int SpatialGrid::getCellX(float x) const {
    return static_cast<int>(x / cellSize);
}

int SpatialGrid::getCellY(float y) const {
    return static_cast<int>(y / cellSize);
}

int SpatialGrid::getCellIndex(int cx, int cy) const {
    return cy * gridWidth + cx;
}
