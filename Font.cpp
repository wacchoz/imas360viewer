#include "Font.h"


BitmapFont::BitmapFont()
{
	m_listBaseA = 0;
	m_listBaseU = 0;
	m_FontA = NULL;
	m_FontU = NULL;
}


BitmapFont::~BitmapFont()
{
	if ( m_listBaseA != 0 )
	{
		glDeleteLists(m_listBaseA, 96);
		m_listBaseA = 0;
	}
	if ( m_listBaseU != 0 )
	{
		glDeleteLists(m_listBaseU, m_StrLength);
		m_listBaseU = 0;
		m_StrLength = 0;
	}
	if ( m_FontA )
	{
		DeleteObject( m_FontA );
		m_FontA = NULL;
	}
	if ( m_FontU )
	{
		DeleteObject( m_FontU );
		m_FontU = NULL;
	}
}


bool BitmapFont::CreateA(char *fontname, int size)
{
	HINSTANCE hInst = NULL;
	m_FontA = CreateFontA(
		size,						//フォント高さ
        0,							//文字幅
        0,							//テキストの角度
        0,							//ベースラインとｘ軸との角度
        FW_REGULAR,					//フォントの重さ（太さ）
        FALSE,						//イタリック体
        FALSE,						//アンダーライン
        FALSE,						//打ち消し線
        ANSI_CHARSET,				//文字セット
        OUT_DEFAULT_PRECIS,			//出力精度
        CLIP_DEFAULT_PRECIS,		//クリッピング精度
        ANTIALIASED_QUALITY,		//出力品質
        FIXED_PITCH | FF_MODERN,	//ピッチとファミリー
        fontname);					//書体名

	if ( m_FontA == NULL )
		return false;

	m_hDC = wglGetCurrentDC();
	SelectObject(m_hDC, m_FontA);

	m_listBaseA = glGenLists(96);
	wglUseFontBitmapsA(m_hDC, 32, 96, m_listBaseA);
	
	return true;
}


bool BitmapFont::CreateW(wchar_t *fontname, int size)
{
	m_FontU = CreateFontW(
		size,						//フォント高さ
        0,							//文字幅
        0,							//テキストの角度
        0,							//ベースラインとｘ軸との角度
        FW_REGULAR,					//フォントの重さ（太さ）
        FALSE,						//イタリック体
        FALSE,						//アンダーライン
        FALSE,						//打ち消し線
        SHIFTJIS_CHARSET,			//文字セット
        OUT_DEFAULT_PRECIS,			//出力精度
        CLIP_DEFAULT_PRECIS,		//クリッピング精度
        ANTIALIASED_QUALITY,		//出力品質
        FIXED_PITCH | FF_MODERN,	//ピッチとファミリー
        fontname);					//書体名

	if ( m_FontU == NULL )
		return false;

	m_hDC = wglGetCurrentDC();
	SelectObject(m_hDC, m_FontU);

	return true;
}


void BitmapFont::ReleaseA()
{
	if ( m_listBaseA != 0 )
	{
		glDeleteLists(m_listBaseA, 96);
		m_listBaseA = 0;
	}
	if ( m_FontA )
	{
		DeleteObject( m_FontA );
		m_FontA = NULL;
	}
}


void BitmapFont::ReleaseW()
{
	if ( m_listBaseU != 0 )
	{
		glDeleteLists(m_listBaseU, m_StrLength);
		m_listBaseU = 0;
		m_StrLength = 0;
	}
	if ( m_FontU )
	{
		DeleteObject( m_FontU );
		m_FontU = NULL;
	}
}


void BitmapFont::DrawStringA(char *format, ...)
{
	char buf[FONT_BUFFER_SIZE];
	va_list ap;

	//ポインタがNULLの場合は終了
	if ( format == NULL )
		return;

	//文字列変換
	va_start(ap, format);
	vsprintf_s(buf, format, ap);
	va_end(ap);

	//ディスプレイリストで描画
	glPushAttrib(GL_LIST_BIT);
	glListBase(m_listBaseA - 32);
	glCallLists(strlen(buf), GL_UNSIGNED_BYTE, buf);
	glPopAttrib();
}


void BitmapFont::DrawStringW(wchar_t *format, ...)
{
	wchar_t buf[FONT_BUFFER_SIZE];
	va_list ap;
	BOOL result = FALSE;
	
	//ポインタがNULLの場合は終了
	if ( format == NULL )
		return;

	//文字列変換
	va_start(ap, format);
	vswprintf_s(buf, format, ap);
	va_end(ap);

	//以前のディスプレイリストがある場合は削除
	if ( m_listBaseU != 0 )
	{
		glDeleteLists(m_listBaseU, m_StrLength);
		m_listBaseU = 0;
		m_StrLength = 0;
	}

	//ディスプレイリスト作成
	m_StrLength = wcslen(buf);
	m_listBaseU = glGenLists(m_StrLength);
	SelectObject(m_hDC, m_FontU);

	for( int i=0; i<m_StrLength; i++ )
	{
		//ディスプレイリストに格納
		result = wglUseFontBitmapsW(m_hDC, buf[i], 1, m_listBaseU + (DWORD)i);

		//格納失敗時
		if ( !result )
		{
			glDeleteLists(m_listBaseU, m_StrLength);
			m_listBaseU = 0;
			m_StrLength = 0;
			return;
		}
	}

	//ディスプレイリストで描画
	for( int i=0; i<m_StrLength; i++ )
	{
		glCallList(m_listBaseU + i);
	}

	//ディスプレイリスト破棄
	glDeleteLists(m_listBaseU, m_StrLength);
	m_listBaseU = 0;
	m_StrLength = 0;
}

