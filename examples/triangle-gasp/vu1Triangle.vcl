/*
 * vu1Triangle.vcl
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

destOffset      .assign         20
kickOffset      .assign         20

matrixOffset    .assign         0
gifTagOffset    .assign         4
colorOffset     .assign         5
vertexOffset    .assign         6

vertexCount     .assign         3

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

.macro   matrixMultiplyVertex vertexResult,matrix,vertexIn
  mul            acc,           \matrix[0], \vertexIn[x]
  madd           acc,           \matrix[1], \vertexIn[y]
  madd           acc,           \matrix[2], \vertexIn[z]
  madd           \vertexResult, \matrix[3], \vertexIn[w]
.endm

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                .syntax new
                .name vu1Triangle             
                .vu
                .init_vf_all
                .init_vi_all
                --enter
                --endenter

                iaddiu          vertexData, vi00, vertexOffset
                iaddiu          destAddress, vi00, destOffset
                iaddiu          kickAddress, vi00, kickOffset

								lq              worldToScreen[0], matrixOffset+0(vi00)
                lq              worldToScreen[1], matrixOffset+1(vi00)
                lq              worldToScreen[2], matrixOffset+2(vi00)
                lq              worldToScreen[3], matrixOffset+3(vi00)

                lq              gifTag, gifTagOffset(vi00)
                sqi             gifTag, (destAddress++)
                
                lq              color, colorOffset(vi00)
                sqi             color, (destAddress++)

                iaddiu          vertexCounter, vi00, vertexCount
vertexLoop:
                lqi             vertex, (vertexData++)

                matrixMultiplyVertex vertexOut, worldToScreen, vertex

                div             q, vf00[w], vertexOut[w]
                mul.xyz         vertexOut, vertexOut, q
                ftoi4           vertexOut, vertexOut

                sqi             vertexOut, (destAddress++)

                iaddi           vertexCounter, vertexCounter, -1
                ibne            vertexCounter, vi00, vertexLoop
                
                xgkick          kickAddress

								--exit
								--endexit
.END
