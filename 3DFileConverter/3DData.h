#ifndef UNIQUE_3DData
#define UNIQUE_3DData
#include<vector>

namespace FormatConverter {
	//Structs for storing 3D data
	struct Vertex {
		float x;
		float y;
		float z;
		float w;

		//Simple substraction
		Vertex operator-(const Vertex& rhs) const;
		//Absolute size of vector
		float length() const;
		//Cross product
		Vertex operator*(const Vertex& rhs) const;
	};

	struct Normal {
		float x;
		float y;
		float z;

		//Returns the normalized version of the Normal
		Normal unit() const;
	};

	struct Tcoord {
		float u;
		float v;
		float w;
	};

	struct Face {
		long V1;
		long V2;
		long V3;
		long N1;
		long N2;
		long N3;
		long T1;
		long T2;
		long T3;
	};

	//For faster computation during exports
	struct CachedFace{
		Vertex* V1;
		Vertex* V2;
		Vertex* V3;
		Normal* N1;
		Normal* N2;
		Normal* N3;
		Tcoord* T1;
		Tcoord* T2;
		Tcoord* T3;
	};

	//Helper structure for triangularization
	struct VertexData
	{
		long vertex_index;
		long normal_index;
		long tcoord_index;
	};

	//Class for storing the 3D data
	class D3Data{
	private:
		std::vector<CachedFace> cached_faces;
	public:
		std::vector<Vertex> vertices;
		std::vector<Normal> normals;
		std::vector<Tcoord> tcoords;
		std::vector<Face> faces;

		bool faces_cached;

		//Default constructor
		//Sets cached flag to false on default
		D3Data();

		//Memory reserving constructor, if some heuristics or the concrete
		//amount of data is known
		D3Data(size_t vert, size_t norm, size_t tc, size_t fc);

		/*
		Caches faces for faster export, returns a reference to the created vector
		and sets the cached flag true.
		If it has been already created, it only passes the reference.
		*/
		const std::vector<CachedFace>& CachedFaces();

		/*
		Normalizes stored normals
		*/
		void NormalizeNormals();

	};

	//Static utility class
	class Utils {
	public:		
		//Calculates normal according to right hand rule
		//Returns unit normals
		static Normal calculate_normal(const Vertex& a, const Vertex& b, const Vertex& c);

		//Calculates cross product of two normals
		static Normal cross(const Normal& u, const Normal& v);

		//Calculates average of three normals
		//Returns unit normals
		static Normal average(const Normal&a, const Normal& b, const Normal& c);

		//Calculates surface of triangle
		static float calculate_surface(const CachedFace& c);

		//Calculates surface area of given model
		static float calculate_surface_area(D3Data& data);

		//Counts specified character occurences in given string
		static unsigned int count_occurences(std::string& s, char c);

		//Calculates signed volume of a triangle given by its vertices
		static float signed_volume_of_triangle(const Vertex& a, const Vertex& b, const Vertex& c);

		//Calculates volume of mesh by calculating the signed volumes of tetrahedrons
		//built on the triangles
		static float calculate_volume(D3Data& data);
	};
}

#endif