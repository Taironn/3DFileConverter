---


---

<h1 id="dfileconverter">3DFileConverter</h1>
<p>This is a general 3d file converter written in C++. It defines an inner structure for 3d data, and abstact classes for file loaders and writers. This version contains a wavefront obj loader and a binary stl writer.</p>
<h2 id="data-representation">Data representation</h2>
<p>For a loaded mesh a D3Data object is used. The data is stored as in wavefront objects. There are separate vectors for vertices, normals, texture coordinates and faces. The faces contain indexes for their data. There is a cached face vector to save time on derefering the indexes, and making usage easier. These cached faces store pointers to the data structures for the face.<br>
The representation is written only for triangles, it could be easily extended for polygons however.</p>
<h2 id="reading-files">Reading files</h2>
<p>Supported formats (for loading) have their own loader classes. They create a D3Data object and store the data from the given files on usage. They all inherit from Loader class, and all have a <em>load</em> function, which only needs a path parameter. The <em>load</em> function gives back the generated D3Data object.</p>
<h2 id="writing-files">Writing files</h2>
<p>Supported formats (for writing) also have a class for outputting data. They work from a D3Data object and write the data according to their file format to a file. They all inherit from Writer class, and all have a <em>write</em> function, which needs a path parameter and a D3Data object paramter to write from.</p>
<h2 id="utilities">Utilities</h2>
<p>There are some extra functionalities implemented on D3Data format. These are in static functions in the Utilities class. For example volume and surface of the given D3Data object can be calculated with these.</p>
<h1 id="usage">Usage</h1>
<p>The correct loader has to be instantiated, and its load function has to be called with the path of the file that is needed to be parsed. The function gives back the D3Data object. Needed metrics can be calculated on the object using the static functions of the Utilities class.<br>
The data can be then written out to a file using a Writer class. The class for the needed format has to be instantiated, then the file can be written out by calling its write funtion and passing the path, and the D3Data object that is needed to be written out.</p>
<h1 id="implemented-formats">Implemented formats</h1>
<h2 id="checkedobjloader---loading-wavefron-obj-files">CheckedObjLoader - Loading wavefron obj files</h2>
<p>Loads a default wavefront object file. It only support v, vt, vn and f lines. During parsing if the syntax is not correct throws an InvalidFormatException. However if the file can be parsed by throwing away lines that does not start with a specified basic character from the obj specification, it parses the file. It supports basic triangularization for polygons, but only for convex polygons. If the model includes concave polygons the triangulariazation has to be turned off, using the parameter in the load function.</p>
<h2 id="stilwriter---writing-out-binary-stl-files">StilWriter - Writing out binary stl files</h2>
<p>Writes out a binary stl file from a D3Data object. According to the specification, the number of triangles after the header will be written in little endian. The rest of the file uses the default endianity. Normals are written out in normalized form.</p>
<h1 id="extending-for-other-formats">Extending for other formats</h1>
<p>For each new format a new class should be written for either loading or writing. They shoud inherit from the abstract base classes respectively and the default load/write function should be accessible through the virtual function from the base class.<br>
Other functionailites can be added to the Utilities class that use the D3Data format.</p>
<h1 id="example">Example</h1>
<p>An example can be found in the Main.cpp file. It reades a basic cube obj file and writes it out into a mycube stl file. After loading it writes out the surface and volume of the mesh.</p>

