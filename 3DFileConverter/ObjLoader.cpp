#include "ObjLoader.h"
#include <fstream>
using namespace FormatConverter;
/*
Calls the specified load function. The default settings are index checking, which
includes the swap of negative indexes, therefore no need for negative index fixing.
*/
D3Data FormatConverter::CheckedObjLoader::load(std::string& path)
{
	return load(path, true, false, false);
}
/*
Loads the obj file into the D3Data middle format. Checks for file validity. If the
obj specifications are not followed, throws InvalidFormatException. Index validity
checking can be turned off, and negative value swapping for positive (for indexing)
can be turned on. Throws FileException if the file cannot be opened on the given path.

If the file is sensible, meaning it can be interpreted by ignoring lines that does not
start with a valid character there will be no exception thrown, and the file will be
parsed.
*/
D3Data CheckedObjLoader::load(std::string& path, bool check_index_validity, bool fix_neg, bool basic_triangularitaion)
{
	/*
	Creating data file and opening file on given path.
	*/
	D3Data data = D3Data();
	std::ifstream objfile(path);
	std::string line;
	if (!objfile.is_open())
		throw FileException();
	else
	{
		//Line counting for better exception information.
		int line_num = 0;
		try
		{
			//Boolean for signalling that there was a face in the file
			//Therefore there should not be any more vertices/normals/texcoords
			bool n_faces = false;
			Vertex vtemp;
			Normal ntemp;
			Tcoord ttemp;
			Face ftemp;
			//Indexes for string processing
			std::string::size_type sz;
			std::string::size_type pos;
			while (getline(objfile, line))
			{
				line_num += 1;
				//Switch for first character
				switch (line[0])
				{
					//Starting with v
				case 'v':
					//v type line after faces. Throwing InvalidFormatException.
					if (n_faces) throw InvalidFormatException("Vertex/Normal/Texccord after faces");
					switch (line[1])
					{
						//Vertex case
					case ' ':
						//The rest has to be in the same format
						//If not InvalidFormatException will be thrown
						sz = pos = 2;
						vtemp.x = std::stof(line.substr(pos), &sz);
						pos += sz;
						vtemp.y = std::stof(line.substr(pos), &sz);
						pos += sz;
						vtemp.z = std::stof(line.substr(pos), &sz);
						pos += sz;
						//w value is 0 by default
						if (line.size() <= pos) vtemp.w = 0;
						else {
							vtemp.w = std::stof(line.substr(pos), &sz);
							if (vtemp.w < 0 || vtemp.w > 1)
								throw InvalidFormatException(("Invalid vertice w property. Line: " + std::to_string(line_num)).c_str());
						}
						data.vertices.push_back(vtemp);
						break;
						//Normal case - same format after that goes like at vertice case
					case 'n':
						sz = pos = 3;
						ntemp.x = std::stof(line.substr(pos), &sz);
						pos += sz;
						ntemp.y = std::stof(line.substr(pos), &sz);
						pos += sz;
						ntemp.z = std::stof(line.substr(pos), &sz);
						data.normals.push_back(ntemp);
						break;
						//Texcoord case
					case 't':
						sz = pos = 3;
						ttemp.u = std::stof(line.substr(pos), &sz);
						pos += sz;
						ttemp.v = std::stof(line.substr(pos), &sz);
						pos += sz;
						//w value is 0 by default
						if (line.size() <= pos) ttemp.w = 0;
						else {
							ttemp.w = std::stof(line.substr(pos), &sz);
							if (ttemp.w < 0 || ttemp.w > 1)
								throw InvalidFormatException(("Invalid texcoord w property. Line: " + std::to_string(line_num)).c_str());
						}
						data.tcoords.push_back(ttemp);
						break;
						//If there is anything else after v it is an invalid line
						//Throwing InvalidFormatException
					default:
						throw InvalidFormatException(("Invalid line. Line: " + std::to_string(line_num)).c_str());
						break;
					}
					break;
					//Face case - Only works for triangles, ignores the rest of the vertices
				case 'f':
					//There cannot be vertices/normals/texcoords after that
					n_faces = true;
					//Triangularization check if set
					if (basic_triangularitaion)
					{
						//If there are more than 3 data chunks, using triangularization
						if (Utils::count_occurences(line.substr(2), ' ') > 2)
						{
							//Breaking out of case 'f'
							triangulate_polygon(data, line.substr(2));
							break;
						}
					}
					sz = pos = 2;
					ftemp.V1 = stol(line.substr(pos), &sz);
					pos += sz;
					//Simple vertices case
					if (line[pos] == ' ')
					{
						ftemp.V2 = stol(line.substr(pos), &sz);
						pos += sz;
						ftemp.V3 = stol(line.substr(pos), &sz);
						ftemp.N1 = ftemp.N2 = ftemp.N3 = ftemp.T1 = ftemp.T2 = ftemp.T3 = 0;
					}
					//Multiple information case
					else if (line[pos] == '/')
					{
						//Texcoords missing case
						if (line[pos + 1] == '/')
						{
							pos += 2;
							ftemp.N1 = stol(line.substr(pos), &sz);
							pos += sz;
							ftemp.V2 = stol(line.substr(pos), &sz);
							pos += sz + 2;
							ftemp.N2 = stol(line.substr(pos), &sz);
							pos += sz;
							ftemp.V3 = stol(line.substr(pos), &sz);
							pos += sz + 2;
							ftemp.N3 = stol(line.substr(pos), &sz);
							ftemp.T1 = ftemp.T2 = ftemp.T3 = 0;
						}
						else
						{
							pos += 1;
							ftemp.T1 = stol(line.substr(pos), &sz);
							pos += sz;
							//All three data present case
							//If format not valid stol will throw an exception,
							//that is caught and converted to InvalidFormatException later
							if (line[pos] == '/') {
								pos += 1;
								ftemp.N1 = stol(line.substr(pos), &sz);
								pos += sz;
								ftemp.V2 = stol(line.substr(pos), &sz);
								pos += sz + 1;
								ftemp.T2 = stol(line.substr(pos), &sz);
								pos += sz + 1;
								ftemp.N2 = stol(line.substr(pos), &sz);
								pos += sz;
								ftemp.V3 = stol(line.substr(pos), &sz);
								pos += sz + 1;
								ftemp.T3 = stol(line.substr(pos), &sz);
								pos += sz + 1;
								ftemp.N3 = stol(line.substr(pos), &sz);
							}
							//Normals missing case
							//If format not valid stol wil throw an exception that is handled later
							else {
								ftemp.V2 = stol(line.substr(pos), &sz);
								pos += sz + 1;
								ftemp.T2 = stol(line.substr(pos), &sz);
								pos += sz;
								ftemp.V3 = stol(line.substr(pos), &sz);
								pos += sz + 1;
								ftemp.T3 = stol(line.substr(pos), &sz);
								ftemp.N1 = ftemp.N2 = ftemp.N3 = 0;
							}
						}
					}
					//If after the first value the next character is neither space or / the format is invalid
					else throw InvalidFormatException(("Invalid face format. Line: " + std::to_string(line_num)).c_str());
					//Index validity check if set
					if (check_index_validity) {
						//Fixing negative indexes
						if (ftemp.V1 < 0) ftemp.V1 += data.vertices.size() + 1;
						if (ftemp.V2 < 0) ftemp.V2 += data.vertices.size() + 1;
						if (ftemp.V3 < 0) ftemp.V3 += data.vertices.size() + 1;

						if (ftemp.N1 < 0) ftemp.N1 += data.normals.size() + 1;
						if (ftemp.N2 < 0) ftemp.N2 += data.normals.size() + 1;
						if (ftemp.N3 < 0) ftemp.N3 += data.normals.size() + 1;

						if (ftemp.T1 < 0) ftemp.T1 += data.tcoords.size() + 1;
						if (ftemp.T2 < 0) ftemp.T2 += data.tcoords.size() + 1;
						if (ftemp.T3 < 0) ftemp.T3 += data.tcoords.size() + 1;

						//Checking for positive out of bound
						if (ftemp.N1 > (long)data.normals.size() || ftemp.N2 > (long)data.normals.size() || ftemp.N3 > (long)data.normals.size()
							|| ftemp.V1 > (long)data.vertices.size() || ftemp.V2 > (long)data.vertices.size() || ftemp.V3 > (long)data.vertices.size()
							|| ftemp.T1 > (long)data.tcoords.size() || ftemp.T2 > (long)data.tcoords.size() || ftemp.T3 > (long)data.tcoords.size())
							throw InvalidFormatException(("Out of range index in face. Line: " + std::to_string(line_num)).c_str());
						//Checking for negative out of bound
						if (ftemp.N1 < 0 || ftemp.N2 < 0 || ftemp.N3 < 0
							|| ftemp.V1 < 1 || ftemp.V2 < 1 || ftemp.V3 < 1
							|| ftemp.T1 < 0 || ftemp.T2 < 0 || ftemp.T3 < 0)
							throw InvalidFormatException(("Out of range index in face. Line: " + std::to_string(line_num)).c_str());
					}
					data.faces.push_back(ftemp);
					break;
					//Anything else is ignored, like comments or invalid lines with different starting characters from v or f.
				default:
					break;
				}
			}
			objfile.close();
		}
		//Invalid argument or out of range exception from stof or stol meaning invalid formatting
		catch (const std::invalid_argument&)
		{
			throw(InvalidFormatException(("Bad number formatting. Line: " + std::to_string(line_num)).c_str()));
		}
		catch (const std::out_of_range&)
		{
			throw(InvalidFormatException(("Bad number formatting. Line: " + std::to_string(line_num)).c_str()));
		}
	}
	//If negative fixing is turned on
	if (fix_neg) fix_negative_indexes(data);
	return data;
}

