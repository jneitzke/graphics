
#include "stdafx.h"
#include <windows.h>


#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <c:/GL/glut.h>
#include <GL/glu.h>
#include <gl/gl.h>
#endif


#include <math.h>
#include "rayCam.h" // modified camera for raytracing

#define _USE_MATH_DEFINES // for C++
#include <cmath>

//  clamp values to 0 <= d <= 1
const double epsilon = 0.000015; //threshold for 'near zero'

double unitIntervalBounded(double d)
{
	if (d < 0)
	{
		return 0.0;
	}
	else if (d > 1)
	{
		return 1.0;
	}
	return d;
}

// modified from Hill 
// a Cam class for flying through 3D scenes.

int(*textureArray[maxTexture])(Point3 p);

int  solid(Point3 p){
	return 0;
}
////  checkerboard texture

//int steps = 7;

boolean even(int x)
{
	return x / 2 * 2 == x;
}

int checkerboard3(Point3 p, int steps){
	int a = (int)(abs(p.getX())* steps);
	int b = (int)(abs(p.getY())* steps);
	int c = (int)(abs(p.getZ())* steps);
	if (p.getX() < 0){ a += 1; }
	if (p.getY() < 0){ b += 1; }
	if (p.getZ() < 0){ b += 1; }


	return (a + b + c) % 2;
}

double rounding(double x){
	double y = (double)floor(x - .5);
	if ((double)floor(x) == y) return y + 1;
	else
		return y;
}

