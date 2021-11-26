#ifndef SHAPE_INCLUDE
#define SHAPE_INCLUDE

#include "Framework.h"
#include "Point2d.h"
#include <array>
#include <vector>

struct SAABB
{
	CPoint2d m_LowerBound;
	CPoint2d m_UpperBound;
};

class CShape
{
public:
	static std::array<CPoint2d, 6> CShape::HexagonPoints;
	static std::array<CPoint2d, 8> CShape::OctagonPoints;

	CShape(float x, float y);
	virtual ~CShape(void);

	virtual void Update(float dt);
	virtual int Draw(STriangle* tri) = 0;
	virtual bool Test(CShape* shape) = 0;
	virtual bool IsWithin(float x, float y) = 0;
	virtual int GetType() const = 0;

	float GetX() const { return m_PosX; }
	float GetY() const { return m_PosY; }
	SAABB GetAABB(void) const;
	int GetId() const { return m_Id; }
	void SetId(int id) { m_Id = id; }
	void FindTarget(std::vector<CShape*>& attractors);
	void CheckAttractor(CShape* shape, float& minDistanceSquared, CShape*& target);
	void Follow(CShape* shape);

	static bool DetectCollisionBroad(CShape* shape_0, CShape* shape_1);
	static bool DetectCollisionNarrow(CShape* shape_0, CShape* shape_1);
	static void Collide(CShape* shape_0, CShape* shape_1);

protected:
	float m_PosX, m_PosY;
	float m_DirX, m_DirY;
	int m_Id;
};

class CTriangle : public CShape
{
public:
	CTriangle(float x, float y, float size);
	virtual ~CTriangle(void) {}

	virtual void Update(float dt);
	virtual int Draw(STriangle* tri);
	virtual bool Test(CShape* shape);
	virtual bool IsWithin(float x, float y);
	virtual int GetType() const { return 0; }
};

class CRectangle : public CShape
{
public:
	CRectangle(float x, float y, float size);
	virtual ~CRectangle(void) {}

	virtual void Update(float dt);
	virtual int Draw(STriangle* tri);
	virtual bool Test(CShape* shape);
	virtual bool IsWithin(float x, float y);
	virtual int GetType() const { return 1; }
};

class CHexagon : public CShape
{
public:
	CHexagon(float x, float y, float radius);
	virtual ~CHexagon(void) {}

	virtual void Update(float dt);
	virtual int Draw(STriangle* tri);
	virtual bool Test(CShape* shape);
	virtual bool IsWithin(float x, float y);
	virtual int GetType() const { return 2; }
};

class COctagon : public CShape
{
public:
	COctagon(float x, float y, float radius);
	virtual ~COctagon(void) {}

	virtual void Update(float dt);
	virtual int Draw(STriangle* tri);
	virtual bool Test(CShape* shape);
	virtual bool IsWithin(float x, float y);
	virtual int GetType() const { return 3; }
};

#endif
