#ifndef APPLICATION_INCLUDE
#define APPLICATION_INCLUDE

#include "Framework.h"
#include "Shape.h"
#include "Grid.h"
#include <set>

class CApplication : public IApplication
{
public:
	CApplication(void);
	virtual ~CApplication(void);

	virtual int Update(float dt, STriangle* tri);

	virtual void DestroyShapes(int num = MaxNumOfShapes);
	virtual void SpawnTriangle(float x, float y, float size);
	virtual void SpawnRectangle(float x, float y, float size);
	virtual void SpawnHexagon(float x, float y, float radius);
	virtual void SpawnOctagon(float x, float y, float radius);
	virtual void Resize(float scale);

	void AddShape(CShape* shape);
	void DetectAttractions(void);
	void DetectAttractionsForGridCell(int row, int col);
	void DetectCollisions(void);
	void DetectCollisionsAmongShapes(std::vector<CShape*>& shapes);
	void DetectCollision(CShape* shape_0, CShape* shape_1);
	bool CheckCollisionPair(int id_0, int id_1);
	/*
	void BuildVerletNeighborLists(void);
	void BuildVerletNeighborList(int shapeIndex);
	*/

private:
	std::array<CShape*, MaxNumOfShapes> m_Shapes;
	CGrid m_Grid;
	std::set<std::pair<int, int>> m_CheckedCollisionPairs;
	/*
	// Verlet neighbor lists
	std::array<std::vector<CShape*>, MaxNumOfShapes> m_VerletNeighborLists;
	int m_NumOfVerletListRebuilsPerFrame;
	int m_ShapeIndex;
	*/
};

#endif
