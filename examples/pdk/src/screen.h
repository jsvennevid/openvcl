/*
 * screen.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#ifndef _PDK_SCREEN_H_
#define _PDK_SCREEN_H_

#define THE_SMODE2 ((volatile u64 *)(0x12000020))

typedef struct st_pdkGsEnv
{
  u32 Screen0;
  u32 Screen1;
  u32 Zbuffer;
  u32 Width;
  u32 Height;
  u32 Psm;
  u32 Flip;

} pdkGsEnv;

pdkGsEnv* pdkScreenSetup();
void pdkScreenClear(pdkGsEnv* env);
void pdkScreenVsync(pdkGsEnv* env);
void pdkScreenFlip(pdkGsEnv* env);

#endif//_PDK_SCREEN_H_