int grid(Point3 p, int steps){
	double width = .5;
	int spacing = 2 * steps;
	if (((rounding(p.getX()) * spacing - (double)floor(p.getX()* spacing)) < width) ||
		((rounding(p.getY()) * spacing - (double)floor(p.getY()* spacing)) < width) ||
		((rounding(p.getZ()) * spacing - (double)floor(p.getZ()* spacing)) < width))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

double frem(double x){  //  double equivalent of mod
	return x - (double)floor(x);
}

int diag(Point3 p, int steps){
	double width = .15;
	int spacing = 2 * steps;
	if (frem(((p.getX() * steps) +
		(p.getY() * steps) +
		(p.getZ() * steps))) < width) return 0;
	if (frem(((p.getX() * steps) +
		(p.getY() * steps) -
		(p.getZ() * steps))) < width) return 0;
	if (frem(((p.getX() * steps) -
		(p.getY() * steps) +
		(p.getZ() * steps))) < width) return 0;
	if (frem(((p.getX() * steps) -
		(p.getY() * steps) -
		(p.getZ() * steps))) < width) return 0;
	if (frem(((-p.getX() * steps) +
		(p.getY() * steps) +
		(p.getZ() * steps))) < width) return 0;
	if (frem(((-p.getX() * steps) +
		(p.getY() * steps) -
		(p.getZ() * steps))) < width) return 0;
	if (frem(((-p.getX() * steps) -
		(p.getY() * steps) +
		(p.getZ() * steps))) < width) return 0;
	if (frem(((-p.getX() * steps) -
		(p.getY() * steps) -
		(p.getZ() * steps))) < width) return 0;
	return 1;

}

double euclid2d(double p1, double p2, double q1, double q2)
{
	double p1q1 = p1 - q1;
	double p2q2 = p2 - q2;
	return sqrt(p1q1*p1q1 + p2q2 * p2q2);
}

int ringxy(Point3 p, int steps)
{
	double distance = euclid2d(p.getX(), p.getY(), 0, 0);
	return (int)(distance * steps) % 2;

}


int ringyz(Point3 p, int steps)
{
	double distance = euclid2d(p.getY(), p.getZ(), 0, 0);
	return (int)(distance * steps) % 2;

}


int ringxz(Point3 p, int steps)
{
	double distance = euclid2d(p.getX(), p.getZ(), 0, 0);
	return (int)(distance * steps) % 2;

}

int selectTextureFunction(int functionNumber, Point3 p, int steps)
{
	if (functionNumber == SOLID)
	{
		return solid(p);
	}
	if (functionNumber == CHECKERBOARD)
	{
		return checkerboard3(p, steps);
	}
	if (functionNumber == GRID)
	{
		return grid(p, steps);
	}
	if (functionNumber == DIAG)
	{
		return diag(p, steps);
	}
	if (functionNumber == RINGXY)
	{
		return ringxy(p, steps);
	}
	if (functionNumber == RINGXZ)
	{
		return ringyz(p, steps);
	}
	if (functionNumber == RINGYZ)
	{
		return ringxz(p, steps);
	}
}

//<<<<<<<<<<<<<<<<<<<<<<< default constructor >>>>>>>>>>>>>>>>>
void Cam::setDefaultCam(void)
{ // make default camera
	setShape(45.0f, 640 / (double)480, 0.1f, 200.0f); // good default values here
	set(5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0, 1, 0);
}

Cam::Cam()
{
	eye.set(0.0, 0.0, -1.0);
	n.set(0.0, 0.0, 1.0);
	u.set(0.0, 1.0, 0.0);
	v.set(1.0, 0.0, 0.0);
	setModelViewMatrix();	// inform OpenGL
}

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< rayTrace >>>>>>>>>>>>>>>>>>>>>>
//  like in camera-- nonmirror part

Color3 Cam::computeColor(scene& scn, Ray theRay, int which, Material whichmaterial,
	Point3 where, vector3 norm)
{
	Color3 objColor;
	Color3 lighting;
	Color3 highlights;
	Coefficients3 diff;
	Coefficients3 spec;
	Coefficients3 amb;
	double specExp;


	lighting = scn.diffuse(theRay, which, whichmaterial, where, norm);	// calculate diffuse lighting effects
	specExp = whichmaterial.specularExponent;
	highlights = scn.specular(theRay, which, where, specExp, norm);// calculate specular lighting effects
	objColor = whichmaterial.emissive;  //  emitted color of object
	amb = whichmaterial.ambient;  //  get ambient coefficients
	objColor.AddAmbient(scn.getAmbient(), amb); // add ambient coefficients * scene light
	diff = whichmaterial.diffuse;  //  get diffuse coefficients
	objColor.AddDiffuse(lighting, diff);  // add diffuse component
	spec = whichmaterial.specular;  //  get specular coefficients
	objColor.AddSpecular(highlights, spec);  // add specular component
	return objColor;
}

Color3 Cam::mirrorColor(scene& scn, Ray theRay, int which,
	Point3 where, vector3 norm, int reflectionsRemaining)
{
	Color3 objColor;
	Color3 lighting;
	Color3 highlights;
	Coefficients3 diff;
	Coefficients3 spec;
	Coefficients3 amb;
	Color3 refColor;
	Color3 mirColor;
	Color3 sumColor;
	Color3 black;
	black.set(0, 0, 0);
	float specExp;
	int inwhich;
	Point3 inwhere;
	vector3 innorm;
	float t;
	Ray reflection;
	//build the incident ray's reflection	
	reflection.setStart(where);
	reflection.setDir(theRay.dir - 2 * (theRay.dir * norm) * norm);
	t = scn.timeHit2(reflection);

	if (t > epsilon)/// bounced ray hits an object
	{
		inwhich = scn.whatHit2(reflection);  // object hit
		inwhere = scn.getnthobject(inwhich)->whereHit(reflection); // where it was hit 
		innorm = scn.getnthobject(inwhich)->normalHit(reflection); //normal there
		int textureNumber = scn.getnthobject(inwhich)->textureNumber;  // get 3d texture
		int texture;
		texture = selectTextureFunction(textureNumber, where, scn.getnthobject(which)->step);

		Material whichmaterial;

		if (texture == 0){
			whichmaterial = scn.sceneobjects[inwhich - 1]->mat1;

		}
		else{
			whichmaterial = scn.sceneobjects[inwhich - 1]->mat2;
		}
		//  color from object hit
		objColor = computeColor(scn, theRay, inwhich, whichmaterial, inwhere, innorm);
		if (reflectionsRemaining > 0) {

			if (whichmaterial.reflective.red > epsilon ||
				whichmaterial.reflective.blue > epsilon ||
				whichmaterial.reflective.green > epsilon)
			{
				mirColor = mirrorColor(scn, theRay, inwhich, inwhere, innorm,
					reflectionsRemaining - 1);
				refColor.set(whichmaterial.reflective.red * mirColor.red,
					whichmaterial.reflective.blue * mirColor.blue,
					whichmaterial.reflective.green * mirColor.green);

				sumColor.set(objColor.red + refColor.red,
					objColor.blue + refColor.blue,
					objColor.green + refColor.green);
				return sumColor;
			}
		}
		{
			return objColor;
		}
	}
	return black;
}

void Cam::raytrace(scene& scn, int blockSize)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //set to identity
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0, nCols, 0, nRows); // whole screen is the window
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING); // so glColor3f() will work
	Ray theRay;
	theRay.setStart(eye);
	double blockWidth = 2 * W / (double)nCols;
	double blockHeight = 2 * H / (double)nRows;
	Color3 clr;
	Point3 where;
	vector3 norm;
	Color3 objColor;
	Color3 lighting;
	Color3 highlights;
	Coefficients3 diff;
	Coefficients3 spec;
	Coefficients3 amb;
	double specExp;
	double x, y;
	int which;
	double t;
	for (int row = 0; row < nRows; row += blockSize)   // one pixel at a time
	{
		printf("row %d \n", row);
		for (int col = 0; col < nCols; col += blockSize)
		{
		//	printf("c%d ", col);
			//build the ray's direction				
			x = -W + col * blockWidth;
			y = -H + row * blockHeight;
			theRay.setDir(-nearDist * n.deltaX() + x * u.deltaX() + y * v.deltaX(),
				-nearDist * n.deltaY() + x * u.deltaY() + y * v.deltaY(),
				-nearDist * n.deltaZ() + x * u.deltaZ() + y * v.deltaZ());

			t = scn.timeHit2(theRay);
		//	printf("time %f ", t);
			//  find all intersections of the ray with objects in the scene
			//identify the intersection closest to the eye
			// find the color of the light returning to the eye
			// save the pixel color

			if (t > 0.0)
			{  // This is not specialized for spheres
				which = scn.whatHit2(theRay);
				where = scn.getnthobject(which)->whereHit(theRay);
				norm = scn.getnthobject(which)->normalHit(theRay);
				int textureNumber = scn.getnthobject(which)->textureNumber;  // get 3d texture
				//			printf("%d ",which);
				int texture;
				texture = selectTextureFunction(textureNumber, where, scn.getnthobject(which)->step);
			//	printf("%d", texture);
				Material whichmaterial;
				//  retrieve material

				if (texture == 0){
					whichmaterial = scn.sceneobjects[which - 1]->mat1;
				}
				else
				{
					whichmaterial = scn.sceneobjects[which - 1]->mat2;
				}
			
				objColor = computeColor(scn, theRay, which, whichmaterial, where, norm);
				if (whichmaterial.reflective.red > epsilon ||
					whichmaterial.reflective.blue > epsilon ||
					whichmaterial.reflective.green > epsilon)
				{
					//printf("%f ", whichmaterial.reflective.red);
					// pure mirror color
					Color3 mirColor;
					Color3 refColor;
					mirColor = mirrorColor(scn, theRay, which, where, norm, MAXREFLECTIONS);
					//  modified mirror color , scaled by reflective coefficients
					refColor.set(whichmaterial.reflective.red * mirColor.red,
						whichmaterial.reflective.blue * mirColor.blue,
						whichmaterial.reflective.green * mirColor.green);
				// add colors
					objColor.set(objColor.red + refColor.red,
						objColor.blue + refColor.blue,
					objColor.green + refColor.green);
				}

				Color3 boundedColor(unitIntervalBounded(objColor.red),
					unitIntervalBounded(objColor.green),
					unitIntervalBounded(objColor.blue));

				glColor3f(boundedColor.red,
					boundedColor.green,
					boundedColor.blue); // set current color 
			}
			else
			{
				//printf("b");
				glColor3f(scn.getBackground().red,
					scn.getBackground().green,
					scn.getBackground().blue);
			}
			glRecti(col, row, col + blockSize, row + blockSize);
		}
		glFlush();
	}
} // end of raytrace
//<<<<<<<<<<<<<<<<<<< setDisplay >>>>>>>>>>>>
void Cam::setDisplay(int numRows, int numCols)
{ // tell camera screen size
	nRows = numRows; 	nCols = numCols;
}

