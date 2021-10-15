#pragma once

#include "NpChar.h"

void JadgeHitNpCharBlock(NPCHAR *npc, int x, int y);
void JudgeHitNpCharTriangleA(NPCHAR *npc, int x, int y);
void JudgeHitNpCharTriangleB(NPCHAR *npc, int x, int y);
void JudgeHitNpCharTriangleC(NPCHAR *npc, int x, int y);
void JudgeHitNpCharTriangleD(NPCHAR *npc, int x, int y);
void JudgeHitNpCharTriangleE(NPCHAR *npc, int x, int y);
void JudgeHitNpCharTriangleF(NPCHAR *npc, int x, int y);
void JudgeHitNpCharTriangleG(NPCHAR *npc, int x, int y);
void JudgeHitNpCharTriangleH(NPCHAR *npc, int x, int y);
void JudgeHitNpCharWater(NPCHAR *npc, int x, int y);
void HitNpCharMap();
void LoseNpChar(NPCHAR *npc, bool bVanish);
void HitNpCharBullet();
