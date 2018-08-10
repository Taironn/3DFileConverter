#ifndef UNIQUE_ConverterBase
#define UNIQUE_ConverterBase

#include "3DData.h"
#include <exception>
#include <string>
namespace FormatConverter {
	/*
	Abstract base class for writers.
	Has a virtual write function that has to be overriden.
	*/
	class Writer {
	public:
		/*
		Writes given data to given path. Format depends on child class.
		Throws FileException if the file given by path cannot be opened.
		*/
		virtual bool write(std::string& path, D3Data& data) = 0;
		virtual ~Writer() {};
	};
	/*

	Abstract base class for loaders.
	Has a virtual load function that has to be overriden.
	*/
	class Loader {
	public:
		/*
		The return value is allocated by the compiler and moved.
		Throws FileException if the file given by path cannot be opened,
		and InvalidFormatException if the file is not in correct format
		for the specific loader.
		*/
		virtual D3Data load(std::string& path) = 0;
		virtual ~Loader() {};
	};

	//Exception for file handling
	class FileException : public std::exception{
		using std::exception::exception;
	};

	//Exception for signalling invalid format
	class InvalidFormatException : public std::exception {
		using std::exception::exception;
	};
}
#endif