//<<<<<<<<<<<<<<<<<<<< set >>>>>>>>>>>>>>>>>>>
void Cam::set(Point3 Eye, Point3 look, vector3 up)
{ // make u, v, n vectors
	eye.set(Eye);
	n.set(eye.getX() - look.getX(), eye.getY() - look.getY(), eye.getZ() - look.getZ());  //create n
	u = cross(up, n);		//  u is up x n
	v = cross(n, u);			//  v is n x u
	u = normalize(u);
	v = normalize(v);
	n = normalize(n);	// normalize all of them
	setModelViewMatrix();	// inform OpenGL
}
//<<<<<<<<<<<<<<<<<<<<<<<<< set - takes 9 params >>>>>>>>>>>>>>>>>
void Cam::set(double ex, double ey, double ez, double lx, double ly, double lz, double upx, double upy, double upz)
{
	Point3 e(ex, ey, ez);
	Point3 look(lx, ly, lz);
	vector3 Up(upx, upy, upz);
	set(e, look, Up);		// refer to preceding routine for action
}
//<<<<<<<<<<<<<<<< setModelViewMatrix >>>>>>>>>>>>>>>>
void Cam::setModelViewMatrix(void)
{ // load modelview matrix with existing camera values
	double m[16];
	vector3 eVec(eye.getX(), eye.getY(), eye.getZ());
	m[0] = u.deltaX(); m[4] = u.deltaY(); m[8] = u.deltaZ();  m[12] = -eVec * u;
	m[1] = v.deltaX(); m[5] = v.deltaY(); m[9] = v.deltaZ();  m[13] = -eVec * v;
	m[2] = n.deltaX(); m[6] = n.deltaY(); m[10] = n.deltaZ();  m[14] = -eVec * n;
	m[3] = 0;          m[7] = 0;          m[11] = 0;           m[15] = 1.0;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(m);  ///load OpenGL's matrix mode
}
//<<<<<<<<<<<<<<<<<<<<<< setProjectionMatrix >>>>>>>>>>>>>
void Cam::setProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(viewAngle, aspect, nearDist, farDist);
	glMatrixMode(GL_MODELVIEW); // set its mode back again
}
//<<<<<<<<<<<<<<<<<<<<<<< setShape >>>>>>>>>>>>>>>>
void Cam::setShape(double vAngle, double asp, double nr, double fr)
{ // load projection matrix and camera values
	viewAngle = vAngle; // viewangle in degrees - must be < 180
	printf("angle %f ", viewAngle);
	aspect = asp; nearDist = nr; farDist = fr;
	printf("nearDist %f ", nearDist);
	H = nearDist * tan(M_PI * viewAngle / (2.0 * 180));
	W = aspect * H;
	setProjectionMatrix();
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<< setAspect >>>>>>>>>>>>>>>>>>>>>>
void Cam::setAspect(double asp)
{
	aspect = asp; W = aspect * H;
	setShape(viewAngle, aspect, nearDist, farDist);
}

//<<<<<<<<<<<<<<<<<<<<<< slide >>>>>>>>>>>>>>>>>>>>>>..
void Cam::slide(double du, double dv, double dn)
{ // slide both eye and lookAt by amount du * u + dv * v + dn * n;
	double x; double y; double z;
	x = eye.getX() + du * u.deltaX() + dv * v.deltaX() + dn * n.deltaX();
	y = eye.getY() + du * u.deltaY() + dv * v.deltaY() + dn * n.deltaY();
	z = eye.getZ() + du * u.deltaZ() + dv * v.deltaZ() + dn * n.deltaZ();
	eye.set(x, y, z);
	setModelViewMatrix();
}
//<<<<<<<<<<<<<<<<<<<<<<<< rotAxes >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void Cam::rotAxes(vector3& a, vector3& b, double angle)
{ // rotate orthogonal vectors a (like x axis) and b(like y axia) through angle degrees
	double ang = M_PI / 180 * angle;
	double C = cos(ang), S = sin(ang);
	vector3 t(C * a.deltaX() + S * b.deltaX(), C * a.deltaY() + S * b.deltaY(),
		C * a.deltaZ() + S * b.deltaZ()); // tmp for a vector
	b.set(-S * a.deltaX() + C * b.deltaX(), -S * a.deltaY() + C * b.deltaY()
		, -S * a.deltaZ() + C * b.deltaZ());
	a.set(t.deltaX(), t.deltaY(), t.deltaZ()); // put tmp into a'
}
//<<<<<<<<<<<<<<<<<<<<<<<< roll >>>>>>>>>>>>>>>>>>>>>>>>>>>
void Cam::roll(double angle)
{
	//roll the camera through angle degrees
	double cs = cos(M_PI / 180 * angle);
	double sn = sin(M_PI / 180 * angle);
	vector3 t = u;	// remember old u
	u.set(cs * t.deltaX() - sn * v.deltaX(), cs * t.deltaY() - sn * v.deltaY(),
		cs * t.deltaZ() - sn * v.deltaZ());
	v.set(sn * t.deltaX() + cs * v.deltaX(), sn * t.deltaY() + cs * v.deltaY(),
		sn * t.deltaZ() + cs * v.deltaZ());
	setModelViewMatrix();
}
//<<<<<<<<<<<<<<<<<<<<<<<< pitch >>>>>>>>>>>>>>>>>>>>>>>>>
void Cam::pitch(double angle)
{
	rotAxes(n, v, angle); setModelViewMatrix();
}
//<<<<<<<<<<<<<<<<<<<<<<<<< yaw >>>>>>>>>>>>>>>>>>>>>>>>
void Cam::yaw(double angle)
{
	rotAxes(u, n, angle); setModelViewMatrix();
}
//%%%%%%%%%%% end of class Cam %%%%%%%%%%%%%%%%%%%%%%
// Material methods
void Material::setDefault(){
	textureType = 0; // for none  // not used
	numParams = 0;     // not used
	//	reflectivity = transparency = 0.0;     // not used
	//	speedOfLight = specularExponent = 1.0;     // not used
	specularFraction = 0.0;     // not used
	surfaceRoughness = 1.0;     // not used
	ambient.set(0.1f, 0.1f, 0.1f);
	diffuse.set(0.8f, 0.8f, 0.8f);
	specular.set(0, 0, 0);
	emissive.set(0, 0, 0);
}
void Material::set(Material& m)
{
	textureType = m.textureType;     // not used
	numParams = m.numParams;     // not used
	for (int i = 0; i < numParams; i++) params[i] = m.params[i];     // not used
	//	transparency = m.transparency;     // not used
	//	speedOfLight = m.speedOfLight;     // not used
	//reflectivity = m.reflectivity;     
	specularExponent = m.specularExponent;     // not used
	specularFraction = m.specularFraction;     // not used
	surfaceRoughness = m.surfaceRoughness;     // not used
	ambient.set(m.ambient);
	diffuse.set(m.diffuse);
	specular.set(m.specular);
	emissive.set(m.emissive);
}

// Color3 methods
void Color3::add(Color3& src, Color3& refl)     // not used
{ // add the product of source color and reflection coefficient
	red += src.red   * refl.red;
	green += src.green * refl.green;
	blue += src.blue  * refl.blue;
}
void Color3::add(Color3   colr)     // not used
{ // add colr to this color
	red += colr.red; green += colr.green; blue += colr.blue;
}

void Color3::build4tuple(double v[])     // not used
{// load 4-tuple with this color: v[3] = 1 for homogeneous
	v[0] = red; v[1] = green; v[2] = blue; v[3] = 1.0f;
}

void Color3::AddDiffuse(Color3 lighting, Coefficients3 diffuse)

{
	red += diffuse.red   * lighting.red;
	green += diffuse.green * lighting.green;
	blue += diffuse.blue  * lighting.blue;
}

void Color3::AddSpecular(Color3 lighting, Coefficients3 specular)

{
	red += specular.red   * lighting.red;
	green += specular.green * lighting.green;
	blue += specular.blue  * lighting.blue;
}

void Color3::AddAmbient(Color3 amb, Coefficients3 ambcoeff)
{
	red += ambcoeff.red * amb.red;
	green += ambcoeff.green * amb.green;
	blue += ambcoeff.blue * amb.blue;
}

// Coefficient3 methods
void Coefficients3::add(Coefficients3& src, Coefficients3& refl)     // not used
{ // add the product of source color and reflection coefficient
	red += src.red   * refl.red;
	green += src.green * refl.green;
	blue += src.blue  * refl.blue;
}

//Affine4 methods
Affine4::Affine4(){ // make identity transform
	m[0] = m[5] = m[10] = m[15] = 1.0;
	m[1] = m[2] = m[3] = m[4] = 0.0;
	m[6] = m[7] = m[8] = m[9] = 0.0;
	m[11] = m[12] = m[13] = m[14] = 0.0;
}
void Affine4::setIdentityMatrix(){ // make identity transform
	m[0] = m[5] = m[10] = m[15] = 1.0;
	m[1] = m[2] = m[3] = m[4] = 0.0;
	m[6] = m[7] = m[8] = m[9] = 0.0;
	m[11] = m[12] = m[13] = m[14] = 0.0;
}
void Affine4::set(Affine4 a)// set this matrix to a
{
	for (int i = 0; i < 16; i++)
		m[i] = a.m[i];
}
//<<<<<<<<<<<<<< preMult >>>>>>>>>>>>
void Affine4::preMult(Affine4 n)
{// postmultiplies this with n
	double sum;
	Affine4 tmp;
	tmp.set(*this); // tmp copy
	// following mult's : this = tmp * n
	for (int c = 0; c < 4; c++)
	for (int r = 0; r < 4; r++)
	{
		sum = 0;
		for (int k = 0; k < 4; k++)
			sum += n.m[4 * k + r] * tmp.m[4 * c + k];
		m[4 * c + r] = sum;
	}// end of for loops
}// end of preMult()	
//<<<<<<<<<<<< postMult >>>>>>>>>>>
void Affine4::postMult(Affine4 n){// postmultiplies this with n
	double sum;
	Affine4 tmp;
	tmp.set(*this); // tmp copy
	for (int c = 0; c < 4; c++)// form this = tmp * n
	for (int r = 0; r < 4; r++)
	{
		sum = 0;
		for (int k = 0; k < 4; k++)
			sum += tmp.m[4 * k + r] * n.m[4 * c + k];
		m[4 * c + r] = sum;
	}// end of for loops
}
//  added functionality
void Affine4::putAffine()
{
	Affine4 temp;
	temp.set(*this);
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			printf(" %f  ", temp.m[r + 4 * c]);
		}
		printf("\n");
	}
}

