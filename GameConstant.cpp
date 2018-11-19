#include "GameConstant.h"


std::map< std::string, float> imas::g_bpm;
std::map< std::string, float> imas::g_scriptDelay;
std::map< imas::CHARACTER, int > imas::g_chara2track;
std::map< imas::CHARACTER, int > imas::g_chara2mpkID;
std::map< imas::CHARACTER, std::wstring > imas::g_charaName;
std::map< std::string, std::wstring> imas::g_songName;

void imas::InitializeGameConstant()
{
	// scriptDelay is manually tuned. Not checked completely.

	imas::g_bpm["age"] = 178.0f;	imas::g_scriptDelay["age"] = 16.0f;
	imas::g_bpm["aoi"] = 106.0f;	imas::g_scriptDelay["aoi"] = 16.0f;
	imas::g_bpm["col"] = 159.0f;	imas::g_scriptDelay["col"] =  9.0f;
	imas::g_bpm["dod"] = 171.0f;	imas::g_scriptDelay["dod"] = 14.0f;
	imas::g_bpm["fir"] = 147.0f;	imas::g_scriptDelay["fir"] = 12.0f;
	imas::g_bpm["ful"] = 127.0f;	imas::g_scriptDelay["ful"] =  8.0f;
	imas::g_bpm["fut"] = 117.0f;	imas::g_scriptDelay["fut"] = 14.0f;
	imas::g_bpm["gmw"] = 180.0f;	imas::g_scriptDelay["gmw"] = 12.0f;
	imas::g_bpm["hwg"] = 149.0f;	imas::g_scriptDelay["hwg"] =  8.0f;
	imas::g_bpm["i00"] = 140.0f;	imas::g_scriptDelay["i00"] = 13.0f;
	imas::g_bpm["ipa"] = 161.0f;	imas::g_scriptDelay["ipa"] = 10.0f;
	imas::g_bpm["iwt"] = 178.0f;	imas::g_scriptDelay["iwt"] = 16.0f;
	imas::g_bpm["kam"] = 148.0f;	imas::g_scriptDelay["kam"] = 12.0f;
	imas::g_bpm["kos"] = 132.0f;	imas::g_scriptDelay["kos"] =  6.0f;
	imas::g_bpm["kir"] = 180.0f;	imas::g_scriptDelay["kir"] = 16.0f;
	imas::g_bpm["mah"] = 150.0f;	imas::g_scriptDelay["mah"] = 12.0f;
	imas::g_bpm["mas"] = 122.0f;	imas::g_scriptDelay["mas"] =  8.0f;
	imas::g_bpm["mbf"] = 157.0f;	imas::g_scriptDelay["mbf"] = 12.0f;
	imas::g_bpm["meg"] = 145.0f;	imas::g_scriptDelay["meg"] =  8.0f;
	imas::g_bpm["mei"] = 152.0f;	imas::g_scriptDelay["mei"] =  8.0f;
	imas::g_bpm["mys"] = 104.0f;	imas::g_scriptDelay["mys"] = 16.0f;
	imas::g_bpm["oha"] = 152.0f;	imas::g_scriptDelay["oha"] = 12.0f;
	imas::g_bpm["omo"] = 133.0f;	imas::g_scriptDelay["omo"] =  8.0f;
	imas::g_bpm["pm9"] = 67.5f;		imas::g_scriptDelay["pm9"] = 12.0f;
	imas::g_bpm["pos"] = 138.0f;	imas::g_scriptDelay["pos"] = 12.0f;
	imas::g_bpm["rel"] = 142.0f;	imas::g_scriptDelay["rel"] =  8.0f;
	imas::g_bpm["shi"] = 170.0f;	imas::g_scriptDelay["shi"] = 16.0f;
	imas::g_bpm["str"] = 165.0f;	imas::g_scriptDelay["str"] = 12.0f;
	imas::g_bpm["tai"] = 128.0f;	imas::g_scriptDelay["tai"] =  8.0f;
	imas::g_bpm["tim"] = 165.0f;	imas::g_scriptDelay["tim"] =  8.0f;
	imas::g_bpm["tnr"] = 88.0f;		imas::g_scriptDelay["tnr"] =  8.0f;
	imas::g_bpm["wat"] = 168.0f;	imas::g_scriptDelay["wat"] = 16.0f;

	// track number in AIX
	// 00�t���A01��悢�A02�������A03�瑁�A04�ɐD�A05���q�A06����A07�^�A08�����A09����A10���t(��)�A11���t(�E)
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

	// param number of face command in MPK
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

	imas::g_charaName[ imas::HARUKA ] = L"�V�C�t��";
	imas::g_charaName[ imas::CHIHAYA] = L"�@���瑁";
	imas::g_charaName[ imas::YUKIHO ] = L"�������";
	imas::g_charaName[ imas::YAYOI  ] = L"���΂�悢";
	imas::g_charaName[ imas::RITSUKO] = L"�H�����q";
	imas::g_charaName[ imas::AZUSA  ] = L"�O�Y������";
	imas::g_charaName[ imas::IORI   ] = L"�����ɐD";
	imas::g_charaName[ imas::MAKOTO ] = L"�e�n�^";
	imas::g_charaName[ imas::AMI    ] = L"�o�C����";
	imas::g_charaName[ imas::MAMI   ] = L"�o�C�^��";
	imas::g_charaName[ imas::MIKI   ] = L"�������";
	imas::g_charaName[ imas::MIKI_SHORT] = L"�������";

	imas::g_songName["age"] = L"�G�[�W�F���g�������";
	imas::g_songName["aoi"] = L"������";
	imas::g_songName["col"] = L"Colorful Days";
	imas::g_songName["dod"] = L"Do-Dai";
	imas::g_songName["fir"] = L"First Stage";
	imas::g_songName["ful"] = L"�ӂ�ӂ�t���[�`���[��";
	imas::g_songName["fut"] = L"�t�^���̋L��";
	imas::g_songName["gmw"] = L"GO MY WAY!!";
	imas::g_songName["hwg"] = L"Here we go!!";
	imas::g_songName["i00"] = L"i";
	imas::g_songName["ipa"] = L"�����ς������ς�";
	imas::g_songName["iwt"] = L"I Want";
	imas::g_songName["kam"] = L"�_���܂�Birthday";
	imas::g_songName["kos"] = L"Kosmos, Cosmos";
	imas::g_songName["kir"] = L"�L�����L����";
	imas::g_songName["mah"] = L"���@��������!";
	imas::g_songName["mas"] = L"�܂�����";
	imas::g_songName["mbf"] = L"My Best Friend";
	imas::g_songName["meg"] = L"�ڂ������u��";	
	imas::g_songName["mei"] = L"����Mind";
	imas::g_songName["mys"] = L"my song";
	imas::g_songName["oha"] = L"���͂悤!! ������";
	imas::g_songName["omo"] = L"�v���o�����肪�Ƃ�";
	imas::g_songName["pm9"] = L"9:02pm";
	imas::g_songName["pos"] = L"�|�W�e�B�u!";
	imas::g_songName["rel"] = L"relations";	
	imas::g_songName["shi"] = L"shiny smile";
	imas::g_songName["str"] = L"�X�^���g�X�^��";
	imas::g_songName["tai"] = L"���z�̃W�F���V�[";
	imas::g_songName["tim"] = L"THE IDOLM@STER";
	imas::g_songName["tnr"] = L"�ׂɁc";
	imas::g_songName["wat"] = L"���̓A�C�h��";

}