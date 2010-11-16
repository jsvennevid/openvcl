/*
 * vu1.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#ifndef _PDK_VU1_H_
#define _PDK_VU1_H_

typedef struct st_pdkVu1ListBuild
{
  int   isBuidingCnt;
  void* offset;
  void* kickBuffer;
  int   pos;
  int   dmaSize;
  int   cntDmaDest;

} pdkVu1ListBuild;

void pdkVu1Dump();
void pdkVu1DumpOnce();

void pdkVu1UploadProg(int Dest, void* start, void* end);
void pdkVu1ListBegin();
void pdkVu1ListData(int destAdress,void* data,int quadSize);
void pdkVu1ListAddBegin(int adress);
void pdkVu1ListAdd128(u64 v1,u64 v2);
void pdkVu1ListAdd64(u64 v);
void pdkVu1ListAdd32(u32 v);
void pdkVu1ListAddFloat(float v);
void pdkVu1ListAddEnd();
void pdkVu1ListEnd(int start);

// internal functions

void pdkVu1ListCheckCnt();
void pdkvu1ListCheckEndCnt();  
int  pdkVu1Size(u32* start, u32* end);

#endif//_PDK_VU1_H_