//  matrices!!!

void mat2x2::set(double a[4])
{
	for (int r = 0; r < 2; r++)
	{
		for (int c = 0; c < 2; c++)
		{
			m[r][c] = a[r * 2 + c];
		}
	}
}
double mat2x2::det()
{
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

void mat3x3::set(double a[9])
{
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			m[r][c] = a[r * 3 + c];
		}
	}
}

double mat3x3::cofactor(int nr, int nc)
{
	mat2x2 temp;
	int row;
	int col;
	double f;
	for (int r = 0; r < 2; r++)
	{
		if (nr <= r)
		{
			row = r + 1;
		}
		else
		{
			row = r;
		}
		for (int c = 0; c < 2; c++)
		{
			if (nc <= c)
			{
				col = c + 1;
			}
			else
			{
				col = c;
			}
			f = m[row][col];
			//	printf(" %d %d ::: %f \n", row, col,f);
			temp.setone(r, c, f);
		}
	}
	//	printf(" %f \n", temp.det());
	return temp.det();

}


double mat3x3::det()  // by cofactors of first row
{
	double sum = 0.0;
	double x = -1;
	//	mat2x2 temp ;

	for (int column = 0; column < 3; column++)
	{
		x *= -1;
		sum += m[0][column] * x * cofactor(0, column);
	}
	return sum;
}

mat4x4  copy(mat4x4 incom)

{
	mat4x4 temp;


	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			//				printf(" %f  ", mm[r ][c ]);
			temp.mm[r][c] = incom.mm[r][c];
		}
	}
	return temp;
}
void mat4x4::set(Affine4 a)
{
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			mm[c][r] = a.m[c + 4 * r];
		}
	}
}

