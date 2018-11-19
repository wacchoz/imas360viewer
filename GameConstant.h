#ifndef _GAME_CONSTANT_H_
#define _GAME_CONSTANT_H_

#include <map>
#include <string>

namespace imas
{

enum CHARACTER { HARUKA, CHIHAYA, YUKIHO, YAYOI, RITSUKO, AZUSA, IORI, MAKOTO, AMI, MAMI, MIKI, MIKI_SHORT };

void InitializeGameConstant();

extern std::map< std::string, float> g_bpm;
extern std::map< std::string, float> g_scriptDelay;
extern std::map< CHARACTER, int > g_chara2track;
extern std::map< CHARACTER, int > g_chara2mpkID;

}

#endif