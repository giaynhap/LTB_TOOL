
#include "FreeImage.h"
#include <windows.h> 
#include <GL/gl.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <iostream>
#include <cstdint>
#include <math.h>
#include <vector>
#include <tdx.h>
#pragma comment(lib,"F:\\visual studio 2010\\Projects\\class_math_matrix\\class_math_matrix\\FreeImage.lib")

typedef struct DtxHeader
{
	unsigned int iResType;
	int iVersion;
	unsigned short usWidth;
	unsigned short usHeight;
	unsigned short usMipmaps;
	unsigned short usSections;
	int iFlags;
	int iUserFlags;
	unsigned char ubExtra[12];
	char szCommandString[128];
};
 

#pragma once
#define EXP5TO8R(packedcol)					\
   ((((packedcol) >> 8) & 0xf8) | (((packedcol) >> 13) & 0x7))

#define EXP6TO8G(packedcol)					\
   ((((packedcol) >> 3) & 0xfc) | (((packedcol) >>  9) & 0x3))

#define EXP5TO8B(packedcol)					\
   ((((packedcol) << 3) & 0xf8) | (((packedcol) >>  2) & 0x7))

#define EXP4TO8(col)						\
   ((col) | ((col) << 4))

// ########################################## Thu vien DTX (convert color bit)

// ########################################## Thu vien DTX (convert color bit)

static void dxt135_decode_imageblock(const GLubyte *img_block_src,
	GLint i, GLint j, GLuint dxt_type, GLvoid *texel) {
	GLchan *rgba = (GLchan *)texel;
	const GLushort color0 = img_block_src[0] | (img_block_src[1] << 8);
	const GLushort color1 = img_block_src[2] | (img_block_src[3] << 8);
	const GLuint bits = img_block_src[4] | (img_block_src[5] << 8) |
		(img_block_src[6] << 16) | (img_block_src[7] << 24);
	/* What about big/little endian? */
	GLubyte bit_pos = 2 * (j * 4 + i);
	GLubyte code = (GLubyte)((bits >> bit_pos) & 3);

	rgba[ACOMP] = CHAN_MAX;
	switch (code) {
	case 0:
		rgba[RCOMP] = UBYTE_TO_CHAN(EXP5TO8R(color0));
		rgba[GCOMP] = UBYTE_TO_CHAN(EXP6TO8G(color0));
		rgba[BCOMP] = UBYTE_TO_CHAN(EXP5TO8B(color0));
		break;
	case 1:
		rgba[RCOMP] = UBYTE_TO_CHAN(EXP5TO8R(color1));
		rgba[GCOMP] = UBYTE_TO_CHAN(EXP6TO8G(color1));
		rgba[BCOMP] = UBYTE_TO_CHAN(EXP5TO8B(color1));
		break;
	case 2:
		if (color0 > color1) {
			rgba[RCOMP] = UBYTE_TO_CHAN(((EXP5TO8R(color0) * 2 + EXP5TO8R(color1)) / 3));
			rgba[GCOMP] = UBYTE_TO_CHAN(((EXP6TO8G(color0) * 2 + EXP6TO8G(color1)) / 3));
			rgba[BCOMP] = UBYTE_TO_CHAN(((EXP5TO8B(color0) * 2 + EXP5TO8B(color1)) / 3));
		}
		else {
			rgba[RCOMP] = UBYTE_TO_CHAN(((EXP5TO8R(color0) + EXP5TO8R(color1)) / 2));
			rgba[GCOMP] = UBYTE_TO_CHAN(((EXP6TO8G(color0) + EXP6TO8G(color1)) / 2));
			rgba[BCOMP] = UBYTE_TO_CHAN(((EXP5TO8B(color0) + EXP5TO8B(color1)) / 2));
		}
		break;
	case 3:
		if ((dxt_type > 1) || (color0 > color1)) {
			rgba[RCOMP] = UBYTE_TO_CHAN(((EXP5TO8R(color0) + EXP5TO8R(color1) * 2) / 3));
			rgba[GCOMP] = UBYTE_TO_CHAN(((EXP6TO8G(color0) + EXP6TO8G(color1) * 2) / 3));
			rgba[BCOMP] = UBYTE_TO_CHAN(((EXP5TO8B(color0) + EXP5TO8B(color1) * 2) / 3));
		}
		else {
			rgba[RCOMP] = 0;
			rgba[GCOMP] = 0;
			rgba[BCOMP] = 0;
			if (dxt_type == 1) rgba[ACOMP] = UBYTE_TO_CHAN(0);
		}
		break;
	default:
		/* CANNOT happen (I hope) */
		break;
	}
}

