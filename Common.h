#ifndef _Common_h_
#define _Common_h_


// convert string to capital
#include <string>
std::string ToUpper( const std::string &src );

// extract filename from full path
std::string fullPath2FileName( const std::string &src );

// convert wstring to string
std::string WStringToString( std::wstring oWString );


#endif