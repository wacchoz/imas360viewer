//
// Common.h
// ���ʂŎg�p����}�N����萔�̒�`
//

#ifndef _Common_h_
#define _Common_h_


// �������啶����
#include <string>
std::string toUpper( const std::string &src );

// �t�@�C���t���p�X����t�@�C����
std::string fullPath2FileName( const std::string &src );

// wstring��string�֕ϊ�
std::string WStringToString( std::wstring oWString );


#endif // _Common_h_