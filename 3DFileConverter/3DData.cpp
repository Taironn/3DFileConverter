#include "3DData.h"
using namespace FormatConverter;
//Default constructor sets the cached flag false
FormatConverter::D3Data::D3Data() {
	faces_cached = false;
}
//Reserve constructor if heuristics are known
FormatConverter::D3Data::D3Data(size_t vert, size_t norm, size_t tc, size_t fc) {
	faces_cached = false;
	vertices.reserve(vert);
	normals.reserve(norm);
	tcoords.reserve(tc);
	faces.reserve(fc);
}
//Caches faces for faster exporting
const std::vector<CachedFace>& FormatConverter::D3Data::CachedFaces()
{
	if (faces_cached) return cached_faces;
	//The space needed is known
	cached_faces.reserve(faces.size());
	CachedFace temp;
	for (size_t i = 0; i < faces.size(); i++)
	{
		temp.V1 = &vertices[faces[i].V1 - 1];
		temp.V2 = &vertices[faces[i].V2 - 1];
		temp.V3 = &vertices[faces[i].V3 - 1];
		if (normals.size() > 0)
		{
			temp.N1 = &normals[faces[i].N1 - 1];
			temp.N2 = &normals[faces[i].N2 - 1];
			temp.N3 = &normals[faces[i].N3 - 1];
		}
		if (tcoords.size() > 0)
		{
			temp.T1 = &tcoords[faces[i].T1 - 1];
			temp.T2 = &tcoords[faces[i].T2 - 1];
			temp.T3 = &tcoords[faces[i].T3 - 1];
		}

		cached_faces.push_back(temp);
	}
	//Setting the cached flag
	faces_cached = true;
	return cached_faces;
}

//Converts stored normals into unit vectors
void FormatConverter::D3Data::NormalizeNormals()
{
	for (size_t i = 0; i < normals.size(); i++)
	{
		normals[i] = normals[i].unit();
	}
}

//Calculates normal of triangle with right hand rule, returns unit normal
Normal FormatConverter::Utils::calculate_normal(const Vertex & a, const Vertex & b, const Vertex & c)
{
	Vertex u = b - a;
	Vertex v = c - a;
	return Normal{ u.y*v.z - u.z*v.y, u.z*v.x - u.x*v.z,u.x*v.y - u.y*v.x }.unit();
}

//Calculates cross product of two normals
Normal FormatConverter::Utils::cross(const Normal & u, const Normal & v)
{
	return Normal{ u.y*v.z - u.z*v.y, u.z*v.x - u.x*v.z,u.x*v.y - u.y*v.x };
}

//Calculates average of three normals, returns unit normal
Normal FormatConverter::Utils::average(const Normal & a, const Normal & b, const Normal & c)
{
	return Normal{ (a.x + b.x + c.x) / 3.f,(a.y + b.y + c.y) / 3.f ,(a.y + b.y + c.y) / 3.f }.unit();
}

//Calculates surface of a face
float FormatConverter::Utils::calculate_surface(const CachedFace & c)
{
	Vertex u = *c.V2 - *c.V1;
	Vertex v = *c.V3 - *c.V1;
	return 0.5f*(u*v).length();
}

//Calculate surface area of a mesh
float FormatConverter::Utils::calculate_surface_area(D3Data & data)
{
	auto cachedData = data.CachedFaces();
	float surface = 0;
	for (size_t i = 0; i < cachedData.size(); i++)
	{
		surface += calculate_surface(cachedData[i]);
	}
	return surface;
}

//Counts specified character occurences in given string
unsigned int FormatConverter::Utils::count_occurences(std::string & s, char c)
{
	unsigned int count = 0;
	for (size_t i = 0; i < s.size(); i++)
	{
		if (s[i] == c) ++count;
	}

	return count;
}

//Calculates signed volume of a triangle
float FormatConverter::Utils::signed_volume_of_triangle(const Vertex & a, const Vertex & b, const Vertex & c)
{
	return (1.f / 6.f)*(-c.x*b.y*a.z + b.x*c.y*a.z + c.x*a.y*b.z
		- a.x*c.y*b.z - b.x*a.y*c.z + a.x*b.y*c.z);
}

//Calculates volume of given mesh
float FormatConverter::Utils::calculate_volume(D3Data & data)
{
	float sum = 0.f;
	auto faces = data.CachedFaces();
	for (size_t i = 0; i < faces.size(); i++)
	{
		sum += signed_volume_of_triangle(*faces[i].V1, *faces[i].V2, *faces[i].V3);
	}

	return fabsf(sum);
}

//Overload of - for substraction of two vertices
Vertex FormatConverter::Vertex::operator-(const Vertex & rhs) const
{
	return Vertex{ this->x - rhs.x, this->y - rhs.y, this->z - rhs.z, 0 };
}

//Calculates size of a vertex vector
float FormatConverter::Vertex::length() const
{
	return sqrtf(x*x + y*y + z*z);
}

//Overload of * for cross product of two vertice vectors
Vertex FormatConverter::Vertex::operator*(const Vertex & rhs) const
{
	return Vertex{ y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x };
}

//Returns normalized normal vector
Normal FormatConverter::Normal::unit() const
{
	float siz = sqrtf(x*x + y*y + z*z);
	return Normal{ x / siz,y / siz,z / siz };
}
