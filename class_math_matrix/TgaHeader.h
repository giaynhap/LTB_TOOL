
typedef unsigned short  word; 
typedef unsigned long  DWORD; 
#include "gl\glut.h"
#include <malloc.h>
#include <stdio.h>

#define TEXTUREID_BASE 6000
int g_iTextureNums = 0;
int meshText = 0;

int TEX_GenerateID(void)
{
	return TEXTUREID_BASE + g_iTextureNums++;
}


typedef struct loadtga_s
{
	byte	id_length;
	byte	colormap_type;
	byte	image_type;
	word	colormap_index;
	word	colormap_length;
	byte	colormap_size;
	word	x_origin;
	word	y_origin;
	word	width;
	word	height;
	byte	pixel_size;
	byte	attributes;
}loadtga_t;

int get_file_size(FILE*p_file, bool reset = true) // path to file
{
	int old = ftell(p_file);
	fseek(p_file, 0, SEEK_END);
	int size = ftell(p_file);
	if (reset == true)fseek(p_file, 0, 0);
	else fseek(p_file, old, 0);
	return size;
}
int LoadTgaImage(char *szFileName, int &Index, int *SizeW, int *SizeH)
{
	FILE  *pFile;
	pFile = fopen(szFileName, "rb");
	if (pFile == NULL)
	{

		return -1;
	}

	int fileLength = get_file_size(pFile);

	byte *buffer = (byte *)malloc(fileLength);
	fread(buffer, fileLength, 1, pFile);


	int	i, columns, rows, row_inc, row, col;
	byte	*buf_p, *pixbuf, *targa_rgba;
	byte	palette[256][4], red = 0, green = 0, blue = 0, alpha = 0;
	int	readpixelcount, pixelcount;
	bool	compressed;
	loadtga_s	targa_header;

	buf_p = (byte *)buffer;
	targa_header.id_length = *buf_p++;
	targa_header.colormap_type = *buf_p++;
	targa_header.image_type = *buf_p++;

	targa_header.colormap_index = buf_p[0] + buf_p[1] * 256;		buf_p += 2;
	targa_header.colormap_length = buf_p[0] + buf_p[1] * 256;		buf_p += 2;
	targa_header.colormap_size = *buf_p;				buf_p += 1;
	targa_header.x_origin = *(short *)buf_p;			buf_p += 2;
	targa_header.y_origin = *(short *)buf_p;			buf_p += 2;
	targa_header.width = *(short *)buf_p;		buf_p += 2;
	targa_header.height = *(short *)buf_p;		buf_p += 2;
	targa_header.pixel_size = *buf_p++;
	targa_header.attributes = *buf_p++;
	if (targa_header.id_length != 0) buf_p += targa_header.id_length;	
	if (targa_header.image_type == 1 || targa_header.image_type == 9)
	{
		if (targa_header.pixel_size != 8)
		{
			return false;
		}
		if (targa_header.colormap_length != 256)
		{
			return false;
		}
		if (targa_header.colormap_index)
		{
			return false;
		}
		if (targa_header.colormap_size == 24)
		{
			for (i = 0; i < targa_header.colormap_length; i++)
			{
				palette[i][2] = *buf_p++;
				palette[i][1] = *buf_p++;
				palette[i][0] = *buf_p++;
				palette[i][3] = 255;
			}
		}
		else if (targa_header.colormap_size == 32)
		{
			for (i = 0; i < targa_header.colormap_length; i++)
			{
				palette[i][2] = *buf_p++;
				palette[i][1] = *buf_p++;
				palette[i][0] = *buf_p++;
				palette[i][3] = *buf_p++;
			}
		}
		else
		{
			return false;
		}
	}
	else if (targa_header.image_type == 2 || targa_header.image_type == 10)
	{
		
		if (targa_header.pixel_size != 32 && targa_header.pixel_size != 24)
		{
			return false;
		}
	}
	else if (targa_header.image_type == 3 || targa_header.image_type == 11)
	{
		
		if (targa_header.pixel_size != 8)
		{
			return false;
		}
	}

	columns = targa_header.width;
	rows = targa_header.height;

	int size = columns * rows * 4;
	targa_rgba = (byte *)malloc(size);

	pixbuf = targa_rgba + (rows - 1) * columns * 4;
	row_inc = -columns * 4 * 2;

	compressed = (targa_header.image_type == 9 || targa_header.image_type == 10 || targa_header.image_type == 11);
	for (row = col = 0; row < rows; )
	{
		pixelcount = 0x10000;
		readpixelcount = 0x10000;

		if (compressed)
		{
			pixelcount = *buf_p++;
			if (pixelcount & 0x80)  
				readpixelcount = 1;
			pixelcount = 1 + (pixelcount & 0x7f);
		}

		while (pixelcount-- && (row < rows))
		{
			if (readpixelcount-- > 0)
			{
				switch (targa_header.image_type)
				{
				case 1:
				case 9:
				
					blue = *buf_p++;
					red = palette[blue][0];
					green = palette[blue][1];
					alpha = palette[blue][3];
					blue = palette[blue][2];
					//if( alpha != 255 ) image.flags |= IMAGE_HAS_ALPHA;
					break;
				case 2:
				case 10:
					// 24 or 32 bit image
					blue = *buf_p++;
					green = *buf_p++;
					red = *buf_p++;
					alpha = 255;
					if (targa_header.pixel_size == 32)
					{
						alpha = *buf_p++;
						//if( alpha != 255 )
						//image.flags |= IMAGE_HAS_ALPHA;
					}
					break;
				case 3:
				case 11:
					// greyscale image
					blue = green = red = *buf_p++;
					alpha = 255;
					break;
				}
			}


			*pixbuf++ = red;
			*pixbuf++ = green;
			*pixbuf++ = blue;
			*pixbuf++ = alpha;
			if (++col == columns)
			{
				row++;
				col = 0;
				pixbuf += row_inc;
			}
		}
	}

	// Make a texture


	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Index);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, targa_header.width, targa_header.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, targa_rgba);
	free(targa_rgba);
	free(buffer);
	fclose(pFile);

	*SizeW = targa_header.width;
	*SizeH = targa_header.height;
	//TextSize
	return 1;
}
