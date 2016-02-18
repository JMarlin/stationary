/***************************************************************************
                          cfg.c  -  description
                             -------------------
    begin                : Sun Oct 28 2001
    copyright            : (C) 2001 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

//*************************************************************************//
// History of changes:
//
// 2008/04/27 - Pete
// - SSSPSXLimitBtn and game fix 0x400
//
// 2005/04/15 - Pete
// - Changed user frame limit to floating point value
//
// 2003/01/26 - linuzappz
// - rewrote Config/About dialogs using Glade
// - fullscreen modes are get through XF86VM if enabled
//
// 2003/01/04 - Pete
// - added dithering/scale2x options and odd/even game fix
//
// 2002/11/06 - Pete
// - added 2xSai, Super2xSaI, SuperEagle cfg stuff
//
// 2002/09/27 - linuzappz
// - added based on cfg.c for linux gui
//
//*************************************************************************//

#include "stdafx.h"


#define _IN_CFG

#undef FALSE
#undef TRUE

#ifdef USE_XF86VM

#include <X11/extensions/xf86vmode.h>
static XF86VidModeModeInfo **modes=0;

#endif

#include "externals.h"
#include "cfg.h"
#include "gpu.h"


int            iResX;
int            iResY;
long           GlobalTextAddrX,GlobalTextAddrY,GlobalTextTP;
long           GlobalTextREST,GlobalTextABR,GlobalTextPAGE;
short          ly0,lx0,ly1,lx1,ly2,lx2,ly3,lx3;
long           lLowerpart;
BOOL           bIsFirstFrame;
int            iWinSize;
BOOL           bCheckMask;
unsigned short sSetMask;
unsigned long  lSetMask;
BOOL           bDeviceOK;
short          g_m1;
short          g_m2;
short          g_m3;
short          DrawSemiTrans;
int            iUseGammaVal;
int            iUseScanLines;
int            iUseDither;
int            iDesktopCol;
int            iUseNoStretchBlt;
int            iShowFPS;
int            iFastFwd;


BOOL           bUsingTWin;
TWin_t         TWin;
unsigned long  clutid;
void (*primTableJ[256])(unsigned char *);
void (*primTableSkip[256])(unsigned char *);
unsigned short  usMirror;
unsigned long  dwCfgFixes;
unsigned long  dwActFixes;
int            iUseFixes;
BOOL           bDoVSyncUpdate;
long           drawX;
long           drawY;
long           drawW;
long           drawH;

VRAMLoad_t     VRAMWrite;
VRAMLoad_t     VRAMRead;
DATAREGISTERMODES DataWriteMode;
DATAREGISTERMODES DataReadMode;
int            iColDepth;
int            iWindowMode;
//char           szDispBuf[];
//char           szMenuBuf[];
//char           szDebugText[];
//short          sDispWidths[];
BOOL           bDebugText;
//unsigned int   iMaxDMACommandCounter;
//unsigned long  dwDMAChainStop;
PSXDisplay_t   PSXDisplay;
PSXDisplay_t   PreviousPSXDisplay;
BOOL           bSkipNextFrame;
long           lGPUstatusRet;
long           drawingLines;
unsigned char  * psxVub;
signed char    * psxVsb;
unsigned short * psxVuw;
signed short   * psxVsw;
unsigned long  * psxVul;
signed long    * psxVsl;
BOOL           bChangeWinMode;
long           lSelectedSlot;
BOOL           bInitCap;


unsigned long dwCoreFlags;

unsigned long  ulKeybits;


int            UseFrameLimit;
int            UseFrameSkip;
BOOL           bSSSPSXLimit;
float          fFrameRate;
int            iFrameLimit;
float          fFrameRateHz;
float          fps_skip;
float          fps_cur;

char           modesl[64][32];

char * pConfigFile=NULL;
