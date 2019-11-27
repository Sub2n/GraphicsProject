#include "pch.h"
#include "Vector.h"

float Vector::dotProduct(const Vector& v) {
	return x * v.x + y * v.y + z * v.z;
}

Vector Vector::crossProduct(const Vector& v) {
	Vector c;

	c.x = y * v.z - z * v.y;
	c.y = x * v.z - z * v.x;
	c.z = x * v.y - y * v.x;

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


Vector operator*(const Vector& v1, float f)
{
	Vector v(v1.x * f, v1.y * f, v1.z * f);
	return v;
}

Vector& Vector::operator=(const Vector& v1) {
	if (this == &v1) return *this;
	x = v1.x;
	y = v1.y;
	z = v1.z;
	return *this;
}

Vector& Vector::operator+=(const Vector& v1) {
	x += v1.x;
	y += v1.y;
	z += v1.z;
	return *this;
}

Vector& Vector::operator-=(const Vector& v1) {
	x -= v1.x;
	y -= v1.y;
	z -= v1.z;
	return *this;
}

Vector Vector::normalize() {
	int s = sqrt(x * x + y * y + z * z);
	Vector n;
	n.x = x / s;
	n.y = y / s;
	n.z = z / s;
	return n;
}