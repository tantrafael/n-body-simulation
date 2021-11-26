#include "Shape.h"
#include "Framework.h"

const float ShapeSizeHalf = 0.5f * MaxShapeSize;
const float MaxSearchRangeSquared = MaxSearchRange * MaxSearchRange;

std::array<CPoint2d, 6> CShape::HexagonPoints;
std::array<CPoint2d, 8> CShape::OctagonPoints;

float FastInvSqrt(float x)
{
	float xhalf = 0.5f * x;
	int i = *(int*)&x;
	i = 0x5f3759df - (i >> 1);
	x = *(float*)&i;  x = x * (1.5f - (xhalf*x*x));

	return x;
}

bool EdgeTest(float p0x, float p0y, float p1x, float p1y, float x, float y)
{
	float nx = -(p1y - p0y);
	float ny = p1x - p0x;
	float dot = nx * (x - p0x) + ny * (y - p0y);

	return dot < 0;
}

CShape::CShape(float x, float y)
: m_PosX(x)
, m_PosY(y)
, m_DirX(1.0f)
, m_DirY(0.1f)
, m_Id(0)
{}

CShape::~CShape(void)
{}

void CShape::Update(float dt)
{
	// Move
	m_PosX += dt * m_DirX;
	m_PosY += dt * m_DirY;

	// Wrap around window frame
	if (m_PosX > WorldMaxX)
	{
		m_PosX -= (WorldMaxX - WorldMinX);
	}
	else if (m_PosX < WorldMinX)
	{
		m_PosX += (WorldMaxX - WorldMinX);
	}

	if (m_PosY > WorldMaxY)
	{
		m_PosY -= (WorldMaxY - WorldMinY);
	}
	else if (m_PosY < WorldMinY)
	{
		m_PosY += (WorldMaxY - WorldMinY);
	}
}

SAABB CShape::GetAABB(void) const
{
	SAABB aabb;
	aabb.m_LowerBound = CPoint2d(m_PosX - ShapeSizeHalf, m_PosY - ShapeSizeHalf);
	aabb.m_UpperBound = CPoint2d(m_PosX + ShapeSizeHalf, m_PosY + ShapeSizeHalf);

	return aabb;
}

void CShape::FindTarget(std::vector<CShape*>& attractors)
{
	CShape* target = nullptr;
	float minDistanceSquared = MaxSearchRangeSquared;

	for (auto attractor : attractors)
	{
		CheckAttractor(attractor, minDistanceSquared, target);
	}

	if (target != nullptr)
	{
		Follow(target);
	}
}

void CShape::CheckAttractor(CShape* attractor, float& minDistanceSquared, CShape*& target)
{
	if (GetId() != attractor->GetId())
	{
		float delta_x = attractor->GetX() - GetX();
		float delta_y = attractor->GetY() - GetY();
		float distanceSquared = delta_x * delta_x + delta_y * delta_y;

		if (distanceSquared < minDistanceSquared)
		{
			minDistanceSquared = distanceSquared;
			target = attractor;
		}
	}
}

void CShape::Follow(CShape* target)
{
	float delta_x = target->GetX() - GetX();
	float delta_y = target->GetY() - GetY();
	float distanceInverted = FastInvSqrt(delta_x * delta_x + delta_y * delta_y);
	float dirTargetX = delta_x * distanceInverted;
	float dirTargetY = delta_y * distanceInverted;

	// Blend in direction towards target shape
	m_DirX = (1.0f - TargetBlend) * m_DirX + TargetBlend * dirTargetX;
	m_DirY = (1.0f - TargetBlend) * m_DirY + TargetBlend * dirTargetY;

	// Normalize direction
	float directionNormInv = FastInvSqrt(m_DirX * m_DirX + m_DirY * m_DirY);
	m_DirX *= directionNormInv;
	m_DirY *= directionNormInv;
}

bool CShape::DetectCollisionBroad(CShape* shape_0, CShape* shape_1)
{
	SAABB* A = &shape_0->GetAABB();
	SAABB* B = &shape_1->GetAABB();

	return (A->m_UpperBound.GetX() > B->m_LowerBound.GetX()
			&& A->m_LowerBound.GetX() < B->m_UpperBound.GetX()
			&& A->m_UpperBound.GetY() > B->m_LowerBound.GetY()
			&& A->m_LowerBound.GetY() < B->m_UpperBound.GetY());
}

