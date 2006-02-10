//////////////////////////////////////////////////////////////////////////////
// string.hpp
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_STRING_HPP_INCLUDED
#define SQLITEPP_STRING_HPP_INCLUDED

#include <string>
#include <locale>

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

namespace meta {

//////////////////////////////////////////////////////////////////////////////

// Meta if
template<bool C, typename T1, typename T2>
struct if_ { typedef T1 type; };
template<typename T1, typename T2>
struct if_<false, T1, T2> { typedef T2 type; };

struct utf16_char_selector
{
	class unknown_utf16_char_type;
	typedef if_<sizeof(wchar_t) == 2, wchar_t,
		if_<sizeof(unsigned short) == 2, unsigned short,
		if_<sizeof(unsigned int) == 2, unsigned int, 
			unknown_utf16_char_type>::type>::type>::type type;
};

struct utf32_char_selector
{
	class unknown_utf32_char_type;
	typedef if_<sizeof(wchar_t) == 4, wchar_t,
		if_<sizeof(unsigned short) == 4, unsigned short,
		if_<sizeof(unsigned int) == 4, unsigned int, 
			unknown_utf32_char_type>::type>::type>::type type;
};

//////////////////////////////////////////////////////////////////////////////

} // namespace meta

//////////////////////////////////////////////////////////////////////////////

using std::size_t;

typedef unsigned char                   utf8_char;
typedef meta::utf16_char_selector::type utf16_char;
typedef meta::utf32_char_selector::type utf32_char;

typedef std::basic_string<utf8_char>    utf8_string;
typedef std::basic_string<utf16_char>   utf16_string;
typedef std::basic_string<utf32_char>   utf32_string;

#ifdef SQLITEPP_UTF16
	typedef utf16_char   char_t;
	typedef utf16_string string_t;
#else
	typedef utf8_char    char_t;
	typedef utf8_string  string_t;
#endif // SQLITEPP_UTF16

size_t const npos = (size_t)-1;

utf8_string  utf16_to_utf8(utf16_char const* str, size_t size = npos);
utf8_string  utf32_to_utf8(utf32_char const* str, size_t size = npos);
utf8_string  ansi_to_utf8(char const* str, size_t size = npos, std::locale const& = std::locale());
std::string  utf8_to_ansi(utf8_char const* str, size_t size = npos, std::locale const& = std::locale());

utf16_string utf8_to_utf16(utf8_char const* str, size_t size = npos);
utf16_string utf32_to_utf16(utf32_char const* str, size_t size = npos);
utf16_string ansi_to_utf16(char const* str, size_t size = npos, std::locale const& = std::locale());
std::string  utf16_to_ansi(utf16_char const* str, size_t size = npos, std::locale const& = std::locale());

utf32_string utf8_to_utf32(utf8_char const* str, size_t size = npos);
utf32_string utf16_to_utf32(utf16_char const* str, size_t size = npos);
utf32_string ansi_to_utf32(char const* str, size_t size = npos, std::locale const& = std::locale());
std::string  utf32_to_ansi(utf32_char const* str, size_t size = npos, std::locale const& = std::locale());

//////////////////////////////////////////////////////////////////////////////

namespace aux {

//////////////////////////////////////////////////////////////////////////////

template<typename C, typename T1, typename T2>
struct selector;

template<typename T1, typename T2>
struct selector<utf8_char, T1, T2>
{
	selector(T1 t1, T2) : result(t1) {}
	typedef T1 type;
	type result;
};

template<typename T1, typename T2>
struct selector<utf16_char, T1, T2>
{
	selector(T1, T2 t2) : result(t2) {}
	typedef T2 type;
	type result;
};

template<typename T1, typename T2>
inline typename selector<char_t, T1, T2>::type select(T1 t1, T2 t2)
{
	return selector<char_t, T1, T2>(t1, t2).result;
}

template<typename C>
struct converter;

template<>
struct converter<utf8_char>
{
	static utf8_string utf(utf8_char const* str, size_t)
	{
		return str;
	}
	static utf8_string utf(utf16_char const* str, size_t size)
	{
		return utf16_to_utf8(str, size);
	}
	static utf8_string utf(utf32_char const* str, size_t size)
	{
		return utf32_to_utf8(str, size);
	}
	static utf8_string utf(char const* str, size_t size, std::locale const& loc)
	{
		return ansi_to_utf8(str, size, loc);
	}
	static std::string ansi(utf8_char const* str, size_t size, std::locale const& loc)
	{
		return utf8_to_ansi(str, size, loc);
	}
};

template<>
struct converter<utf16_char>
{
	static utf16_string utf(utf8_char const* str, size_t size)
	{
		return utf8_to_utf16(str, size);
	}
	static utf16_string utf(utf16_char const* str, size_t size)
	{
		return str;
	}
	static utf16_string utf(utf32_char const* str, size_t size)
	{
		return utf32_to_utf16(str, size);
	}
	static utf16_string utf(char const* str, size_t size, std::locale const& loc)
	{
		return ansi_to_utf16(str, size, loc);
	}
	static std::string ansi(utf16_char const* str, size_t size, std::locale const& loc)
	{
		return utf16_to_ansi(str, size, loc);
	}
};

template<>
struct converter<utf32_char>
{
	static utf32_string utf(utf8_char const* str, size_t size)
	{
		return utf8_to_utf32(str, size);
	}
	static utf32_string utf(utf16_char const* str, size_t size)
	{
		return utf16_to_utf32(str, size);
	}
	static utf32_string utf(utf32_char const* str, size_t)
	{
		return str;
	}
	static utf32_string utf(char const* str, size_t size, std::locale const& loc)
	{
		return ansi_to_utf32(str, size, loc);
	}
	static std::string ansi(utf32_char const* str, size_t size, std::locale const& loc)
	{
		return utf32_to_ansi(str, size, loc);
	}
};

//////////////////////////////////////////////////////////////////////////////

} // namespace aux

