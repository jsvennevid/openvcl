/*
 * matrix.h
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

#ifndef _PDKMATRIX_H_
#define _PDKMATRIX_H_

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct st_pdkMatrix
{
  float    m_matrix[4*4];
} pdkMatrix __attribute__((aligned(16)));

// Functions

void pdkMatrixIdentity(pdkMatrix* pMatrix);
void pdkMatrixRotate(pdkMatrix* pMatrix, float x,float y, float z);
void pdkMatrixScale(pdkMatrix* pMatrix, float xscale, float yscale, float zscale);

void pdkMatrixMultiply(pdkMatrix* dest,const pdkMatrix* src1,const pdkMatrix* src2);
void pdkMatrixInvert(pdkMatrix* pDest,const pdkMatrix* pSrc);
void pdkMatrixTranslate(pdkMatrix* pMatrix, float x, float y, float z);

// Util functions

void pdkMatrixViewScreen(pdkMatrix* pMatrix, float scrz, float ax, float ay, 
                                             float cx, float cy, float zmin, 
                                             float zmax, float nearz, float farz);

// Rotation functions

void pdkMatrixRotateY(pdkMatrix* pMatrix, float angle);
void pdkMatrixRotateX(pdkMatrix* pMatrix, float angle);
void pdkMatrixRotateZ(pdkMatrix* pMatrix, float angle);

#endif//_PDKMATRIX_H_