/*
Swaps negative indexes to positve ones.
*/
void FormatConverter::CheckedObjLoader::fix_negative_indexes(D3Data & data)
{
	for (size_t i = 0; i < data.faces.size(); i++)
	{
		if (data.faces[i].V1 < 0) data.faces[i].V1 += data.vertices.size() + 1;
		if (data.faces[i].V2 < 0) data.faces[i].V2 += data.vertices.size() + 1;
		if (data.faces[i].V3 < 0) data.faces[i].V3 += data.vertices.size() + 1;

		if (data.faces[i].N1 < 0) data.faces[i].N1 += data.normals.size() + 1;
		if (data.faces[i].N2 < 0) data.faces[i].N2 += data.normals.size() + 1;
		if (data.faces[i].N3 < 0) data.faces[i].N3 += data.normals.size() + 1;

		if (data.faces[i].T1 < 0) data.faces[i].T1 += data.tcoords.size() + 1;
		if (data.faces[i].T2 < 0) data.faces[i].T2 += data.tcoords.size() + 1;
		if (data.faces[i].T3 < 0) data.faces[i].T3 += data.tcoords.size() + 1;
	}
}

//Triangulates polygon and saves it into given data object
//Only works correctly if the given string does not contain the f and space character at the start
void FormatConverter::CheckedObjLoader::triangulate_polygon(D3Data & data, std::string & line)
{
	//Getting tokenized data
	auto vdata = tokenize_obj_face_line(line);

	//Simple face generation from first vertex
	Face tempf;
	tempf.V1 = vdata[0].vertex_index;
	tempf.N1 = vdata[0].normal_index;
	tempf.T1 = vdata[0].tcoord_index;
	for (size_t i = 2; i < vdata.size(); i++)
	{
		tempf.V2 = vdata[i - 1].vertex_index;
		tempf.N2 = vdata[i - 1].normal_index;
		tempf.T2 = vdata[i - 1].tcoord_index;

		tempf.V3 = vdata[i].vertex_index;
		tempf.N3 = vdata[i].normal_index;
		tempf.T3 = vdata[i].tcoord_index;

		data.faces.push_back(tempf);
	}

}

