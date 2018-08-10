#include "StlWriter.h"
#include <fstream>
using namespace FormatConverter;
//Calls write with default settings
bool StlWriter::write(std::string & path, D3Data & data)
{
	return write(path, data, 'c');
}

//Writes given data object onto given path, n_type defines what normals will be written
bool StlWriter::write(std::string& path, D3Data& data, char n_type)
{
	//Throwin exception if unable to open file
	std::ofstream outfile(path, std::ios::binary | std::ios::out | std::ios::trunc);
	if (!outfile.is_open())
		throw FileException();
	else
	{
		//Writing binary output
		//Header
		char header[80]="FormatConverter stl file";
		outfile.write(header, 80);

		uint32_t num_of_triangles = data.faces.size();
		//Forcing little endian according to the documentation
		uint8_t bytes[4];
		bytes[0] = (num_of_triangles) & 0xFF;
		bytes[1] = (num_of_triangles >> 8) & 0xFF;
		bytes[2] = (num_of_triangles >> 16) & 0xFF;
		bytes[3] = (num_of_triangles >> 24) & 0xFF;
		outfile.write((char*)bytes, 4);

		//Asking for cached values
		auto cachedData = data.CachedFaces();
		//Only using the cached values from now on

		//Loop for triangle data
		Normal n_null{ 0, 0, 0 };
		uint16_t attr_byte_count = 0;
		if (data.normals.size() == 0) n_type = 'c';
		for (size_t i = 0; i < cachedData.size(); i++)
		{
			//Writing normal values depending on parameter n_type
			switch (n_type)
			{
			case '0':
				writeNormal(&n_null, outfile);
				break;
			case 'a':
				writeNormal(&Utils::average(*cachedData[i].N1, *cachedData[i].N2,
					*cachedData[i].N3), outfile);
				break;
			default:
				writeNormal(&Utils::calculate_normal(*cachedData[i].V1,
					*cachedData[i].V2, *cachedData[i].V3), outfile);
				break;
			}

			//Writing vertice values
			writeVertex(cachedData[i].V1, outfile);
			writeVertex(cachedData[i].V2, outfile);
			writeVertex(cachedData[i].V3, outfile);

			//Writing attribute byte count - 0 by default
			outfile.write((char*)&attr_byte_count, sizeof(uint16_t));
		}

	}
	outfile.close();
	return true;
}

//Writes out Vertex in binary format to given ofstream object
void FormatConverter::StlWriter::writeVertex(const Vertex* v, std::ofstream & ofs)
{
	ofs.write((char*)(&(v->x)), sizeof(float));
	ofs.write((char*)(&(v->y)), sizeof(float));
	ofs.write((char*)(&(v->z)), sizeof(float));
}

//Writes out Normal in binary format to given ofstream object
void FormatConverter::StlWriter::writeNormal(const Normal * n, std::ofstream & ofs)
{
	ofs.write((char*)(&(n->x)), sizeof(float));
	ofs.write((char*)(&(n->y)), sizeof(float));
	ofs.write((char*)(&(n->z)), sizeof(float));
}