Affine4 toAffine4(mat4x4 mmm)  // back to Affine4
{
	Affine4 a;
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			a.m[r + 4 * c] = mmm.mm[r][c];
		}
	}
	return a;
}


double mat4x4::cofactor(int nr, int nc)
{
	mat3x3 temp;
	int row;
	int col;

	for (int r = 0; r < 3; r++)
	{
		if (nr <= r)
			row = r + 1;
		else
			row = r;

		for (int c = 0; c < 3; c++)
		{
			if (nc <= c)
				col = c + 1;
			else
				col = c;
			//		printf(" vv %f  ", mm[row][col]);
			temp.setone(r, c, mm[row][col]);
		}
	}
	return temp.det();
}

double mat4x4::det()  // by cofactors of first row
{
	double sum = 0.0;
	double x = -1;

	for (int column = 0; column < 4; column++)
	{
		x *= -1;
		sum += mm[0][column] * x * cofactor(0, column);
	}
	return sum;
}

mat4x4   transpose(mat4x4 mmm)   //   
{
	mat4x4 temp;
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			temp.mm[r][c] = mmm.mm[c][r];
		}
	}
	return temp;
}

double sgn(int i, int j)
{
	if ((i + j) % 2 == 0)
		return 1.0;
	else
		return -1.0;
}

mat4x4  inverse(mat4x4 mmm)  //  cofactor method
{
	mat4x4 temp;
	temp = copy(mmm);
	mat4x4 tt;
	double d;
	d = temp.det();
	double dinv;
	dinv = 1.0 / d;
	//					printf("d: %f    \n",   d );
	//					printf("dinv: %f    \n",   dinv );

	if (d = 0)
	{
		printf("  CAUTION:  NONINVERTIBLE MATRIX \n");
		return temp;
	}
	else
	{
		//		double p = 1.0;
		for (int r = 0; r < 4; r++)
		{
			for (int c = 0; c < 4; c++)
			{
				tt.setone(r, c, dinv * sgn(r, c)  * mmm.cofactor(r, c));
				//			p = p * -1.0;
				//				printf("mm: %f %f %f   ",   dinv , p  , mmm.cofactor(r,c) );
			}
		}
		return transpose(tt);
	}
}

//@@@@@@@@@@ AffineNode class @@@@@@@@@@@
class AffineNode{
	// used by Scene class to read SDL files
public:
	Affine4 * affn;
	Affine4 * invAffn;
	AffineNode * next;
	AffineNode()
	{
		next = NULL;
		affn = new Affine4; // new affine with identity in it
		invAffn = new Affine4; // and for the inverse
	}
	~AffineNode() //destructor
	{
		delete affn;
		delete invAffn;
	}
};

Point3 rayPos(Point3  s, vector3  v, double t)
{
	Point3 p(s.getX() + v.deltaX() * t,
		s.getY() + v.deltaY() * t,
		s.getZ() + v.deltaZ() * t);
	return p;
}
//<<<<<<<<<<<<<<<< rayPos >>>>>>>>>>>>>>>>>>>>>..
Point3 rayPos(Ray r, double t)
{
	return rayPos(r.start, r.dir, t);
}

//<<<<<<<<<<<<<<<<<<<< xfrmPoint >>>>>>>>>>>...
void xfrmPoint(Point3 res, Affine4 aff, Point3 pt)
{ // form res = aff * pt - include translation part
	res.set(aff.m[0] * pt.getX() + aff.m[4] * pt.getY() + aff.m[8] * pt.getZ() + aff.m[12],
		aff.m[1] * pt.getX() + aff.m[5] * pt.getY() + aff.m[9] * pt.getZ() + aff.m[13],
		aff.m[2] * pt.getX() + aff.m[6] * pt.getY() + aff.m[10] * pt.getZ() + aff.m[14]);
}
//<<<<<<<<<<<<<<<<<<<< xfrmVect >>>>>>>>>>>...
void xfrmVect(vector3 res, Affine4  aff, vector3  v)
{ // form res = aff * v 
	res.set(aff.m[0] * v.deltaX() + aff.m[4] * v.deltaY() + aff.m[8] * v.deltaZ(),
		aff.m[1] * v.deltaX() + aff.m[5] * v.deltaY() + aff.m[9] * v.deltaZ(),
		aff.m[2] * v.deltaX() + aff.m[6] * v.deltaY() + aff.m[10] * v.deltaZ());
}

//<<<<<<<<<<<<<<<<<<<< xfrmVect >>>>>>>>>>>...
void postTranXfrmVect(vector3& res, Affine4&  aff, vector3&  v)
{ // form res =   v * aff^T

	res.set(aff.m[0] * v.deltaX() + aff.m[4] * v.deltaY() + aff.m[8] * v.deltaZ(),
		aff.m[1] * v.deltaX() + aff.m[5] * v.deltaY() + aff.m[9] * v.deltaZ(),
		aff.m[2] * v.deltaX() + aff.m[6] * v.deltaY() + aff.m[10] * v.deltaZ());
	//	printf("\n %f %f %f ", res.deltaX(), res.deltaY(), res.deltaZ());

}
/**/
//<<<<<<<<<<<<<<<<<<<<<<<<<< xfrmRay >>>>>>>>>>>>>>>>>>>>>
void xfrmRay(Ray& gg, Affine4& aff, Ray& r)
{// g = aff * r // only the start part gets the translation component.
	Point3 pp;

	double xx, yy, zz;
	xx = aff.m[0] * r.start.getX() + aff.m[4] * r.start.getY() + aff.m[8] * r.start.getZ() + aff.m[12];
	yy = aff.m[1] * r.start.getX() + aff.m[5] * r.start.getY() + aff.m[9] * r.start.getZ() + aff.m[13];
	zz = aff.m[2] * r.start.getX() + aff.m[6] * r.start.getY() + aff.m[10] * r.start.getZ() + aff.m[14];

	pp.set(xx, yy, zz);
	gg.setStart(pp);
	vector3 vv;
	xx = aff.m[0] * r.dir.deltaX() + aff.m[4] * r.dir.deltaY() + aff.m[8] * r.dir.deltaZ();
	yy = aff.m[1] * r.dir.deltaX() + aff.m[5] * r.dir.deltaY() + aff.m[9] * r.dir.deltaZ();
	zz = aff.m[2] * r.dir.deltaX() + aff.m[6] * r.dir.deltaY() + aff.m[10] * r.dir.deltaZ();
	gg.setDir(xx, yy, zz);

}
//<<<<<<<<<<<<<<<<<<<< xfrmNormal >>>>>>>>>>>...
void xfrmNormal(vector3& res, Affine4& aff, vector3& v)
{ // form res = transpose(aff) * v 
	double xx, yy, zz;
	xx = aff.m[0] * v.deltaX() + aff.m[1] * v.deltaY() + aff.m[2] * v.deltaZ();
	yy = aff.m[4] * v.deltaX() + aff.m[5] * v.deltaY() + aff.m[6] * v.deltaZ();
	zz = aff.m[8] * v.deltaX() + aff.m[9] * v.deltaY() + aff.m[10] * v.deltaZ();
	res.set(xx, yy, zz);
}