void fetch_2d_texel_rgb_dxt1(GLint srcRowStride, const GLubyte *pixdata,
	GLint i, GLint j, GLvoid *texel)
{
	/* Extract the (i,j) pixel from pixdata and return it
	* in texel[RCOMP], texel[GCOMP], texel[BCOMP], texel[ACOMP].
	*/

	const GLubyte *blksrc = (pixdata + ((srcRowStride + 3) / 4 * (j / 4) + (i / 4)) * 8);
	dxt135_decode_imageblock(blksrc, (i & 3), (j & 3), 0, texel);
}

void fetch_2d_texel_rgba_dxt1(GLint srcRowStride, const GLubyte *pixdata,
	GLint i, GLint j, GLvoid *texel)
{
	/* Extract the (i,j) pixel from pixdata and return it
	* in texel[RCOMP], texel[GCOMP], texel[BCOMP], texel[ACOMP].
	*/

	const GLubyte *blksrc = (pixdata + ((srcRowStride + 3) / 4 * (j / 4) + (i / 4)) * 8);
	dxt135_decode_imageblock(blksrc, (i & 3), (j & 3), 1, texel);
}

void fetch_2d_texel_rgba_dxt3(GLint srcRowStride, const GLubyte *pixdata,
	GLint i, GLint j, GLvoid *texel) {

	/* Extract the (i,j) pixel from pixdata and return it
	* in texel[RCOMP], texel[GCOMP], texel[BCOMP], texel[ACOMP].
	*/

	GLchan *rgba = (GLchan *)texel;
	const GLubyte *blksrc = (pixdata + ((srcRowStride + 3) / 4 * (j / 4) + (i / 4)) * 16);
#if 0
	/* Simple 32bit version. */
	/* that's pretty brain-dead for a single pixel, isn't it? */
	const GLubyte bit_pos = 4 * ((j & 3) * 4 + (i & 3));
	const GLuint alpha_low = blksrc[0] | (blksrc[1] << 8) | (blksrc[2] << 16) | (blksrc[3] << 24);
	const GLuint alpha_high = blksrc[4] | (blksrc[5] << 8) | (blksrc[6] << 16) | (blksrc[7] << 24);

	dxt135_decode_imageblock(blksrc + 8, (i & 3), (j & 3), 2, texel);
	if (bit_pos < 32)
		rgba[ACOMP] = UBYTE_TO_CHAN((GLubyte)(EXP4TO8((alpha_low >> bit_pos) & 15)));
	else
		rgba[ACOMP] = UBYTE_TO_CHAN((GLubyte)(EXP4TO8((alpha_high >> (bit_pos - 32)) & 15)));
#endif
#if 1
	/* TODO test this! */
	const GLubyte anibble = (blksrc[((j & 3) * 4 + (i & 3)) / 2] >> (4 * (i & 1))) & 0xf;
	dxt135_decode_imageblock(blksrc + 8, (i & 3), (j & 3), 2, texel);
	rgba[ACOMP] = UBYTE_TO_CHAN((GLubyte)(EXP4TO8(anibble)));
#endif

}

