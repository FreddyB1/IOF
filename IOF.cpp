/*
MIT License

Copyright (c) 2018 Freddy Borja

	** Simple binary reader and writer library for C++ **

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "IOF.h"

DECLARE_ENUM_BITFLAGS_OPERS(IOF::Flags);


IOF::IOF(const std::string& filename, Flags flags) : endian{Endianness::little_endian}
{
	std::ios_base::openmode mode{0};
	if(flags & Flags::bin)
		mode |= std::fstream::binary;
	if(flags & Flags::app)
		mode |= std::fstream::app;
	if(flags & Flags::ate)
		mode |= std::fstream::ate;
	if(flags & Flags::in)
		mode |= std::fstream::in;
	if(flags & Flags::out)
		mode |= std::fstream::out;
	if(flags & Flags::big_endian)
		endian = Endianness::big_endian;

	fst.open(filename, mode);
	if(fst.fail())
	{
		std::string str = "[IOF]: error -> could not open file: " + filename;
		throw Exception(str);
	}
}
