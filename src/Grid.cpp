#include "Grid.h"
#include <algorithm>
#include <array>

const float MaxSearchRangeSquared = MaxSearchRange * MaxSearchRange;
const float GridCellSizeInv = CGrid::NumOfCells / (WorldMaxX - WorldMinX);
const int GridCellMinIndex = 0;
const int GridCellMaxIndex = CGrid::NumOfCells - 1;

const std::array<std::pair<int, int>, 9> NearCellIndexOffsets =
{
	std::make_pair(-1, -1),
	std::make_pair(-1,  0),
	std::make_pair(-1,  1),
	std::make_pair( 0, -1),
	std::make_pair( 0,  0),
	std::make_pair( 0,  1),
	std::make_pair( 1, -1),
	std::make_pair( 1,  0),
	std::make_pair( 1,  1)
};

CGrid::CGrid(void)
{
}

CGrid::~CGrid(void)
{
	Clear();
}

CGridCell* CGrid::GetCell(const int row, const int col)
{
	CGridCell* cell = nullptr;

	if (ValidPosition(row, col))
	{
		cell = &m_Cells[row][col];
	}

	return cell;
}

void CGrid::Insert(CShape* shape)
{
	SAABB* aabb = &shape->GetAABB();
	CPoint2d lowerBound = aabb->m_LowerBound;
	CPoint2d upperBound = aabb->m_UpperBound;
	int lowerRow = static_cast<int>((WorldMaxY - lowerBound.GetY()) * GridCellSizeInv);
	int lowerCol = static_cast<int>((-WorldMinX + lowerBound.GetX()) * GridCellSizeInv);
	int upperRow = static_cast<int>((WorldMaxY - upperBound.GetY()) * GridCellSizeInv);
	int upperCol = static_cast<int>((-WorldMinX + upperBound.GetX()) * GridCellSizeInv);
	int startRow = std::max(GridCellMinIndex, upperRow);
	int startCol = std::max(GridCellMinIndex, lowerCol);
	int endRow = std::min(GridCellMaxIndex, lowerRow);
	int endCol = std::min(GridCellMaxIndex, upperCol);

	for (int row = startRow; row <= endRow; row++)
	{
		for (int col = startCol; col <= endCol; col++)
		{
			GetCell(row, col)->Insert(shape);
		}
	}
}

std::vector<CShape*>* CGrid::GetShapes(const int row, const int col)
{
	std::vector<CShape*>* shapes;

	if (ValidPosition(row, col))
	{
		CGridCell* cell = &m_Cells[row][col];
		shapes = cell->GetShapes();
	}

	return shapes;
}

std::vector<CShape*>* CGrid::GetShapesOfType(const int row, const int col, int type)
{
	std::vector<CShape*>* shapesOfType;

	if (ValidPosition(row, col))
	{
		CGridCell* cell = &m_Cells[row][col];
		shapesOfType = cell->GetShapesOfType(type);
	}

	return shapesOfType;
}

std::vector<CShape*> CGrid::GetNearShapesOfType(int row, int col, int type)
{
	std::vector<std::pair<int, int>> nearGridPositions;

	for (auto offset : NearCellIndexOffsets)
	{
		int r = row + offset.first;
		int c = col + offset.second;

		if (ValidPosition(r, c))
		{
			nearGridPositions.push_back(std::make_pair(r, c));
		}
	}

	std::vector<CShape*> nearShapesOfType;

	for (auto gridPosition : nearGridPositions)
	{
		auto shapes = GetShapesOfType(gridPosition.first, gridPosition.second, type);
		nearShapesOfType.insert(nearShapesOfType.end(), shapes->begin(), shapes->end());
	}

	return nearShapesOfType;
}

bool CGrid::ValidPosition(int row, int col)
{
	return ((row >= 0) && (row < NumOfCells) && (col >= 0) && (col < NumOfCells));
}

void CGrid::Clear(void)
{
	for (int row = 0; row < NumOfCells; row++)
	{
		for (int col = 0; col < NumOfCells; col++)
		{
			m_Cells[row][col].Clear();
		}
	}
}
