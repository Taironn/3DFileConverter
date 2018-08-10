#ifndef UNIQUE_StlWriter
#define UNIQUE_StlWriter
#include "ConverterBase.h"
namespace FormatConverter {
	class StlWriter : public Writer{
	public :
		/*
		Calls write with defalut n_type, meaning the normals will be calculated
		by the right hand rule
		Throws FileException if not able to open file
		Returns true on successful writing.
		*/
		virtual bool write(std::string& path, D3Data& data);

		/*
		Writes an stl file to the given path from the data parameter.
		n_type specifies the normals written in the stl
		'0' means all zero normals will be written
		'c' means normals will be calculated by the right hand rule - this is the default
		'a' means normals will be averaged from data - if there are no normals,
			the fall back is 'c'
		Throws FileException if not able to open file
		Returns true on successful writing.
		*/
		bool write(std::string& path, D3Data& data, char n_type);

	protected:
		//Writes out a vertice in binary format to given ofstream
		void writeVertex(const Vertex* v, std::ofstream& ofs);
		//Writes out a normal in binary format to given ofstream
		void writeNormal(const Normal* n, std::ofstream& ofs);
	};
}
#endif