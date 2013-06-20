/**
 * @file
 * 
 * @author  Sina Hatef Matbue ( _null_ ) <sinahatef.cpp@gmail.com>
 *
 * @section License
 * This file is part of GraVitoN.
 *
 * Graviton is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Graviton is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Graviton.  If not, see http://www.gnu.org/licenses/.
 *
 * @brief GraVitoN Config File
 *
*/

#ifndef GRAVITON_CONFIG_H
#define GRAVITON_CONFIG_H

//#define GVN_ACTIVATE_LUABRIDGE

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
/// My Lovely Define!
#define _null_ 0x00 //NULL

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
// COMPILER OPTIONS
#ifdef INFO_COMPILER_GCC
    #pragma GCC diagnostic ignored "-Wdeprecated"
    #pragma GCC diagnostic ignored "-Wcpp"
    #pragma GCC diagnostic ignored "-Wignored-qualifiers"
    #pragma GCC diagnostic ignored "-Wnarrowing"
#endif

#ifdef INFO_COMPILER_MSVC
    #pragma warning( disable : 4996)
#endif

#ifdef INFO_OS_WINDOWS
	/// YOU DO NOT NEED TO WRITE #include<windows.h> INSIDE YOUR SOURCE CODE
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

#include <string>

namespace GraVitoN
{
    typedef std::string         gstring;
    typedef char                gchar;
    typedef unsigned char       guchar;
    
	namespace Config
	{
		const unsigned int MAX_TCP_PACKET_SIZE = 4069;
		const unsigned long MAX_FILE_SIZE = 32 * 1024 * 1024 * sizeof(unsigned char); // 64 MB
	}
}

#endif