double sphere::timeHit(Ray r)    // smallest positive time a ray hits a sphere
{
	Ray genray;
	xfrmRay(genray, aff, r);
	double aa, bb, cc, discrim;

	aa = genray.dir  * genray.dir;
	//printf("inside\n"); 
	vector3 temp(genray.start.getX(), genray.start.getY(), genray.start.getZ());

	bb = temp * genray.dir;
	cc = temp * temp - 1.0;
	if (aa == 0.0) // should never happen
	{
		printf("ERROR IN sphere \n");
		return -1000.0;
	}
	else
	{
		discrim = ((bb * bb) - (aa * cc));
		if (discrim < 0.0)
			return -5.0;  //  ray misses circle
		else
		{
			double time1;
			double time2;
			time1 = (-bb - sqrtf(discrim)) / aa;
			time2 = (-bb + sqrtf(discrim)) / aa;
			//			 	
			if (time1 < 0.0)
				return time2;
			else
				return time1;
		}
	}
}

void sphere::iama(){ printf("i am a sphere \n"); };

Point3 sphere::whereHit(Ray r) //  returns the place of contact
{
	double hitTime;
	hitTime = timeHit(r);
	Point3 contact;
	contact = r.start + (hitTime * r.dir);
	return contact;
}

vector3 sphere::normalHit(Ray r) //  returns the normal vector at the point of contact
{
	vector3 zero(0, 0, 0);
	Ray genray;
	xfrmRay(genray, aff, r);
	double aa, bb, cc, discrim;

	Point3 genHitPoint;
	aa = genray.dir  * genray.dir;

	vector3 temp(genray.start.getX(), genray.start.getY(), genray.start.getZ());

	bb = temp * genray.dir;
	cc = temp * temp - 1.0;
	if (aa == 0.0) // should never happen
	{
		printf("ERROR IN sphere \n");
		return zero;
	}
	else
	{

		discrim = ((bb * bb) - (aa * cc));
		if (discrim < 0.0)
			return zero;  //  ray misses circle
		else
		{
			double time1;
			double time2;
			time1 = (-bb - sqrtf(discrim)) / aa;
			time2 = (-bb + sqrtf(discrim)) / aa;
			//			 	
			if (time1 < 0.0)
				genHitPoint = genray.start + time2 * genray.dir;
			//	return time2;
			else
				genHitPoint = genray.start + time1 * genray.dir;
			//				return time1;
		}
	}



	Point3 contact;
	Point3 center(0, 0, 0);
	Point3 origin(0, 0, 0);
	vector3 v;
	vector3 res;
	contact = genHitPoint;
	v = contact - center;
	postTranXfrmVect(res, aff, v);
	//	printf("\n %f %f %f ", v.deltaX(), v.deltaY(), v.deltaZ());
	//	printf("\n %f %f %f ", res.deltaX(), res.deltaY(), res.deltaZ());
	return normalize(res);
}

double plane::timeHit(Ray r)    // smallest positive time a ray hits a plane
{
	Ray genray;
	xfrmRay(genray, aff, r);
	double denom = genray.dir.deltaZ();
	if (fabs(denom) < 0.0001) return -55;  // ray parallel to eye
	double time = -genray.start.getZ() / denom; // hit time
	//printf(" plane time %f \n", time);
	return time;  //  may be negative	
}


Point3 plane::whereHit(Ray r) //  returns the place of contact
{
	double hitTime;
	hitTime = timeHit(r);
	Point3 contact;
	contact = r.start + (hitTime * r.dir);
	return contact;
}

vector3 plane::normalHit(Ray r) //  returns the normal vector at the point of contact
{
	vector3 v;
	vector3 res;
	v.set(0, 0, 1);
	postTranXfrmVect(res, aff, v);
	//	printf("\n %f %f %f ", v.deltaX(), v.deltaY(), v.deltaZ());
	//	printf("\n %f %f %f ", res.deltaX(), res.deltaY(), res.deltaZ());
	return normalize(res);
}

void plane::iama(){ printf("i am a plane \n"); };
//////////////////////////
double square::timeHit(Ray r)    // smallest positive time a ray hits a square
{
	Ray genray;
	xfrmRay(genray, aff, r);
	double denom = genray.dir.deltaZ();
	if (fabs(denom) < 0.0001) return -55;  // ray parallel to eye
	double time = -genray.start.getZ() / denom; // hit time
	if (time < 0.0) return -66;
	double hx = genray.start.getX() + genray.dir.deltaX() * time;
	double hy = genray.start.getY() + genray.dir.deltaY() * time;
	if ((hx < -1.0) || (hx > 1.0)) return -77;//  miss in x dir
	if ((hy < -1.0) || (hy > 1.0)) return -88;//   miss in y dir
	//printf(" square time %f \n", time);
	return time;  //  may be negative	
}


Point3 square::whereHit(Ray r) //  returns the place of contact
{
	double hitTime;
	hitTime = timeHit(r);
	Point3 contact;
	contact = r.start + (hitTime * r.dir);
	return contact;
}

vector3 square::normalHit(Ray r) //  returns the normal vector at the point of contact
{
	vector3 v;
	vector3 res;
	v.set(0, 0, 1);
	postTranXfrmVect(res, aff, v);
	//	printf("\n %f %f %f ", v.deltaX(), v.deltaY(), v.deltaZ());
	//	printf("\n %f %f %f ", res.deltaX(), res.deltaY(), res.deltaZ());
	return normalize(res);
}

