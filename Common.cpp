#include "Common.h"
#include <windows.h>

// ï∂éöóÒÇëÂï∂éöÇ÷
std::string toUpper( const std::string &src )
{
	std::string result;
	result = src;

    int i;
	for( i = 0; src[i] != '\0'; i ++)
	{
        result[i] = toupper( src[i] );
    }
    return result;
}

std::string fullPath2FileName( const std::string &src )
{
	std::string::size_type pos = src.find_last_of( '\\' );

	if( pos == std::string::npos )
		return src;
	else
		return std::string( src, pos+1, src.length() - pos -1 );
}

std::string WStringToString( std::wstring oWString )
{
	// wstring Å® SJIS
	int iBufferSize = WideCharToMultiByte( CP_OEMCP, 0, oWString.c_str() , -1, (char*)NULL, 0, NULL, NULL );

	CHAR* cpMultiByte = new CHAR[ iBufferSize ];
	// wstring Å® SJIS
	WideCharToMultiByte( CP_OEMCP, 0, oWString.c_str(), -1, cpMultiByte, iBufferSize, NULL, NULL );
	// stringÇÃê∂ê¨
	std::string oRet( cpMultiByte, cpMultiByte + iBufferSize - 1 );

	delete [] cpMultiByte;

	return( oRet );
}
