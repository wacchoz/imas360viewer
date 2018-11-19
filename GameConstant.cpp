#include "GameConstant.h"


std::map< std::string, float> imas::g_bpm;
std::map< std::string, float> imas::g_scriptDelay;
std::map< imas::CHARACTER, int > imas::g_chara2track;
std::map< imas::CHARACTER, int > imas::g_chara2mpkID;


void imas::InitializeGameConstant()
{
	// scriptDelayは手調整。全部は検証できていません。

	imas::g_bpm["age"] = 178.0f;	imas::g_scriptDelay["age"] = 16.0f;
	imas::g_bpm["aoi"] = 106.0f;	imas::g_scriptDelay["aoi"] = 16.0f;
	imas::g_bpm["col"] = 159.0f;	imas::g_scriptDelay["col"] =  9.0f;
	imas::g_bpm["dod"] = 171.0f;	imas::g_scriptDelay["dod"] = 16.0f;
	imas::g_bpm["fir"] = 147.0f;	imas::g_scriptDelay["fir"] = 12.0f;
	imas::g_bpm["ful"] = 127.0f;	imas::g_scriptDelay["ful"] =  8.0f;
	imas::g_bpm["fut"] = 117.0f;	imas::g_scriptDelay["fut"] = 14.0f;
	imas::g_bpm["gmw"] = 180.0f;	imas::g_scriptDelay["gmw"] = 12.0f;
	imas::g_bpm["hwg"] = 149.0f;	imas::g_scriptDelay["hwg"] =  8.0f;
	imas::g_bpm["i00"] = 140.0f;	imas::g_scriptDelay["i00"] = 13.0f;
	imas::g_bpm["ipa"] = 161.0f;	imas::g_scriptDelay["ipa"] = 10.0f;
	imas::g_bpm["kam"] = 148.0f;	imas::g_scriptDelay["kam"] = 12.0f;
	imas::g_bpm["mah"] = 150.0f;	imas::g_scriptDelay["mah"] = 12.0f;
	imas::g_bpm["mas"] = 122.0f;	imas::g_scriptDelay["mas"] =  8.0f;
	imas::g_bpm["mbf"] = 157.0f;	imas::g_scriptDelay["mbf"] = 12.0f;
	imas::g_bpm["mys"] = 104.0f;	imas::g_scriptDelay["mys"] = 16.0f;
	imas::g_bpm["oha"] = 152.0f;	imas::g_scriptDelay["oha"] = 12.0f;
	imas::g_bpm["omo"] = 133.0f;	imas::g_scriptDelay["omo"] =  8.0f;
	imas::g_bpm["pm9"] = 67.5f;		imas::g_scriptDelay["pm9"] = 12.0f;
	imas::g_bpm["pos"] = 138.0f;	imas::g_scriptDelay["pos"] = 12.0f;
	imas::g_bpm["rel"] = 142.0f;	imas::g_scriptDelay["rel"] =  8.0f;
	imas::g_bpm["shi"] = 170.0f;	imas::g_scriptDelay["shi"] = 16.0f;
	imas::g_bpm["tai"] = 128.0f;	imas::g_scriptDelay["tai"] =  8.0f;
	imas::g_bpm["tim"] = 165.0f;	imas::g_scriptDelay["tim"] =  8.0f;
	imas::g_bpm["wat"] = 168.0f;	imas::g_scriptDelay["wat"] = 16.0f;
	imas::g_bpm["tnr"] = 88.0f;		imas::g_scriptDelay["tnr"] =  8.0f;
	imas::g_bpm["kos"] = 132.0f;	imas::g_scriptDelay["kos"] =  8.0f;
	imas::g_bpm["str"] = 165.0f;	imas::g_scriptDelay["str"] = 12.0f;
	imas::g_bpm["mei"] = 152.0f;	imas::g_scriptDelay["mei"] =  8.0f;
	imas::g_bpm["kir"] = 180.0f;	imas::g_scriptDelay["kir"] = 16.0f;
	imas::g_bpm["meg"] = 145.0f;	imas::g_scriptDelay["meg"] =  8.0f;
	imas::g_bpm["iwt"] = 178.0f;	imas::g_scriptDelay["iwt"] = 16.0f;

	// AIXのトラック番号
	// 00春香、01やよい、02あずさ、03千早、04伊織、05律子、06雪歩、07真、08亜美、09美希、10伴奏(左)、11伴奏(右)
	imas::g_chara2track[ imas::HARUKA ] = 0;
	imas::g_chara2track[ imas::YAYOI  ] = 1;
	imas::g_chara2track[ imas::AZUSA  ] = 2;
	imas::g_chara2track[ imas::CHIHAYA] = 3;
	imas::g_chara2track[ imas::IORI   ] = 4;
	imas::g_chara2track[ imas::RITSUKO] = 5;
	imas::g_chara2track[ imas::YUKIHO ] = 6;
	imas::g_chara2track[ imas::MAKOTO ] = 7;
	imas::g_chara2track[ imas::AMI    ] = 8;
	imas::g_chara2track[ imas::MAMI   ] = 8;
	imas::g_chara2track[ imas::MIKI   ] = 9;
	imas::g_chara2track[ imas::MIKI_SHORT] = 9;

	// MPK内のfaceのparam番号とキャラの対応
	imas::g_chara2mpkID[ imas::HARUKA ] = 0;
	imas::g_chara2mpkID[ imas::CHIHAYA] = 1;
	imas::g_chara2mpkID[ imas::YUKIHO ] = 2;
	imas::g_chara2mpkID[ imas::YAYOI  ] = 3;
	imas::g_chara2mpkID[ imas::RITSUKO] = 4;
	imas::g_chara2mpkID[ imas::AZUSA  ] = 5;
	imas::g_chara2mpkID[ imas::IORI   ] = 6;
	imas::g_chara2mpkID[ imas::MAKOTO ] = 7;
	imas::g_chara2mpkID[ imas::AMI    ] = 8;
	imas::g_chara2mpkID[ imas::MAMI   ] = 9;
	imas::g_chara2mpkID[ imas::MIKI   ] = 10;
	imas::g_chara2mpkID[ imas::MIKI_SHORT] = 11;
}