void square::iama(){ printf("i am a square \n"); };

//////////////////////////  superellipsoid -- 3D version of superellipse

double genDist(Point3 p, double exp){
	double sum = pow(fabs(p.getX()), exp) +
		pow(fabs(p.getY()), exp) +
		pow(fabs(p.getZ()), exp);
	double dist = pow(sum, 1 / exp);
	return dist;
}
//  implicit inside/outside function for superellipsoid; negative is inside
double implicitSuper(Ray r, double t, double exponent)
{
	return pow(abs(r.start.getX() + r.dir.deltaX() * t), exponent) +
		pow(abs(r.start.getY() + r.dir.deltaY() * t), exponent) +
		pow(abs(r.start.getZ() + r.dir.deltaZ() * t), exponent) - 1;
}



double superellipsoid::timeHit(Ray r)    // smallest positive time a ray hits a superellipsoid
{
	Ray genray;
	xfrmRay(genray, aff, r);  // ray from camera, transformed to generic world
	//  probe to find object, defined implicitly
	double t = 0;  // start at camera
	double maxdistance = 20.0;  //  furthest distance considered
	double finestDivision = 0.0001;  // finest step considered
	double increment = 0.01;  // initial increment -- smaller avoids missing object, larger is faster
	while ((implicitSuper(genray, t, superexponent) > 0) && (t < maxdistance + 1))  //  find first contact with object; max distance
	{
		t += increment;  // probe forward
	}
	t -= increment;  // back off one step, before contact
	if (t > maxdistance)
	{
		return -111;  //  clean miss
	}
	//  did not miss

	while (increment > finestDivision)    //  try finer increments to fine-tune contact
	{
		while (implicitSuper(genray, t, superexponent) > 0)
		{
			t += increment;  // probe forward
		}
		t -= increment;  // back off one step, before contact
		increment /= 10;  //  make increment finer by factor of 10
	}
	return t;
}


Point3 superellipsoid::whereHit(Ray r) //  returns the place of contact  -- same as others
{
	double hitTime;
	hitTime = timeHit(r);
	Point3 contact;
	contact = r.start + (hitTime * r.dir);
	return contact;
}

// need a version of pow that handles negatives correctly
double signedPow(double base, double exponent)
{
	if (base == 0)
	{
		return 1;
	}

	double value = pow(abs(base), exponent);
	if (base < 0)
	{
		return -value;
	}
	else
	{
		return value;
	}
}

vector3 superellipsoid::normalHit(Ray r) //  returns the normal vector at the point of contact
{
	//  need hit point in generic:  known to make contact
	Ray genray;
	xfrmRay(genray, aff, r);  // inverse transform ray from camera
	double denom = genray.dir.deltaZ();
	//if (fabs(denom) < 0.0001) return -56;  // ray parallel to eye, cannot happen
	double time = -genray.start.getZ() / denom; // hit time
	//  find hit point in generic object
	double hx = genray.start.getX() + genray.dir.deltaX() * time;
	double hy = genray.start.getY() + genray.dir.deltaY() * time;
	double hz = genray.start.getZ() + genray.dir.deltaZ() * time;
	Point3 p;
	p.set(hx, hy, hz);

	//	double distance = genDist(p, superexponent);//////  second value controls shape!!!!!
	//  hit is guaranteed

	// gradient vector is normal to surface
	vector3 res;
	vector3 partials;
	double px = superexponent * signedPow(hx, superexponent - 1);
	double py = superexponent * signedPow(hy, superexponent - 1);
	double pz = superexponent * signedPow(hz, superexponent - 1);
	partials.set(px, py, pz);  //  normal in generic
	postTranXfrmVect(res, aff, partials);  // transform partials back to nongeneric world
	return normalize(res);  // normalize the result
}

void superellipsoid::iama(){ printf("i am a superellipsoid \n"); };



////////////   circular wave

////////////////////////// 


double paramSurface(double x, double y)
{
	double dist = sqrt(x*x + y*y);

	return cos(dist) / (1 + dist);
}

//  implicit inside/outside function for circularWave; negative is inside (below surface)
double implicitParam(Ray r, double t)
{
	double x = r.start.getX() + r.dir.deltaX() * t;
	double y = r.start.getY() + r.dir.deltaY() * t;
	double z = r.start.getZ() + r.dir.deltaZ() * t;
	double implicit = paramSurface(x, y) - z;
	return implicit;
}


double partialX(double x, double y)
{
	double sumSquares = x*x + y*y;
	if (sumSquares < epsilon){ return 1; }  // avoid division by zero
	double rootSumSquares = sqrt(sumSquares);
	double partial = (x * (cos(rootSumSquares) / (rootSumSquares)-(1 + rootSumSquares) *sin(rootSumSquares) / rootSumSquares)) /
		((1 + rootSumSquares)*(1 + rootSumSquares));
	return partial;
}

double partialY(double x, double y)
{
	double sumSquares = x*x + y*y;
	if (sumSquares < epsilon){ return 1; }  // avoid division by zero
	double rootSumSquares = sqrt(sumSquares);
	double partial = partial = (y * (cos(rootSumSquares) / (rootSumSquares)-(1 + rootSumSquares) *sin(rootSumSquares) / rootSumSquares)) /
		((1 + rootSumSquares)*(1 + rootSumSquares));
	return partial;
}


double circularWave::timeHit(Ray r)    // smallest positive time a ray hits a circularWave
{

	Ray genray;
	xfrmRay(genray, aff, r);  // ray from camera, transformed to generic world
	//  probe to find object, defined implicitly
	double t = 0;  // start at camera
	double maxdistance = 100.0;  //  furthest distance considered
	double finestDivision = 0.01;  // finest step considered
	double increment = 1.0;  // initial increment -- smaller avoids missing object, larger is faster
	 //	printf("%f", implicitParam(genray, t));
	while ((implicitParam(genray, t) > 0) && (t < maxdistance + 1))  //  find first contact with object; max distance
	{
		t += increment;  // probe forward
			//printf("%f", implicitParam(genray, t));
	}
	
	if (t > maxdistance)
	{
		return -112;  //  clean miss
	}
	//  did not miss
	t -= increment;  // back off one step, before contact
	while (increment > finestDivision)    //  try finer increments to fine-tune contact
	{
		while (implicitParam(genray, t) > 0)
		{
			t += increment;  // probe forward
		}
		t -= increment;  // back off one step, before contact
		increment /= 10;  //  make increment finer by factor of 10
	}
//	printf(" %f", t);
	return t;
}


