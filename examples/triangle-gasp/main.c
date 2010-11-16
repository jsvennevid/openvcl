/*
 * gs.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include <tamtypes.h>
#include <kernel.h>
#include <screen.h>
#include <matrix.h>
#include <vu1.h>
#include <output.h>
#include "../pdk/src/gs.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void update(pdkMatrix* matrix);
void init();

extern u32 vu1Triangle_CodeStart __attribute__((section(".vudata")));
extern u32 vu1Triangle_CodeEnd __attribute__((section(".vudata")));

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
static pdkMatrix viewscreenMatrix;
static pdkMatrix cameraMatrix;
static pdkMatrix rotateMatrix;
static pdkMatrix finalMatrix;
static pdkMatrix temprotMatrix;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  float angle = 0.0f;  

  pdkGsEnv* env = pdkScreenSetup();

  init();

  // Create our view to screen matrix

  pdkMatrixViewScreen( &viewscreenMatrix, 512.0f,1.0f,1.33f, 
                      2048+640/2,2048+256/2,
                      1.0f, 6777215.0f,64.0f, 5536.0f );

  // Setup the matrix for our camera

  pdkMatrixIdentity(&cameraMatrix );
  pdkMatrixTranslate(&cameraMatrix, 0, 0, 800);

  // Loop of the demo

  while(1)
  {
    angle += 0.03f;
    
    pdkScreenClear(env);
    
    // Rotate & build final matrix to be used in the vu1 program.

    pdkMatrixRotateZ(&rotateMatrix, angle );
    pdkMatrixMultiply(&temprotMatrix, &rotateMatrix, &cameraMatrix);
    pdkMatrixMultiply(&finalMatrix, &temprotMatrix, &viewscreenMatrix);

    update(&finalMatrix);

    pdkScreenVsync(env);
    pdkScreenFlip(env);
  }
    
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init()
{
  pdkVu1UploadProg(0,&vu1Triangle_CodeStart,&vu1Triangle_CodeEnd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void update(pdkMatrix* matrix)
{
  pdkVu1ListBegin();

  // matrix will be uploaded at adress 0 in vumemory.  

  pdkVu1ListData(0,matrix,4);
  
  // data added from here will begin at adress 4
  
  pdkVu1ListAddBegin(4);
  pdkVu1ListAdd128(GS_GIFTAG(1, 1, 1, GS_PRIM( GS_PRIM_TRIANGLE, GS_PRIM_SFLAT, GS_PRIM_TOFF, 
                                               GS_PRIM_FOFF, GS_PRIM_ABOFF, GS_PRIM_AAOFF, GS_PRIM_FSTQ, 
                                               GS_PRIM_C1, 0), 
                                               GS_GIFTAG_PACKED, 4),
                   0x5551);

  // adress 5 (color)
  
  pdkVu1ListAdd32(127);
  pdkVu1ListAdd32(127);
  pdkVu1ListAdd32(127);
  pdkVu1ListAdd32(0);

  // adress 6-9 (vertex coords)
  
  pdkVu1ListAddFloat(-100.0f);
  pdkVu1ListAddFloat(100.0f);
  pdkVu1ListAddFloat(0.0f);
  pdkVu1ListAddFloat(1.0f);

  pdkVu1ListAddFloat(-100.0f);
  pdkVu1ListAddFloat(-100.0f);
  pdkVu1ListAddFloat(0.0f);
  pdkVu1ListAddFloat(1.0f);

  pdkVu1ListAddFloat(50.0f);
  pdkVu1ListAddFloat(50.0f);
  pdkVu1ListAddFloat(0.0f);
  pdkVu1ListAddFloat(1.0f);

  pdkVu1ListAddEnd();

  // end the list and start the vu program (located in micromem adress 0)

  pdkVu1ListEnd(0);
}
