#ifndef GRID_INCLUDE
#define GRID_INCLUDE

#include "Framework.h"
#include "GridCell.h"
#include "Shape.h"

class CGrid
{
public:
	static const int NumOfCells = 16;

	CGrid(void);
	~CGrid(void);

	CGridCell* GetCell(const int row, const int col);
	std::vector<CShape*>* GetShapes(const int row, const int col);
	std::vector<CShape*>* GetShapesOfType(const int row, const int col, int type);
	void Insert(CShape* shape);
	std::vector<CShape*> GetNearShapesOfType(int row, int col, int type);
	void Clear(void);

	static bool ValidPosition(int row, int col);

private:
	std::array<std::array<CGridCell, NumOfCells>, NumOfCells> m_Cells;
};

#endif
