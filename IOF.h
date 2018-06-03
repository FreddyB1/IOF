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


#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <type_traits>

#define DECLARE_ENUM_BITFLAGS_OPERS(FL_TYPE)\
	constexpr FL_TYPE operator|(FL_TYPE lhs, FL_TYPE rhs) noexcept\
	{\
		static_assert(std::is_enum_v<FL_TYPE>, "DECLARE_ENUM_BITFLAGS_OPERS: type specified is not an enum.");\
		using uType = std::underlying_type_t<FL_TYPE>;\
		return static_cast<FL_TYPE>(static_cast<uType>(lhs) | static_cast<uType>(rhs));\
	}\
	constexpr bool operator&(FL_TYPE lhs, FL_TYPE rhs) noexcept\
	{\
		static_assert(std::is_enum_v<FL_TYPE>, "DECLARE_ENUM_BITFLAGS_OPERS: type specified is not an enum.");\
		using uType = std::underlying_type_t<FL_TYPE>;\
		return static_cast<bool>(static_cast<uType>(lhs) & static_cast<uType>(rhs));\
	}


class IOF
{
public:

	class Exception : public std::exception
	{
	public:
		explicit Exception(const std::string& msg) : text(msg) {}
		explicit Exception(std::string&& msg) : text(std::move(msg)) {}

		const char* what() const noexcept override
		{
			return text.c_str();
		}

	private:
		std::string text;
	};


	enum class Flags
	{
		in = 1,
		out = 2,
		bin = 4,
		ate = 8,
		app = 16,
		big_endian = 32
	};

	enum class Reference
	{
		beg = 0,
		cur,
		end
	};

	IOF(const std::string& filename, Flags flags);

	~IOF()
	{
		fst.close();
	}

	//Reading
	template<class T>
	std::enable_if_t<std::is_integral_v<T>, T> read()
	{
		T value{0};
		constexpr size_t size = sizeof(T);
		const auto id = static_cast<int>(endian);
		//handles little_endian and big_endian formats
		for(size_t i = 0; i < size; i++)
		{
			value |= (static_cast<T>(fst.get()) << (id * ((8 * (i + 1) - ((id == 1) ? (8) : (8 * size))))));
		}
		return value;
	}

	template<class T>
	std::enable_if_t<std::is_floating_point_v<T> && sizeof(T) == 4 && sizeof(unsigned long int) == 4, float> read()
	{
		unsigned long int fbinary_repr = read<unsigned long int>();
		return *reinterpret_cast<float*>(&fbinary_repr);
	}

	template<class T>
	std::enable_if_t<std::is_floating_point_v<T> && sizeof(T) == 8 && sizeof(unsigned long long int) == 8, double> read()
	{
		unsigned long long int fbinary_repr = read<unsigned long long int>();
		return *reinterpret_cast<T*>(&fbinary_repr);
	}


	//Writing

	template <class T>
	std::enable_if_t<std::is_integral_v<T>, void> write(T value)
	{
		switch(endian)
		{
		case Endianness::big_endian:
		{
			size_t j = sizeof(T);
			for(size_t i = 0, k = j - 1; i < j; i++)
			{
				fst.put((((value >> (8 * (k - i)) & 0xFF))));
			}
		}
		break;
		case Endianness::little_endian:
		{
			size_t j = sizeof(T);
			for(size_t i = 0; i < j; i++)
			{
				fst.put(((value >> (8 * i)) & 0xFF));
			}
		}
		break;
		}
	}
	template <class T>
	std::enable_if_t<std::is_floating_point_v<T> && sizeof(T) == 4 && sizeof(unsigned long int) == 4, void> write(T value) const
	{
		unsigned long int fbinary_repr = *reinterpret_cast<unsigned long int*>(&value);
		write(fbinary_repr);
	}

	template <class T>
	std::enable_if_t<std::is_floating_point_v<T> && sizeof(T) == 8 && sizeof(unsigned long long int) == 8, void> write(T value) const
	{
		unsigned long long int fbinary_repr = *reinterpret_cast<unsigned long long int*>(&value);
		write(fbinary_repr);
	}

	inline void read(char* src, size_t maxsize)
	{
		fst.getline(src, maxsize);
	}

	inline void write(char *src, size_t maxsize)
	{
		fst.write(src, maxsize);
	}

	inline auto tell()
	{
		return fst.tellg();
	}


	inline void seek(size_t pos, Reference ref)
	{
		switch(ref)
		{
		case Reference::beg: fst.seekg(pos, fst.beg); break;
		case Reference::cur: fst.seekg(pos, fst.cur); break;
		case Reference::end: fst.seekg(pos, fst.end); break;
		}
	}

	inline void flush()
	{
		fst.flush();
	}

	inline void write(const std::string& line)
	{
		fst.write(line.c_str(), line.length());
	}

	inline bool is_fail()
	{
		return fst.fail();
	}

	inline bool is_eof()
	{
		return fst.eof();
	}

	inline bool is_good()
	{
		return fst.good();
	}

	inline bool is_bad()
	{
		return fst.bad();
	}
private:

	enum class Endianness
	{
		little_endian = 1,
		big_endian = -1
	};
	std::fstream fst;
	Endianness endian;
};

