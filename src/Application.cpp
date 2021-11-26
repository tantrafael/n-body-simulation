#include "Application.h"

IApplication* Factory(int index)
{
	switch (index)
	{
	case 1:
		return new CApplication();
	}

	return 0;
}

////////////////////////////////////////////
//
// CApplication
//
////////////////////////////////////////////

const float GridCellSizeInv = CGrid::NumOfCells / (WorldMaxX - WorldMinX);
const int GridCellMinIndex = 0;
const int GridCellMaxIndex = CGrid::NumOfCells - 1;
/*
const int NumOfFramesBetweenVerletListRebuilds = 20;
const float Skin = 2 * MaxShapeSpeed * NumOfFramesBetweenVerletListRebuilds;
const float VerletRadiusSquared = (MaxSearchRange + Skin) * (MaxSearchRange + Skin);
const float NumOfFramesBetweenVerletListRebuildsInv = 1.0f / NumOfFramesBetweenVerletListRebuilds;
*/

CApplication::CApplication(void)
//: m_ShapeIndex(0)
{
	for (int i = 0; i < MaxNumOfShapes; i++)
	{
		m_Shapes[i] = new CTriangle(0.0f, 0.0f, MaxShapeSize);
	}

	float angle;
	int numOfPoints;
	const float pi = 3.141592f;
	const float radius = 0.5f * MaxShapeSize;

	numOfPoints = CShape::HexagonPoints.size();
	angle = 0.0f;
	for (int i = 0; i < numOfPoints; i++, angle += 2.0f * pi / static_cast<float>(numOfPoints))
	{
		CShape::HexagonPoints[i] = CPoint2d(cosf(angle) * radius, -sinf(angle) * radius);
	}

	numOfPoints = CShape::OctagonPoints.size();
	angle = 0.0f;
	for (int i = 0; i < numOfPoints; i++, angle += 2.0f * pi / static_cast<float>(numOfPoints))
	{
		CShape::OctagonPoints[i] = CPoint2d(cosf(angle) * radius, -sinf(angle) * radius);
	}
}

CApplication::~CApplication(void)
{
	DestroyShapes();
}

void CApplication::DestroyShapes(int num)
{
}

void CApplication::SpawnTriangle(float x, float y, float size)
{
	AddShape(new CTriangle(x, y, size));
}

void CApplication::SpawnRectangle(float x, float y, float size)
{
	AddShape(new CRectangle(x, y, size));
}

void CApplication::SpawnHexagon(float x, float y, float radius)
{
	AddShape(new CHexagon(x, y, radius));
}

void CApplication::SpawnOctagon(float x, float y, float radius)
{
	AddShape(new COctagon(x, y, radius));
}

void CApplication::Resize(float scale)
{}

void CApplication::AddShape(CShape* shape)
{
	int index = m_NumOfShapes;
	delete m_Shapes[index];
	m_Shapes[index] = shape;
	shape->SetId(index);
}

int CApplication::Update(float dt, STriangle* tri)
{
	int tri_count = 0;
	CShape* shape;

	for (int i = 0; i < m_NumOfShapes; i++)
	{
		shape = m_Shapes[i];
		m_Grid.Insert(shape);
		//shape->FindTarget(m_VerletNeighborLists[i]);
		shape->Update(dt);
		tri_count += shape->Draw(&tri[tri_count]);
	}

	DetectCollisions();
	DetectAttractions();
	//BuildVerletNeighborLists();
	m_Grid.Clear();

	return tri_count;
}

void CApplication::DetectAttractions(void)
{
	for (int row = 0; row < CGrid::NumOfCells; row++)
	{
		for (int col = 0; col < CGrid::NumOfCells; col++)
		{
			DetectAttractionsForGridCell(row, col);
		}
	}
}

void CApplication::DetectAttractionsForGridCell(int row, int col)
{
	for (int type = 0; type < NumOfShapeTypes; type++)
	{
		int wantedType = AttractorType[type];
		auto shapesOfType = m_Grid.GetShapesOfType(row, col, type);
		auto nearAttractors = m_Grid.GetNearShapesOfType(row, col, wantedType);

		for (auto shapeOfType : *shapesOfType)
		{
			shapeOfType->FindTarget(nearAttractors);
		}
	}
}

void CApplication::DetectCollisions(void)
{
	for (int row = 0; row < CGrid::NumOfCells; row++)
	{
		for (int col = 0; col < CGrid::NumOfCells; col++)
		{
			auto shapes = m_Grid.GetShapes(row, col);
			DetectCollisionsAmongShapes(*shapes);
		}
	}

	m_CheckedCollisionPairs.clear();
}

void CApplication::DetectCollisionsAmongShapes(std::vector<CShape*>& shapes)
{
	int numOfShapes = shapes.size();

	for (int i = 0; i < numOfShapes; i++)
	{
		CShape* shape_i = shapes[i];

		if (numOfShapes > 1)
		{
			for (int j = i + 1; j < numOfShapes; j++)
			{
				CShape* shape_j = shapes[j];
				DetectCollision(shape_i, shape_j);
			}
		}
	}
}

void CApplication::DetectCollision(CShape* shape_0, CShape* shape_1)
{
	if (CShape::DetectCollisionBroad(shape_0, shape_1))
	{
		if (CShape::DetectCollisionNarrow(shape_0, shape_1))
		{
			if (CheckCollisionPair(shape_0->GetId(), shape_1->GetId()))
			{
				CShape::Collide(shape_0, shape_1);
			}
		}
	}
}

bool CApplication::CheckCollisionPair(int id_0, int id_1)
{
	if (id_0 > id_1)
	{
		std::swap(id_0, id_1);
	}

	auto collisionPair = std::make_pair(id_0, id_1);
	bool inserted = m_CheckedCollisionPairs.insert(collisionPair).second;

	return inserted;
}

/*
void CApplication::BuildVerletNeighborLists(void)
{
	m_NumOfVerletListRebuilsPerFrame = static_cast<int>(ceil(m_NumOfShapes * NumOfFramesBetweenVerletListRebuildsInv));

	for (int i = 0; i < m_NumOfVerletListRebuilsPerFrame; i++)
	{
		m_ShapeIndex = (m_ShapeIndex + 1) % m_NumOfShapes;
		BuildVerletNeighborList(m_ShapeIndex);
	}
}

void CApplication::BuildVerletNeighborList(int shapeIndex)
{
	auto verletNeighborList = &m_VerletNeighborLists[shapeIndex];
	verletNeighborList->clear();

	CShape* shape = m_Shapes[shapeIndex];
	int rowIndex = static_cast<int>((WorldMaxY - shape->GetY()) * GridCellSizeInv);
	int colIndex = static_cast<int>((-WorldMinX + shape->GetX()) * GridCellSizeInv);
	int row = std::min(std::max(GridCellMinIndex, rowIndex), GridCellMaxIndex);
	int col = std::min(std::max(GridCellMinIndex, colIndex), GridCellMaxIndex);

	int attractorType = AttractorType[shape->GetType()];
	std::vector<CShape*> nearbyAttractors = m_Grid.GetNearShapesOfType(row, col, attractorType);

	for (auto attractor : nearbyAttractors)
	{
		float delta_x = attractor->GetX() - shape->GetX();
		float delta_y = attractor->GetY() - shape->GetY();
		float distanceSquared = delta_x * delta_x + delta_y * delta_y;

		if (distanceSquared < VerletRadiusSquared)
		{
			verletNeighborList->push_back(attractor);
		}
	}

	shape->m_VerletNeighborList = verletNeighborList;
	verletNeighborList->shrink_to_fit();
}
*/