void fetch_2d_texel_rgba_dxt5(GLint srcRowStride, const GLubyte *pixdata,
	GLint i, GLint j, GLvoid *texel) {

	/* Extract the (i,j) pixel from pixdata and return it
	* in texel[RCOMP], texel[GCOMP], texel[BCOMP], texel[ACOMP].
	*/

	GLchan *rgba = (GLchan *)texel;
	const GLubyte *blksrc = (pixdata + ((srcRowStride + 3) / 4 * (j / 4) + (i / 4)) * 16);
	const GLubyte alpha0 = blksrc[0];
	const GLubyte alpha1 = blksrc[1];
#if 0
	const GLubyte bit_pos = 3 * ((j & 3) * 4 + (i & 3));
	/* simple 32bit version */
	const GLuint bits_low = blksrc[2] | (blksrc[3] << 8) | (blksrc[4] << 16) | (blksrc[5] << 24);
	const GLuint bits_high = blksrc[6] | (blksrc[7] << 8);
	GLubyte code;

	if (bit_pos < 30)
		code = (GLubyte)((bits_low >> bit_pos) & 7);
	else if (bit_pos == 30)
		code = (GLubyte)((bits_low >> 30) & 3) | ((bits_high << 2) & 4);
	else
		code = (GLubyte)((bits_high >> (bit_pos - 32)) & 7);
#endif
#if 1
	/* TODO test this! */
	const GLubyte bit_pos = ((j & 3) * 4 + (i & 3)) * 3;
	const GLubyte acodelow = blksrc[2 + bit_pos / 8];
	const GLubyte acodehigh = blksrc[3 + bit_pos / 8];
	const GLubyte code = (acodelow >> (bit_pos & 0x7) |
		(acodehigh << (8 - (bit_pos & 0x7)))) & 0x7;
#endif
	dxt135_decode_imageblock(blksrc + 8, (i & 3), (j & 3), 2, texel);
#if 0
	if (alpha0 > alpha1) {
		switch (code) {
		case 0:
			rgba[ACOMP] = UBYTE_TO_CHAN(alpha0);
			break;
		case 1:
			rgba[ACOMP] = UBYTE_TO_CHAN(alpha1);
			break;
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			rgba[ACOMP] = UBYTE_TO_CHAN(((alpha0 * (8 - code) + (alpha1 * (code - 1))) / 7));
			break;
}
}
	else {
		switch (code) {
		case 0:
			rgba[ACOMP] = UBYTE_TO_CHAN(alpha0);
			break;
		case 1:
			rgba[ACOMP] = UBYTE_TO_CHAN(alpha1);
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			rgba[ACOMP] = UBYTE_TO_CHAN(((alpha0 * (6 - code) + (alpha1 * (code - 1))) / 5));
			break;
		case 6:
			rgba[ACOMP] = 0;
			break;
		case 7:
			rgba[ACOMP] = CHAN_MAX;
			break;
		}
	}
#endif
#if 1
	/* TODO test this */
	if (code == 0)
		rgba[ACOMP] = UBYTE_TO_CHAN(alpha0);
	else if (code == 1)
		rgba[ACOMP] = UBYTE_TO_CHAN(alpha1);
	else if (alpha0 > alpha1)
		rgba[ACOMP] = UBYTE_TO_CHAN(((alpha0 * (8 - code) + (alpha1 * (code - 1))) / 7));
	else if (code < 6)
		rgba[ACOMP] = UBYTE_TO_CHAN(((alpha0 * (6 - code) + (alpha1 * (code - 1))) / 5));
	else if (code == 6)
		rgba[ACOMP] = 0;
	else
		rgba[ACOMP] = CHAN_MAX;
#endif
}





