/*
 * vu1Triangle.vcl
 *
 * Copyright (C) 2004 Jesper Svennevid, Daniel Collin
 *
 * Licensed under the AFL v2.0. See the file LICENSE included with this
 * distribution for licensing terms.
 *
 */

 		.syntax new
		.name vu1Triangle		
		.vu
		.init_vf_all
		.init_vi_all

		--enter
		--endenter

		lq		worldToScreenRow0, 0(vi00)
		lq		worldToScreenRow1, 1(vi00)
		lq		worldToScreenRow2, 2(vi00)
		lq		worldToScreenRow3, 3(vi00)

		iaddiu		vertexData, vi00, 6
		iaddiu		destAdress, vi00, 20
		iaddiu		kickAdress, vi00, 20

		lq		gifTag, 4(vi00)
		sqi		gifTag, (destAdress++)
		
		lq		color, 5(vi00)
		sqi		color, (destAdress++)

		iaddiu		vertexCounter, vi00, 3
vertexLoop:
		lqi		vertex, (vertexData++)

		mul		acc, worldToScreenRow0, vertex[x]
		madd		acc, worldToScreenRow1, vertex[y]
		madd		acc, worldToScreenRow2, vertex[z]
		madd		vertex, worldToScreenRow3, vertex[w]

		div		q, vf00[w], vertex[w]
		mul.xyz		vertex, vertex, q
		ftoi4		vertex, vertex

		sqi		vertex, (destAdress++)

		iaddi		vertexCounter, vertexCounter, -1
		ibne		vertexCounter, vi00, vertexLoop
		
		xgkick		kickAdress

		--exit
		--endexit