bool CShape::DetectCollisionNarrow(CShape* shape_0, CShape* shape_1)
{
	return (shape_0->Test(shape_1) || shape_1->Test(shape_0));
}

void CShape::Collide(CShape* shape_0, CShape* shape_1)
{
	float delta_x = shape_1->GetX() - shape_0->GetX();
	float delta_y = shape_1->GetY() - shape_0->GetY();
	float deltaNormInv = FastInvSqrt(delta_x * delta_x + delta_y * delta_y);
	float deltaUnit_x = delta_x * deltaNormInv;
	float deltaUnit_y = delta_y * deltaNormInv;
	shape_0->m_DirX = -deltaUnit_x;
	shape_0->m_DirY = -deltaUnit_y;
	shape_1->m_DirX = deltaUnit_x;
	shape_1->m_DirY = deltaUnit_y;
}

//////////////////////////////////////////////////////////////////
//
//
// Triangle shape
//
//
//////////////////////////////////////////////////////////////////

CTriangle::CTriangle(float x, float y, float size)
: CShape(x, y)
{}

void CTriangle::Update(float dt)
{
	CShape::Update(dt);
}

int CTriangle::Draw(STriangle* tri)
{
	float halfSize = 0.5 * MaxShapeSize;

	tri->SetColor(0, 0, 255, 255);
	tri->SetPosition(0, m_PosX, m_PosY + halfSize);

	tri->SetColor(1, 0, 255, 255);
	tri->SetPosition(1, m_PosX - halfSize, m_PosY - halfSize);

	tri->SetColor(2, 0, 255, 255);
	tri->SetPosition(2, m_PosX + halfSize, m_PosY - halfSize);

	return 1;
}

bool CTriangle::Test(CShape* shape)
{
	return (shape->IsWithin(m_PosX, m_PosY + ShapeSizeHalf)
			|| shape->IsWithin(m_PosX - ShapeSizeHalf, m_PosY - ShapeSizeHalf)
			|| shape->IsWithin(m_PosX + ShapeSizeHalf, m_PosY - ShapeSizeHalf));
}

bool CTriangle::IsWithin(float x, float y)
{
	float p0x = m_PosX, p0y = m_PosY + ShapeSizeHalf;
	float p1x = m_PosX + ShapeSizeHalf, p2y = m_PosY - ShapeSizeHalf;
	float p2x = m_PosX - ShapeSizeHalf, p1y = m_PosY - ShapeSizeHalf;

	return (EdgeTest(p0x, p0y, p1x, p1y, x, y)
			&& EdgeTest(p1x, p1y, p2x, p2y, x, y)
			&& EdgeTest(p2x, p2y, p0x, p0y, x, y));
}

//////////////////////////////////////////////////////////////////
//
//
// Rectangle shape
//
//
//////////////////////////////////////////////////////////////////

CRectangle::CRectangle(float x, float y, float size)
: CShape(x, y)
{
}

void CRectangle::Update(float dt)
{
	CShape::Update(dt);
}

int CRectangle::Draw(STriangle* tri)
{
	float halfSize = 0.5 * MaxShapeSize;

	tri->SetColor(0, 255, 0, 0);
	tri->SetPosition(0, m_PosX - halfSize, m_PosY - halfSize);
	tri->SetColor(1, 255, 0, 0);
	tri->SetPosition(1, m_PosX + halfSize, m_PosY - halfSize);
	tri->SetColor(2, 255, 0, 0);
	tri->SetPosition(2, m_PosX + halfSize, m_PosY + halfSize);

	tri++;

	tri->SetColor(0, 255, 0, 0);
	tri->SetPosition(0, m_PosX - halfSize, m_PosY - halfSize);
	tri->SetColor(1, 255, 0, 0);
	tri->SetPosition(1, m_PosX + halfSize, m_PosY + halfSize);
	tri->SetColor(2, 255, 0, 0);
	tri->SetPosition(2, m_PosX - halfSize, m_PosY + halfSize);

	return 2;
}

