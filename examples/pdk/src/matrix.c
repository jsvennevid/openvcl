/*
 * matrix.c
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#include "matrix.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <tamtypes.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkMatrixIdentity(pdkMatrix* matrix)
{
  int i,j;
  
  for( i = 0; i < 4; i++ )
  {
    for( j = 0; j < 4; j++ )
      matrix->m_matrix[(i<<2)+j] = 0.0f;
    
    matrix->m_matrix[(i<<2)+i] = 1.0f;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkMatrixMultiply(pdkMatrix* dest,const pdkMatrix* src1,const pdkMatrix* src2)
{
  asm __volatile__(
    "lqc2            vf16,0x00(%1)\n"
    "lqc2            vf17,0x10(%1)\n"
    "lqc2            vf18,0x20(%1)\n"
    "lqc2            vf19,0x30(%1)\n"
    "lqc2            vf20,0x00(%2)\n"
    "lqc2            vf21,0x10(%2)\n"
    "lqc2            vf22,0x20(%2)\n"
    "lqc2            vf23,0x30(%2)\n"
    "vmulax          ACC,vf20,vf16\n"
    "vmadday         ACC,vf21,vf16\n"
    "vmaddaz         ACC,vf22,vf16\n"
    "vmaddw          vf16,vf23,vf16\n"
    "vmulax          ACC,vf20,vf17\n"
    "vmadday         ACC,vf21,vf17\n"
    "vmaddaz         ACC,vf22,vf17\n"
    "vmaddw          vf17,vf23,vf17\n"
    "vmulax          ACC,vf20,vf18\n"
    "vmadday         ACC,vf21,vf18\n"
    "vmaddaz         ACC,vf22,vf18\n"
    "vmaddw          vf18,vf23,vf18\n"
    "vmulax          ACC,vf20,vf19\n"
    "vmadday         ACC,vf21,vf19\n"
    "vmaddaz         ACC,vf22,vf19\n"
    "vmaddw          vf19,vf23,vf19\n"
    "sqc2            vf16,0x00(%0)\n"
    "sqc2            vf17,0x10(%0)\n"
    "sqc2            vf18,0x20(%0)\n"
    "sqc2            vf19,0x30(%0)\n"
    : : "r"(dest), "r"(src1), "r"(src2) : "memory");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkMatrixRotateY(pdkMatrix* matrix, float angle)
{
  float sinAngle = (float)pdkSin(angle);
  float cosAngle = (float)pdkCos(angle);

  matrix->m_matrix[(0<<2)+0] = cosAngle;
  matrix->m_matrix[(0<<2)+1] = 0;
  matrix->m_matrix[(0<<2)+2] = -sinAngle;
  matrix->m_matrix[(0<<2)+3] = 0;

  matrix->m_matrix[(1<<2)+0] = 0;
  matrix->m_matrix[(1<<2)+1] = 1;
  matrix->m_matrix[(1<<2)+2] = 0;
  matrix->m_matrix[(1<<2)+3] = 0;

  matrix->m_matrix[(2<<2)+0] = sinAngle;
  matrix->m_matrix[(2<<2)+1] = 0;
  matrix->m_matrix[(2<<2)+2] = cosAngle;
  matrix->m_matrix[(2<<2)+3] = 0;

  matrix->m_matrix[(3<<2)+0] = 0;
  matrix->m_matrix[(3<<2)+1] = 0;
  matrix->m_matrix[(3<<2)+2] = 0;
  matrix->m_matrix[(3<<2)+3] = 1;
}

///////////////////////////////////////////////////////////////////////////////
// void pdkMatrixBuildPitch
///////////////////////////////////////////////////////////////////////////////

void pdkMatrixRotateX(pdkMatrix* matrix, float angle)
{
  float sinAngle = (float)pdkSin( angle );
  float cosAngle = (float)pdkCos(angle );

  matrix->m_matrix[(0<<2)+0] = 1;
  matrix->m_matrix[(0<<2)+1] = 0;
  matrix->m_matrix[(0<<2)+2] = 0;
  matrix->m_matrix[(0<<2)+3] = 0;

  matrix->m_matrix[(1<<2)+0] = 0;
  matrix->m_matrix[(1<<2)+1] = cosAngle;
  matrix->m_matrix[(1<<2)+2] = sinAngle;
  matrix->m_matrix[(1<<2)+3] = 0;

  matrix->m_matrix[(2<<2)+0] = 0;
  matrix->m_matrix[(2<<2)+1] = -sinAngle;
  matrix->m_matrix[(2<<2)+2] = cosAngle;
  matrix->m_matrix[(2<<2)+3] = 0;

  matrix->m_matrix[(3<<2)+0] = 0;
  matrix->m_matrix[(3<<2)+1] = 0;
  matrix->m_matrix[(3<<2)+2] = 0;
  matrix->m_matrix[(3<<2)+3] = 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkMatrixRotateZ(pdkMatrix* matrix, float angle)
{
  float sinAngle = (float)pdkSin(angle);
  float cosAngle = (float)pdkCos(angle);

  matrix->m_matrix[(0<<2)+0] = cosAngle;
  matrix->m_matrix[(0<<2)+1] = sinAngle;
  matrix->m_matrix[(0<<2)+2] = 0;
  matrix->m_matrix[(0<<2)+3] = 0;

  matrix->m_matrix[(1<<2)+0] = -sinAngle;
  matrix->m_matrix[(1<<2)+1] = cosAngle;
  matrix->m_matrix[(1<<2)+2] = 0;
  matrix->m_matrix[(1<<2)+3] = 0;

  matrix->m_matrix[(2<<2)+0] = 0;
  matrix->m_matrix[(2<<2)+1] = 0;
  matrix->m_matrix[(2<<2)+2] = 1;
  matrix->m_matrix[(2<<2)+3] = 0;

  matrix->m_matrix[(3<<2)+0] = 0;
  matrix->m_matrix[(3<<2)+1] = 0;
  matrix->m_matrix[(3<<2)+2] = 0;
  matrix->m_matrix[(3<<2)+3] = 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkMatrixViewScreen(pdkMatrix* matrix, float scrz, float ax, float ay, 
                                            float cx, float cy, float zmin, 
                                            float zmax, float nearz, float farz)
{
  pdkMatrix temp2;
  pdkMatrix temp1;

  float cz = (-zmax * nearz + zmin * farz) / (-nearz + farz);
  float az  = farz * nearz * (-zmin + zmax) / (-nearz + farz);

  //     | scrz    0  0 0 |
  // m = |    0 scrz  0 0 | 
  //     |    0    0  0 1 |
  //     |    0    0  1 0 |
  
  pdkMatrixIdentity( &temp1 ); 
  temp1.m_matrix[(0<<2)+0] = scrz;
  temp1.m_matrix[(1<<2)+1] = scrz;
  temp1.m_matrix[(2<<2)+2] = 0.0f;
  temp1.m_matrix[(3<<2)+3] = 0.0f;
  temp1.m_matrix[(3<<2)+2] = 1.0f;
  temp1.m_matrix[(2<<2)+3] = 1.0f;


  //     | ax  0  0 cx |
  // m = |  0 ay  0 cy | 
  //     |  0  0 az cz |
  //     |  0  0  0  1 |
  
  pdkMatrixIdentity( &temp2 ); 
  temp2.m_matrix[(0<<2)+0] = ax;
  temp2.m_matrix[(1<<2)+1] = ay;
  temp2.m_matrix[(2<<2)+2] = az;
  temp2.m_matrix[(3<<2)+0] = cx;
  temp2.m_matrix[(3<<2)+1] = cy;
  temp2.m_matrix[(3<<2)+2] = cz;

  pdkMatrixMultiply( matrix, &temp1, &temp2 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkMatrixTranslate(pdkMatrix* matrix, float x, float y, float z)
{
  matrix->m_matrix[(3<<2)+0] = x;
  matrix->m_matrix[(3<<2)+1] = y;
  matrix->m_matrix[(3<<2)+2] = z;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pdkMatrixScale(pdkMatrix* matrix, float xscale, float yscale, float zscale)
{
  matrix->m_matrix[(0<<2)+0] = xscale;
  matrix->m_matrix[(1<<2)+1] = yscale;
  matrix->m_matrix[(2<<2)+2] = zscale;
  matrix->m_matrix[(3<<2)+3] = 1;
}

