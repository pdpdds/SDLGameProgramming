#pragma once

#include "WindowsWrapper.h"

extern unsigned char gFlagNPC[1000];
extern unsigned char gSkipFlag[0x40];

void InitFlags();
void InitSkipFlags();
void SetNPCFlag(long a);
void CutNPCFlag(long a);
BOOL GetNPCFlag(long a);
void SetSkipFlag(long a);
void CutSkipFlag(long a);
BOOL GetSkipFlag(long a);
