/*
 * gs.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#ifndef _PDK_DMA_H_
#define _PDK_DMA_H_

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tamtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkDmaWait(int channel);
void pdkDmaSend(void* data,u32 QuadSize);
void pdkDmaSendChain(void* data);
void pdkDma01SendChain(void* data);
void pdkDmaStat();
void pkGifStat();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Defines
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DMA_CALL_TAG(Addr, iQuadCount)	((((u64)(Addr)) << 32) | (0x5 << 28) | iQuadCount)
#define DMA_RET_TAG(iQuadCount)					((((u64)(0x6)) << 28) | iQuadCount)
#define DMA_REF_TAG(ADDR, COUNT) ((((unsigned long)ADDR) << 32) | (0x3 << 28) | COUNT )
#define DMA_CNT_TAG(COUNT)       (((unsigned long)(0x1) << 28) | COUNT)
#define DMA_END_TAG(COUNT)       (((unsigned long)(0x7) << 28) | COUNT)
#define DMA_CHCR(DIR,MOD,ASP,TTE,TIE,STR) ( (((u32)DIR)<<0) | (((u32)MOD)<<2) | \
                                            (((u32)ASP)<<4) | (((u32)TTE)<<6) | \
                                            (((u32)TIE)<<7) | (((u32)STR)<<8) )
#ifdef __cplusplus
};
#endif

#endif //_PDK_DMA_H_

