#ifndef POINT2D_INCLUDE
#define POINT2D_INCLUDE

class CPoint2d
{
public:
	CPoint2d() : m_X(0), m_Y(0) {}
	CPoint2d(float x, float y) : m_X(x), m_Y(y) {}

	float GetX() const { return m_X; }
	float GetY() const { return m_Y; }

private:
	float m_X, m_Y;
};

#endif
