//
// Common.h
// 共通で使用するマクロや定数の定義
//

#ifndef _Common_h_
#define _Common_h_


// 文字列を大文字へ
#include <string>
std::string toUpper( const std::string &src );

// ファイルフルパスからファイル名
std::string fullPath2FileName( const std::string &src );

// wstringをstringへ変換
std::string WStringToString( std::wstring oWString );


#endif // _Common_h_