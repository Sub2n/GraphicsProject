#include "pch.h"
#include "Vector.h"

float Vector::dotProduct(const Vector& v1, const Vector& v2) {
	return  v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector Vector::corssProduct(const Vector& v1, const Vector& v2) {
	Vector c;
	c.x = v1.y * v2.z - v1.z * v2.y;
	c.y = v1.x * v2.z - v1.z * v2.x;
	c.z = v1.x * v2.y - v1.y * v2.x;

	return c;
}

Vector operator+(const Vector& v1, const Vector& v2) 
{
	Vector v(v1.x + v2.x, v1.y + v2.y);
	return v;
}

Vector operator-(const Vector& v1, const Vector& v2)
{
	Vector v(v1.x - v2.x, v1.y - v2.y);
	return v;
}

Vector& Vector::operator=(const Vector& v1) {
	if (this == &v1) return *this;
	x = v1.x;
	y = v1.y;
	z = v1.z;
	return *this;
}