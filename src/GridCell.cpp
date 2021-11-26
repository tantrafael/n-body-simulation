#include "GridCell.h"

CGridCell::CGridCell(void)
{
}

CGridCell::~CGridCell(void)
{
	Clear();
}

void CGridCell::Insert(CShape* shape)
{
	m_Bucket.push_back(shape);
	m_TypedBuckets[shape->GetType()].push_back(shape);
}

std::vector<CShape*>* CGridCell::GetShapes()
{
	return &m_Bucket;
}

std::vector<CShape*>* CGridCell::GetShapesOfType(int type)
{
	return &m_TypedBuckets[type];
}

void CGridCell::Clear(void)
{
	m_Bucket.clear();
	m_Bucket.shrink_to_fit();

	for (int i = 0; i < NumOfShapeTypes; i++)
	{
		m_TypedBuckets[i].clear();
		m_TypedBuckets[i].shrink_to_fit();
	}
}
