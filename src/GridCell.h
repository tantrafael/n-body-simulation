#ifndef GRID_CELL_INCLUDE
#define GRID_CELL_INCLUDE

#include "Shape.h"
#include <vector>

class CGridCell
{
public:
	CGridCell(void);
	~CGridCell(void);

	void Insert(CShape* shape);
	std::vector<CShape*>* GetShapes();
	std::vector<CShape*>* GetShapesOfType(int type);
	void Clear(void);

private:
	std::vector<CShape*> m_Bucket;
	std::array<std::vector<CShape*>, NumOfShapeTypes> m_TypedBuckets;
};

#endif
