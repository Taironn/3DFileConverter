#ifndef UNIQUE_ObjLoader
#define UNIQUE_ObjLoader
#include "ConverterBase.h"
namespace FormatConverter {
	class CheckedObjLoader : public Loader {
	public:
		/*
		Throws FileException and InvalidFormatException
		Default settings are index validity checking,
		which includes negative index swapping and
		no triangularization.
		Using move semantics.
		*/
		virtual D3Data load(std::string& path);

		/*
		Throws FileException and InvalidFormatException
		Implementation is inline, due to performance considerations
		Not using the tokenizing function at triangle faces	by default,
		this can be turned on by the bool parameter. Only works for convex
		polygons. If turned off only the first 3 vertices are taken into account
		*/
		D3Data load(std::string& path, bool check_index_validity, bool fix_neg,
			bool basic_triangularitaion = false);

	protected:

		//Swaps negative indexes accordingly
		void fix_negative_indexes(D3Data& data);

		/*
		Triangulates the polygon using a simple iteration from one vertex
		Only works for convex polygons
		Pushes the created faces from the string to the given D3Data object
		String must contaion only the data chunks without the first f and space!
		*/
		void triangulate_polygon(D3Data& data, std::string& line);

		/*
		Separates and converts an input face line into VertexDatas
		Throws invalid_argument and out_ofrange exceptions if formatting is incorrect
		These have to be converted to InvalidFormatExceptions outside where line number is known
		The given string must contaion only the data chunks without the first f and space!
		This method accepts face lines with not same tpye chunks of data. If this is a problem,
		checking should be done.
		*/
		std::vector<VertexData> tokenize_obj_face_line(std::string& line);
	};
}
#endif