//Tokenizes given obj face line string
//Only works with data chunks in string, the f and space have to be removed from the front
std::vector<VertexData> FormatConverter::CheckedObjLoader::tokenize_obj_face_line(std::string & line)
{
	//Splitting by spaces
	std::vector<std::string> chunks;
	size_t pos = 0;
	size_t nextpos = line.find(' ');
	while (nextpos != std::string::npos)
	{
		chunks.push_back(line.substr(pos, nextpos - pos));
		pos = nextpos + 1;
		nextpos = line.find(' ', pos);
	}
	//Adding the last part
	chunks.push_back(line.substr(pos, line.size() - pos));

	//Parsing into vertexData
	std::vector<VertexData> vdata;
	VertexData vd;
	pos = 0;
	size_t sz;
	for (size_t i = 0; i < chunks.size(); i++)
	{
		pos = 0;
		vd.vertex_index = stol(chunks[i], &sz);
		pos += sz;
		//End of string
		//Only vertex index case
		if (pos >= chunks[i].size())
		{
			vd.normal_index = 0;
			vd.tcoord_index = 0;
		}
		//Multiple data case
		else if (chunks[i][pos] == '/')
		{
			//Normals given only case
			if (chunks[i][pos + 1] == '/')
			{
				vd.normal_index = stol(chunks[i].substr(pos + 2));
				vd.tcoord_index = 0;
			}
			//Tcoords given
			else
			{
				vd.tcoord_index = stol(chunks[i].substr(pos + 1), &sz);
				pos += sz;
				//Normal given too
				if (chunks[i][pos] == '/')
				{
					vd.normal_index = stol(chunks[i].substr(pos + 1));
				}
				//Normal not given
				else
				{
					vd.normal_index = 0;
				}
			}
		}
		vdata.push_back(vd);
	}
	return vdata;
}
