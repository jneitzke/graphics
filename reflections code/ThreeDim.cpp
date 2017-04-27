//  three dimensional point and vector math

#include "stdafx.h"
#include <cstdio>
#include <cmath>
#include "ThreeDim.h"

double vector3:: length(void)
{
	return sqrt(dx * dx + dy * dy + dz * dz);
}

double vector3::  deltaX(void)    // return components
{
	return dx;
}

double vector3::  deltaY(void)
{
	return dy;
}

double vector3::  deltaZ(void)
{
	return dz;
}

void putVector(vector3 u)
{
	printf("< %f, %f, %f >", u.deltaX(), u.deltaY(), u.deltaZ());
}


vector3 operator+ (vector3 u, vector3 v)  //  overloaded +
{
	double x; double y; double z;
	vector3 temp;
	x = u.deltaX() + v.deltaX();
	y = u.deltaY() + v.deltaY();
	z = u.deltaZ() + v.deltaZ();
	temp = vector3(x,y,z);
	return temp;
}

Point3 operator+ (Point3 u, vector3 v)   //  overloaded +
{
	double x; double y; double z;
	Point3 temp;
	x = u.getX() + v.deltaX();
	y = u.getY() + v.deltaY();
	z = u.getZ() + v.deltaZ();
	temp = Point3(x,y,z);
	return temp;
}

vector3 operator- (vector3 u, vector3 v)  //  overloaded -
{
	double x; double y; double z;
	vector3 temp;
	x = u.deltaX() - v.deltaX();
	y = u.deltaY() - v.deltaY();
	z = u.deltaZ() - v.deltaZ();
	temp = vector3(x,y,z);
	return temp;
}

vector3 operator- (vector3 v)			  //  overloaded  unary -
{
	double x; double y; double z;
	vector3 temp;
	x =  - v.deltaX();
	y =  - v.deltaY();
	z =  - v.deltaZ();
	temp = vector3(x,y,z);
	return temp;
}

double inner(vector3 u , vector3 v)
{
	return u.deltaX() * v.deltaX() + u.deltaY() * v.deltaY() + u.deltaZ() * v.deltaZ();
}

double operator* (vector3 u, vector3 v)
{
	return inner(u,v);
}

vector3 operator* (double s, vector3 u)  // scalar product
{   
	vector3 temp (s * u.deltaX(), s* u.deltaY(), s* u.deltaZ());
	return temp;
}

vector3 cross (vector3 u, vector3 v)    // vector product -- 3D only
{
	vector3 temp;
	temp.set(u.deltaY() * v.deltaZ() - u.deltaZ() * v.deltaY(),
		u.deltaZ() * v.deltaX() - u.deltaX() * v.deltaZ(),
		u.deltaX() * v.deltaY() - u.deltaY() * v.deltaX());
	return temp;
}



vector3 normalize (vector3 u)  //  scale to unit length; if zero, stays zero
{

	double len ;
	len = u.length();
	//		printf("U.length:%f \n",len);

	if (len == 0.0)
	{
		vector3 temp(0.0, 0.0, 0.0);
		return temp;
	}
	else
	{
		len = u.length();
		//		printf("u.length:%f \n",len);

		//		vector3 temp(u.deltaX()/len, u.deltaY()/len);
		return (1.0 / len) * u;
	}
}

double cosAngleBetween(vector3 u, vector3 v)  //return cosine of angle between vectors
{
	return normalize(u) * normalize(v);
}

double angleBetween(vector3 u, vector3 v)  //return angle between vectors
{
	return acos ( cosAngleBetween(u, v));
}


//  vector3 perp(vector3 u )  // not defined for 3-vectors

vector3 project(vector3 u, vector3 v)  // projection of u onto v
{	// v must be nonzero
	return ((v * u)/(v * v))* v;
}

vector3 operator- (Point3 q, Point3 p)   // vector from p to q
{
	vector3 temp;
	temp.set(q.getX() - p.getX(), q.getY() - p.getY(), q.getZ() - p.getZ());
	return temp;
}

vector3 reflect(vector3 a, vector3 m)  // reflection of a in plane normal to m
{
	vector3 nm;
	nm = normalize (m);
	return a - (2*(a * nm))* nm;
}
