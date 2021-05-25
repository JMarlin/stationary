/***************************************************************************
                          zn.c  -  description
                             -------------------
    begin                : Sat Jan 31 2004
    copyright            : (C) 2004 by Pete Bernert
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
// 2004/01/31 - Pete  
// - added zn interface
//
//*************************************************************************// 


#include <inttypes.h>   

#define _IN_ZN

#include "externals.h"
 
// --------------------------------------------------- // 
// - psx gpu plugin interface prototypes-------------- //
// --------------------------------------------------- //

#ifdef _WINDOWS
int32_t  GPUopen(HWND hwndGPU);
#else
int32_t GPUopen(uint32_t  * disp,const char * CapText,const char * CfgFile);
#endif
void  GPUdisplayText(char * pText);
void  GPUdisplayFlags(uint32_t  dwFlags);
void  GPUmakeSnapshot(void);
int32_t  GPUinit();
int32_t  GPUclose();
int32_t  GPUshutdown();
void  GPUcursor(int iPlayer,int x,int y);
void  GPUupdateLace(void);
uint32_t   GPUreadStatus(void);
void  GPUwriteStatus(uint32_t  gdata);
void  GPUreadDataMem(uint32_t  * pMem, int iSize);
uint32_t   GPUreadData(void);
void  GPUwriteDataMem(uint32_t  * pMem, int iSize);
void  GPUwriteData(uint32_t  gdata);
void  GPUsetMode(uint32_t  gdata);
int32_t  GPUgetMode(void);
int32_t  GPUdmaChain(uint32_t  * baseAddrL, uint32_t  addr);
int32_t  GPUconfigure(void);
void  GPUabout(void);
int32_t  GPUtest(void);
int32_t  GPUfreeze(uint32_t  ulGetFreezeData,void * pF);
void  GPUgetScreenPic(unsigned char * pMem);
void  GPUshowScreenPic(unsigned char * pMem);
#ifndef _WINDOWS
void  GPUkeypressed(int keycode);
#endif

// --------------------------------------------------- // 
// - zn gpu interface -------------------------------- // 
// --------------------------------------------------- // 

uint32_t  dwGPUVersion=0;
int           iGPUHeight=512;
int           iGPUHeightMask=511;
int           GlobalTextIL=0;
int           iTileCheat=0;

// --------------------------------------------------- //
// --------------------------------------------------- // 
// --------------------------------------------------- // 

typedef struct GPUOTAG
 {
  uint32_t   Version;        // Version of structure - currently 1
  int32_t           hWnd;           // Window handle
  uint32_t   ScreenRotation; // 0 = 0CW, 1 = 90CW, 2 = 180CW, 3 = 270CW = 90CCW
  uint32_t   GPUVersion;     // 0 = a, 1 = b, 2 = c
  const char*    GameName;       // NULL terminated string
  const char*    CfgFile;        // NULL terminated string
 } GPUConfiguration_t;

// --------------------------------------------------- // 
// --------------------------------------------------- // 
// --------------------------------------------------- // 

void  ZN_GPUdisplayFlags(uint32_t  dwFlags)
{
 GPUdisplayFlags(dwFlags);
}

// --------------------------------------------------- //

void  ZN_GPUmakeSnapshot(void)
{
 GPUmakeSnapshot();
}

// --------------------------------------------------- //

int32_t  ZN_GPUinit()
{                                                      // we always set the vram size to 2MB, if the ZN interface is used
 iGPUHeight=1024;
 iGPUHeightMask=1023;

 return GPUinit();
}

// --------------------------------------------------- //

extern char * pConfigFile;

int32_t  ZN_GPUopen(void * vcfg)
{

//----- We don't need this for our purposes------
// GPUConfiguration_t * cfg=(GPUConfiguration_t *)vcfg;
// int32_t lret;
//
// if(!cfg)            return -1;
// if(cfg->Version!=1) return -1;
//
//#ifdef _WINDOWS
// pConfigFile=(char *)cfg->CfgFile;                     // only used in this open, so we can store this temp pointer here without danger... don't access it later, though!
// lret=GPUopen((HWND)cfg->hWnd);
//#else
// lret=GPUopen(&cfg->hWnd,cfg->GameName,cfg->CfgFile);
//#endif
//
///*
// if(!lstrcmp(cfg->GameName,"kikaioh")     ||
//    !lstrcmp(cfg->GameName,"sr2j")        ||
//    !lstrcmp(cfg->GameName,"rvschool_a"))
//  iTileCheat=1;
//*/
//
//// some ZN games seem to erase the cluts with a 'white' TileS... strange..
//// I've added a cheat to avoid this issue. We can set it globally (for
//// all ZiNc games) without much risk
//
//iTileCheat=1;
///
// dwGPUVersion=cfg->GPUVersion;
//
// return lret;

   return 0;
}

// --------------------------------------------------- //

int32_t  ZN_GPUclose()
{
 return GPUclose();
}

// --------------------------------------------------- // 

int32_t  ZN_GPUshutdown()
{
 return GPUshutdown();
}

// --------------------------------------------------- // 

void  ZN_GPUupdateLace(void)
{
 GPUupdateLace();
}

// --------------------------------------------------- // 

uint32_t   ZN_GPUreadStatus(void)
{
 return GPUreadStatus();
}

// --------------------------------------------------- // 

void  ZN_GPUwriteStatus(uint32_t  gdata)
{
 GPUwriteStatus(gdata);
}

// --------------------------------------------------- // 

int32_t  ZN_GPUdmaSliceOut(uint32_t  *baseAddrL, uint32_t  addr, uint32_t  iSize)
{
 GPUreadDataMem(baseAddrL+addr,iSize);
 return 0;
}

// --------------------------------------------------- // 

uint32_t   ZN_GPUreadData(void)
{
 return GPUreadData();
}

// --------------------------------------------------- // 

void  ZN_GPUsetMode(uint32_t  gdata)
{
 GPUsetMode(gdata);
}

// --------------------------------------------------- // 

int32_t  ZN_GPUgetMode(void)
{
 return GPUgetMode();
}

// --------------------------------------------------- // 

int32_t  ZN_GPUdmaSliceIn(uint32_t  *baseAddrL, uint32_t  addr, uint32_t  iSize)
{
 GPUwriteDataMem(baseAddrL+addr,iSize);
 return 0;
}
// --------------------------------------------------- // 

void  ZN_GPUwriteData(uint32_t  gdata)
{
 GPUwriteDataMem(&gdata,1);
}

// --------------------------------------------------- // 

int32_t  ZN_GPUdmaChain(uint32_t  * baseAddrL, uint32_t  addr)
{
 return GPUdmaChain(baseAddrL,addr);
}

// --------------------------------------------------- // 

int32_t  ZN_GPUtest(void)
{
 return GPUtest();
}

// --------------------------------------------------- // 

int32_t  ZN_GPUfreeze(uint32_t  ulGetFreezeData,void * pF)
{
 return GPUfreeze(ulGetFreezeData,pF);
}

// --------------------------------------------------- // 

void  ZN_GPUgetScreenPic(unsigned char * pMem)
{
 GPUgetScreenPic(pMem);
}

// --------------------------------------------------- // 

void  ZN_GPUshowScreenPic(unsigned char * pMem)
{
 GPUshowScreenPic(pMem);
}

// --------------------------------------------------- // 

#ifndef _WINDOWS

void  ZN_GPUkeypressed(int keycode)
{
 GPUkeypressed(keycode);
}

#endif
