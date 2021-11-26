#ifndef FRAMEWORK_INCLUDE
#define FRAMEWORK_INCLUDE

#include <new>

const int MaxNumOfShapes = 32768;
const int NumOfShapeTypes = 4;
const float MaxSearchRange = 0.125f;
const float WorldMaxX = 1.0f;
const float WorldMinX = -1.0f;
const float WorldMaxY = 1.0f;
const float WorldMinY = -1.0f;
const float MaxShapeSize = 0.01f;
const float MaxShapeSpeed = MaxShapeSize * 0.5f; // Maximum movement per frame
const float TargetBlend = 0.2f;

extern int AttractorType[4];

struct STriangle
{
	void SetColor(int i, unsigned char r, unsigned char g, unsigned char b)
	{
		m_Red[i] = r;
		m_Green[i] = g;
		m_Blue[i] = b;
	}

	void SetPosition(int i, float x, float y)
	{
		m_Px[i] = x;
		m_Py[i] = y;
	}

	unsigned char m_Red[3], m_Green[3], m_Blue[3];
	float m_Px[3], m_Py[3];
};

class IApplication
{
public:
	IApplication(void) : m_NumOfShapes(0) {}
	~IApplication(void) {}

	virtual int Update(float dt, STriangle* tri) = 0;

	virtual void DestroyShapes(int num = MaxNumOfShapes) = 0;
	virtual void SpawnTriangle(float x, float y, float size) = 0;
	virtual void SpawnRectangle(float x, float y, float size) = 0;
	virtual void SpawnHexagon(float x, float y, float radius) = 0;
	virtual void SpawnOctagon(float x, float y, float radius) = 0;
	virtual void Resize(float scale) = 0;

	int m_NumOfShapes;
};

inline int clampi(int v, int min, int max)
{
	return v < min ? min : (v > max ? max : v);
}

extern void* operator new(size_t size);
extern void operator delete(void* p);

extern IApplication* Factory(int index);

#endif