//////////////////////////////////////////////////////////////////////////////

inline utf8_string utf8(utf16_char const* str, size_t size = npos)
{
	return aux::converter<utf8_char>::utf(str, size);
}

inline utf8_string utf8(utf16_string const& str)
{
	return aux::converter<utf8_char>::utf(str.c_str(), str.size());
}

inline utf8_string utf8(utf32_char const* str, size_t size = npos)
{
	return aux::converter<utf8_char>::utf(str, size);
}

inline utf8_string utf8(utf32_string const& str)
{
	return aux::converter<utf8_char>::utf(str.c_str(), str.size());
}

inline utf8_string utf8(char const* str, size_t size = npos, std::locale const& loc = std::locale())
{
	return aux::converter<utf8_char>::utf(str, size, loc);
}

inline utf8_string utf8(std::string const& str, std::locale const& loc = std::locale())
{
	return aux::converter<utf8_char>::utf(str.c_str(), str.size(), loc);
}

inline utf16_string utf16(utf8_char const* str, size_t size = npos)
{
	return aux::converter<utf16_char>::utf(str, size);
}

inline utf16_string utf16(utf8_string const& str)
{
	return aux::converter<utf16_char>::utf(str.c_str(), str.size());
}

inline utf16_string utf16(utf32_char const* str, size_t size = npos)
{
	return aux::converter<utf16_char>::utf(str, size);
}

inline utf16_string utf16(utf32_string const& str)
{
	return aux::converter<utf16_char>::utf(str.c_str(), str.size());
}

inline utf16_string utf16(char const* str, size_t size = npos, std::locale const& loc = std::locale())
{
	return aux::converter<utf16_char>::utf(str, size, loc);
}

inline utf16_string utf16(std::string const& str, std::locale const& loc = std::locale())
{
	return aux::converter<utf16_char>::utf(str.c_str(), str.size(), loc);
}

inline utf32_string utf32(utf8_char const* str, size_t size = npos)
{
	return aux::converter<utf32_char>::utf(str, size);
}

inline utf32_string utf32(utf8_string const& str)
{
	return aux::converter<utf32_char>::utf(str.c_str(), str.size());
}

inline utf32_string utf32(utf16_char const* str, size_t size = npos)
{
	return aux::converter<utf32_char>::utf(str, size);
}

inline utf32_string utf32(utf16_string const& str)
{
	return aux::converter<utf32_char>::utf(str.c_str(), str.size());
}

inline utf32_string utf32(char const* str, size_t size = npos, std::locale const& loc = std::locale())
{
	return aux::converter<utf32_char>::utf(str, size, loc);
}

inline utf32_string utf32(std::string const& str, std::locale const& loc = std::locale())
{
	return aux::converter<utf32_char>::utf(str.c_str(), str.size(), loc);
}


inline string_t utf(utf8_char const* str, size_t size = npos)
{
	return aux::converter<char_t>::utf(str, size);
}

inline string_t utf(utf8_string const& str)
{
	return aux::converter<char_t>::utf(str.c_str(), str.size());
}

inline string_t utf(utf16_char const* str, size_t size = npos)
{
	return aux::converter<char_t>::utf(str, size);
}

inline string_t utf(utf16_string const& str)
{
	return aux::converter<char_t>::utf(str.c_str(), str.size());
}

inline string_t utf(utf32_char const* str, size_t size = npos)
{
	return aux::converter<char_t>::utf(str, size);
}

inline string_t utf(utf32_string const& str)
{
	return aux::converter<char_t>::utf(str.c_str(), str.size());
}

inline string_t ansi_to_utf(char const* str, size_t size = npos, std::locale const& loc = std::locale())
{
	return aux::converter<char_t>::utf(str, size, loc);
}

inline string_t ansi_to_utf(std::string const& str, std::locale const& loc = std::locale())
{
	return aux::converter<char_t>::utf(str.c_str(), str.size(), loc);
}

inline std::string utf_to_ansi(char_t const* str, size_t size = npos, std::locale const& loc = std::locale())
{
	return aux::converter<char_t>::ansi(str, size, loc);
}

inline std::string utf_to_ansi(string_t const& str, std::locale const& loc = std::locale())
{
	return aux::converter<char_t>::ansi(str.c_str(), str.size(), loc);
}

//////////////////////////////////////////////////////////////////////////////

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_STRING_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
