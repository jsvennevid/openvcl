/*
 * vu1.c
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include <tamtypes.h>
#include <fileio.h>
#include <kernel.h>
#include <string.h>
#include "vu1.h"
#include "dma.h"
#include "output.h"
#include "vif.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static char dmaBuffer1[100*1024] __attribute__((aligned(16)));
static char dmaBuffer2[100*1024] __attribute__((aligned(16)));
static void* currentBuffer;
static int   switchBuffer = 0;
static pdkVu1ListBuild buildList;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkVu1UploadProg(int Dest, void* start, void* end)
{
  int   count = 0;
  u8 tempDma[512] ALIGNED(16);
  void* chain = (u64*)&tempDma; // uncached

  // get the size of the code as we can only send 256 instructions in each MPGtag

  count = pdkVu1Size( start, end ); 

  while( count > 0 )
  {
    u32 current_count = count > 256 ? 256 : count;
    
    *((u64*) chain)++ = DMA_REF_TAG( (u32)start, current_count/2 );
    *((u32*) chain)++ = VIF_CODE( VIF_NOP,0,0 );
    *((u32*) chain)++ = VIF_CODE( VIF_MPG,current_count&0xff,Dest );

    start += current_count*2;
    count -= current_count; 
    Dest += current_count;
  }

  *((u64*) chain)++ = DMA_END_TAG( 0 );
  *((u32*) chain)++ = VIF_CODE(VIF_NOP,0,0);
  *((u32*) chain)++ = VIF_CODE(VIF_NOP,0,0);

  // Send it to vif1
  FlushCache(0);
  pdkDma01SendChain(tempDma);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkVu1ListBegin()
{
  switchBuffer = !switchBuffer;

  memset((char*)&buildList,0,sizeof(buildList));

  if(switchBuffer)
    currentBuffer = (char*)&dmaBuffer1;
  else
    currentBuffer = (char*)&dmaBuffer2;

  buildList.kickBuffer = currentBuffer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkVu1ListData(int destAdress,void* data,int quadSize)
{
  if(((u32)data & 0xf))
  {
    pdkOut("Data to pdkVu1ListAddData is not 16byte aligned!, data WILL be wrong\n");
  }

  *((u64*)currentBuffer)++ = DMA_REF_TAG( (u32)data, quadSize );
  *((u32*)currentBuffer)++ = VIF_CODE( VIF_STCYL,0,0x0101 );
  *((u32*)currentBuffer)++ = VIF_CODE( VIF_UNPACK_V4_32,quadSize,destAdress );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkVu1ListAddBegin(int adress)
{
  if(buildList.isBuidingCnt == 1)
    pdkOut("pdkVu1ListAddBegin: Already building dmalist!\n");

  buildList.cntDmaDest = adress;
  buildList.offset = currentBuffer;
  *((u64*)currentBuffer)++ = DMA_CNT_TAG(0);
  *((u32*)currentBuffer)++ = VIF_CODE(VIF_STCYL,0,0x0101 );
  *((u32*)currentBuffer)++ = VIF_CODE(VIF_UNPACK_V4_32,0,0);
  buildList.isBuidingCnt = 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkVu1ListAdd128(u64 v1,u64 v2)
{
  pdkVu1ListCheckCnt();

  *((u64*)currentBuffer)++ = v1;
  *((u64*)currentBuffer)++ = v2;

  buildList.dmaSize += 16;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkVu1ListAdd64(u64 v)
{
  pdkVu1ListCheckCnt();

  *((u64*)currentBuffer)++ = v;

  buildList.dmaSize += 8;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkVu1ListAdd32(u32 v)
{
  pdkVu1ListCheckCnt();

  *((u32*)currentBuffer)++ = v;

  buildList.dmaSize += 4;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkVu1ListAddFloat(float v)
{
  pdkVu1ListCheckCnt();

  *((float*)currentBuffer)++ = v;

  buildList.dmaSize += 4;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkVu1ListEnd(int start)
{
  *((u64*)currentBuffer)++ = DMA_END_TAG(0);
  *((u32*)currentBuffer)++ = VIF_CODE(VIF_NOP,0,0);

  if(start>=0)
    *((u32*)currentBuffer)++ = VIF_CODE(VIF_MSCAL,0,start);
  else
    *((u32*)currentBuffer)++ = VIF_CODE(VIF_NOP,0,0);

  pdkDmaWait(1);
  pdkDma01SendChain(buildList.kickBuffer);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkVu1ListCheckCnt()
{
  if(buildList.isBuidingCnt == 0)
    pdkOut("pdkVu1ListCheckCnt: pdkVu1ListAddBegin not called\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkVu1ListAddEnd()
{
  if(buildList.isBuidingCnt == 0)
  {
    pdkOut("pdkVu1ListAddEnd: pdkVu1ListAddBegin was not called, nothing to end!\n");
    return;
  }
  
  // pad to qword alignment
  while((buildList.dmaSize & 0xf))
  {
    *((u32*)currentBuffer)++ = 0;    
    buildList.dmaSize += 4;
  }
    
  *((u64*)buildList.offset)++ = DMA_CNT_TAG(buildList.dmaSize >> 4);
  *((u32*)buildList.offset)++ = VIF_CODE(VIF_STCYL,0,0x0101 );
  *((u32*)buildList.offset)++ = VIF_CODE(VIF_UNPACK_V4_32,buildList.dmaSize >> 4,buildList.cntDmaDest);
  
  buildList.isBuidingCnt = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int pdkVu1Size(u32* start, u32* end)
{
  int size = (end-start)/2;

	// if size is odd we have make it even in order for the transfer to work
	// (quadwords, because of that its VERY important to have an extra nop nop
	// at the end of each vuprogram

	if( size&1 )
		size++;

	return size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int FileWrite( const char* pName, void* pData, int size )
{
	int fd;
	fd = fioOpen( pName, O_CREAT|O_WRONLY );
	
	if( fd > 0 )
	{
		if( fioWrite( fd, pData, size) != size )
		{
			fioClose(fd);
			SleepThread();
		}
		fioClose(fd);
	}
	else
	{
		return -1;
		SleepThread();
	}

  return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkVu1Dump()
{
	asm __volatile__(
		"nop\n"
		"nop\n"
"0:\n"
		"bc2t 0b\n"
		"nop\n"
		"nop\n" );
  
  FileWrite( "host:VU1MICROMEM",((void*)0x11008000),1024*16 );
  FileWrite( "host:VU1MEM", ((void*)0x1100c000),1024*16 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkVu1DumpOnce()
{
  static int hasDUmped = 0;

  if(hasDUmped == 0)
  {
    pdkDmaWait(1);
    pdkVu1Dump();
    hasDUmped = 1;
  }
}