Point3 circularWave::whereHit(Ray r) //  returns the place of contact  -- same as others
{
	double hitTime;
	hitTime = timeHit(r);
	Point3 contact;
	contact = r.start + (hitTime * r.dir);
	return contact;
}


vector3 circularWave::normalHit(Ray r) //  returns the normal vector at the point of contact
{
	//  need hit point in generic:  known to make contact
	Ray genray;
	xfrmRay(genray, aff, r);  // inverse transform ray from camera
	double denom = genray.dir.deltaZ();
	//if (fabs(denom) < 0.0001) return -56;  // ray parallel to eye, cannot happen
	double time = -genray.start.getZ() / denom; // hit time
	//  find hit point in generic object
	double hx = genray.start.getX() + genray.dir.deltaX() * time;
	double hy = genray.start.getY() + genray.dir.deltaY() * time;
	double hz = genray.start.getZ() + genray.dir.deltaZ() * time;
	Point3 p;
	p.set(hx, hy, hz);

	//	double distance = genDist(p, superexponent);//////  second value controls shape!!!!!
	//  hit is guaranteed

	// gradient vector is normal to surface
	vector3 res;
	vector3 partials;
	double px = partialX(hx, hy);
	double py = partialY(hx, hy);
	double pz = 1;
	partials.set(px, py, pz);  //  normal in generic
	postTranXfrmVect(res, aff, partials);  // transform partials back to nongeneric world
	return normalize(res);  // normalize the result
}

void circularWave::iama(){ printf("i am a circularWave \n"); };



void scene::setnthobject(int n, sceneobject *c)		// set a sceneobject 
{
	sceneobjects[n - 1] = c;
}

void scene::setnthlight(int n, light p)		// set a sceneobject as a light 
{
	sceneobjectlight[n - 1] = p;
}

double scene::timeHit2(Ray r)
{
	double min = -111.0;
	double time;
	sceneobject * item;
	for (int i = 0; i < activeobjects; i++)
	{
		item = sceneobjects[i];
		//item->iama();
		time = item->timeHit(r);
		//printf("object %d time %f \n", i,time);
		if (((time > epsilon) && (time < min)) || ((time > epsilon) && (min <= 0.0)))
		{
			min = time;
			 		//	printf("new min time %f \n",time  );
		}
	}

	return min;
}

int scene::whatHit2(Ray r)
{
	int what = -1;
	double min = -112.0;
	double time;
	Ray tr;


	for (int i = 0; i < activeobjects; i++)
	{

		time = sceneobjects[i]->timeHit(r);
		//	 			printf("sphere %d time %f \n", i,time);

		if (((time > epsilon) && (time < min)) || ((time > epsilon) && (min <= 0.0)))
		{
			what = i;
			min = time;
			//	printf("new min time %f \n",time  );

		}
	}
	return 1 + what;
}

Point3 scene::whereHit2(Ray r)
{
	//  this is NOT efficient code.
	Point3 where;
	//  now, find the segment with that hit time
	double time;
	double min;
	min = timeHit2(r);

	for (int i = 0; i < activeobjects; i++)
	{
		time = sceneobjects[i]->timeHit(r);
		if (time == min)
		{
			where = sceneobjects[i]->whereHit(r);
			//			printf("new hit:  sphere \n");
			return where;

		}
	}
	return where;
}

Color3 scene::diffuse(Ray r, int what, Material mat, Point3 where, vector3 norm)
{
	Color3 build;
	build.set(0.0, 0.0, 0.0);  // initially 0
	Ray ra; // ray to light source from contact point
	double inner;
	double time;
	int which;

	for (int i = 0; i < activelight; i++)   //  go through lights
	{
		ra.setStart(where);
		ra.setDir(sceneobjectlight[i].position - where);    /// vector, contact to light
		inner = normalize(ra.dir) * norm; //  note -- lambert built in, both normalized

		if (inner > 0.0)  //  light will fall on point -- check if blocked
		{
			time = timeHit2(ra);  // positive if light obscured
			which = whatHit2(ra);
				//	printf(" blocked %f  ", time );
			if (which != what)
			{
				if (time < epsilon)  //  epsilon -- to avoid current surface
				{
					build.red = build.red + inner * sceneobjectlight[i].light_color.red;
					build.green = build.green + inner * sceneobjectlight[i].light_color.green;
					build.blue = build.blue + inner * sceneobjectlight[i].light_color.blue;
				}
			}
		}
	}
	return build;
}
////  checkerboard texture
int scene::checkerboard3(Point3 p){
	int selector = (int)(
		floor(p.getX() * steps) +
		floor(p.getY() * steps) +
		floor(p.getZ() * steps)) % 2;
	return selector;
}

Color3 scene::specular(Ray r, int what, Point3 where, double PhongExp, vector3 norm)
{
	Color3 build;
	build.set(0.0, 0.0, 0.0);  // initially 0
	Ray ra; // ray to light source from contact point
	double inner;
	double time;
	int which;
	vector3 h;
	for (int i = 0; i < activelight; i++)   //  go through lights
	{
		ra.setStart(where);
		ra.setDir(sceneobjectlight[i].position - where);    /// vector, contact to light
		h = ra.dir - r.dir;  // the halfway vector

		inner = normalize(h) * norm; //  note -- phong built in, both normalized
		if (inner > 0.0)  //  light will fall on point -- check if blocked
		{
			time = timeHit2(ra);  // positive if light obscured
			which = whatHit2(ra);
			if (which != what)
			{
				if (time < epsilon)  //  epsilon -- to avoid current surface
				{
					build.red = build.red + pow(inner, PhongExp) * sceneobjectlight[i].light_color.red;
					build.green = build.green + pow(inner, PhongExp)  * sceneobjectlight[i].light_color.green;
					build.blue = build.blue + pow(inner, PhongExp)  * sceneobjectlight[i].light_color.blue;
				}
			}
		}
	}
	return build;
}