bool CRectangle::Test(CShape* shape)
{
	return (shape->IsWithin(m_PosX - ShapeSizeHalf, m_PosY - ShapeSizeHalf)
		    || shape->IsWithin(m_PosX + ShapeSizeHalf, m_PosY - ShapeSizeHalf)
		    || shape->IsWithin(m_PosX - ShapeSizeHalf, m_PosY + ShapeSizeHalf)
		    || shape->IsWithin(m_PosX + ShapeSizeHalf, m_PosY + ShapeSizeHalf));
}

bool CRectangle::IsWithin(float x, float y)
{
	return (x >= m_PosX - ShapeSizeHalf
			&& x <= m_PosX + ShapeSizeHalf
			&& y >= m_PosY - ShapeSizeHalf
			&& y <= m_PosY + ShapeSizeHalf);
}

//////////////////////////////////////////////////////////////////
//
//
// Hexagon shape
//
//
//////////////////////////////////////////////////////////////////

CHexagon::CHexagon(float x, float y, float size)
: CShape(x, y)
{}

void CHexagon::Update(float dt)
{
	CShape::Update(dt);
}

int CHexagon::Draw(STriangle* tri)
{
	for (int i = 0; i < 6; i++)
	{
		tri->SetColor(0, 255, 0, 255);
		tri->SetPosition(0, m_PosX, m_PosY);
		tri->SetColor(1, 255, 0, 255);
		tri->SetPosition(1, m_PosX + HexagonPoints[i].GetX(), m_PosY + HexagonPoints[i].GetY());
		tri->SetColor(2, 255, 0, 255);
		tri->SetPosition(2, m_PosX + HexagonPoints[(i + 1) % 6].GetX(), m_PosY + HexagonPoints[(i + 1) % 6].GetY());
		tri++;
	}

	return 6;
}

bool CHexagon::Test(CShape* shape)
{
	for (int i = 0; i < 6; i++)
	{
		if (shape->IsWithin(m_PosX + HexagonPoints[i % 6].GetX(), m_PosY + HexagonPoints[i % 6].GetY()))
		{
			return true;
		}
	}

	return false;
}

bool CHexagon::IsWithin(float x, float y)
{
	int sum = 0;

	for (int i = 0; i < 6; i++)
	{
		sum += EdgeTest(m_PosX + HexagonPoints[i].GetX(),
						m_PosY + HexagonPoints[i].GetY(),
						m_PosX + HexagonPoints[(i + 1) % 6].GetX(),
						m_PosY + HexagonPoints[(i + 1) % 6].GetY(),
						x,
						y);
	}

	return (sum == 6);
}

//////////////////////////////////////////////////////////////////
//
//
// Octagon shape
//
//
//////////////////////////////////////////////////////////////////

COctagon::COctagon(float x, float y, float size)
: CShape(x, y)
{}

void COctagon::Update(float dt)
{
	CShape::Update(dt);
}

int COctagon::Draw(STriangle* tri)
{
	for (int i = 0; i < 8; i++)
	{
		tri->SetColor(0, 255, 255, 0);
		tri->SetPosition(0, m_PosX, m_PosY);
		tri->SetColor(1, 255, 255, 0);
		tri->SetPosition(1, m_PosX + OctagonPoints[i].GetX(), m_PosY + OctagonPoints[i].GetY());
		tri->SetColor(2, 255, 255, 0);
		tri->SetPosition(2, m_PosX + OctagonPoints[(i + 1) % 8].GetX(), m_PosY + OctagonPoints[(i + 1) % 8].GetY());
		tri++;
	}

	return 8;
}

bool COctagon::Test(CShape* shape)
{
	for (int i = 0; i < 8; i++)
	{
		if (shape->IsWithin(m_PosX + OctagonPoints[i % 8].GetX(), m_PosY + OctagonPoints[i % 8].GetY()))
		{
			return true;
		}
	}

	return false;
}

bool COctagon::IsWithin(float x, float y)
{
	int sum = 0;

	for (int i = 0; i < 8; i++)
	{
		sum += EdgeTest(m_PosX + OctagonPoints[i].GetX(),
						m_PosY + OctagonPoints[i].GetY(),
						m_PosX + OctagonPoints[(i + 1) % 8].GetX(),
						m_PosY + OctagonPoints[(i + 1) % 8].GetY(),
						x,
						y);
	}

	return (sum == 8);
}
