#ifndef __BOUNDINGBOX__
#define __BOUNDINGBOX__

#include <tucano/tucano.hpp>

/**
 * @brief Defines a 3D bounding box
 */
class BoundingBox
{

private:
	Eigen::Vector3f min;
	Eigen::Vector3f max;

public:
	BoundingBox(float x1 = 0,float y1 = 0,float z1 = 0, float x2 = 0, float y2 = 0, float z2 = 0)
	{
		min = Eigen::Vector3f(x1, y1, z1);
		max = Eigen::Vector3f(x2, y2, z2);
	}

	Eigen::Vector3f getMax() const
	{
		return max;
	}

	Eigen::Vector3f getMin() const
	{
		return min;
	}

	float getMinX() const
	{
		return min(0);
	}

	float getMinY() const
	{
		return min(1);
	}

	float getMinZ() const
	{
		return min(2);
	}

	float getMaxX() const
	{
		return max(0);
	}

	float getMaxY() const
	{
		return max(1);
	}

	float getMaxZ() const
	{
		return max(2);
	}
	
	void setMin(int i, float a)
	{
		min(i) = a;
	}

	void setMax(int i, float a)
	{
		max(i) = a;
	}

	void setMinX(float a)
	{
		min(0) = a;
	}

	void setMinY(float a)
	{
		min(1) = a;
	}

	void setMinZ(float a)
	{
		min(2) = a;
	}

	void setMaxX(float a)
	{
		max(0) = a;
	}

	void setMaxY(float a)
	{
		max(1) = a;
	}

	void setMaxZ(float a)
	{
		max(2) = a;
	}

	friend std::ostream &operator<<(std::ostream &os, const BoundingBox &bbox)
	{
		os << "X: [" << bbox.getMinX() << "," << bbox.getMaxX() << "]" << std::endl;
		os << "Y: [" << bbox.getMinY() << "," << bbox.getMaxY() << "]" << std::endl;
		os << "Z: [" << bbox.getMinZ() << "," << bbox.getMaxZ() << "]" << std::endl;
		return os;
	}
};

#endif