int LoadDTXImage(char *szFileName, int Index, int *pWidth, int *pHeight, unsigned char * returnBuffer = NULL)
{
	FILE  *pFile;
	pFile = fopen(szFileName, "rb");

	if (!pFile)
		return false;

	DtxHeader Header;
	memset(&Header, 0, sizeof(Header));

	fread(&Header, sizeof(Header), 1, pFile);

	if (Header.iResType != 0 || Header.iVersion != -5 || Header.usMipmaps == 0)
	{
		fclose(pFile);
		return -1;
	}
	static unsigned char pBuffer[1024 * 1024 * 4];
	static unsigned char imageData[1024 * 1024 * 4];
	memset(pBuffer, 0, sizeof(pBuffer));
	memset(imageData, 0, sizeof(imageData));
	*pWidth = Header.usWidth;
	*pHeight = Header.usHeight;

	int iBpp = Header.ubExtra[2];
	int iSize;
	int pInternalFormat;
	if (iBpp == 3)
	{
		iSize = Header.usWidth * Header.usHeight * 4;
		pInternalFormat = GL_RGBA;
	}
	else if (iBpp == 4)
	{
		iSize = (Header.usWidth * Header.usHeight) >> 1;
		pInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	}
	else if (iBpp == 5)
	{
		iSize = Header.usWidth * Header.usHeight;
		pInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	}
	else if (iBpp == 6)
	{
		iSize = Header.usWidth * Header.usHeight;
		pInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	}
	else
	{
		iSize = 0;
	}

	int pImageSize = iSize;


	int pcurcolor = 0;

	if (iSize == 0)
	{
		fclose(pFile);
		return -1;
	}

	fread(pBuffer, iSize, 1, pFile);
	
	if (iBpp == 3)
	{
		for (unsigned int i = 0; i < iSize; i += 4)
		{
			pBuffer[i + 0] ^= pBuffer[i + 2];
			pBuffer[i + 2] ^= pBuffer[i + 0];
			pBuffer[i + 0] ^= pBuffer[i + 2];

		}

	}
	else
	{
		char data[4];
		for (int y = 0; y < Header.usHeight; ++y)
			for (int x = 0; x < Header.usWidth; ++x)
			{
				switch (iBpp) {
				case 4:
					memset(data, 0, 4);
					fetch_2d_texel_rgba_dxt1(Header.usWidth, pBuffer, x, y, &data);
					//	std::swap(data[0], data[2]);
					imageData[pcurcolor++] = data[0];
					imageData[pcurcolor++] = data[1];
					imageData[pcurcolor++] = data[2];
					if (data[3]<0 || data[3]>255) data[3] = 0;
					imageData[pcurcolor++] = data[3];
					break;
				case 5:
					memset(data, 0, 4);
					fetch_2d_texel_rgba_dxt3(Header.usWidth, pBuffer, x, y, &data);
					//	std::swap(data[0], data[2]);
					imageData[pcurcolor++] = data[0];
					imageData[pcurcolor++] = data[1];
					imageData[pcurcolor++] = data[2];
					if (data[3]<0 || data[3]>255) data[3] = 0;
					imageData[pcurcolor++] = data[3];
					break;
				case 6:
					memset(data, 0, 4);
					fetch_2d_texel_rgba_dxt5(Header.usWidth, pBuffer, x, y, &data);
					//	std::swap(data[0], data[2]);
					imageData[pcurcolor++] = data[0];
					imageData[pcurcolor++] = data[1];
					imageData[pcurcolor++] = data[2];
					if (data[3]<0 || data[3]>255) data[3] = 0;
					imageData[pcurcolor++] = data[3];

					break;
				}

			}
		pImageSize = pcurcolor;
		memcpy(pBuffer, imageData, pcurcolor);
	}
	if (returnBuffer != NULL)
	{
		memcpy(returnBuffer, pBuffer, pcurcolor);
		for (unsigned int i = 0; i < pcurcolor; i += 4)
		{
			returnBuffer[i + 0] ^= returnBuffer[i + 2];
			returnBuffer[i + 2] ^= returnBuffer[i + 0];
			returnBuffer[i + 0] ^= returnBuffer[i + 2];

		}

	}
	fclose(pFile);
	if (Index == NULL) return 0;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Index);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *pWidth, *pHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
	//free(pBuffer);
	

	return 1;
}