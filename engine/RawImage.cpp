/*
 *  Evoimage-gl, a library and program to evolve images
 *  Copyright (C) 2009 Brent Burton
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <stdio.h>
#include <setjmp.h>
#include <png.h>
#include "RawImage.h"

#ifndef png_infopp_NULL
#define png_infopp_NULL (png_infopp)NULL
#endif

#ifndef png_bytepp_NULL
#define png_bytepp_NULL (png_bytepp)NULL
#endif

RawImage::RawImage(int width, int height) : m_width(width), m_height(height), m_invertRows(false)
{
    m_pixels = new ubyte[width * height * 3];
}

RawImage::~RawImage()
{
    delete[] m_pixels;
}

pixel RawImage::getPixel(int x, int y)
{
    ubyte *b = m_pixels + 3 * (x + m_width * y);
    return (b[0] << 16) + (b[1] << 8) + b[2];
}

ubyte *RawImage::getPixels()
{
    return m_pixels;
}

ubyte *RawImage::getPixelAt(int x, int y)
{
    return m_pixels + 3 * (x + m_width * y);
}

int RawImage::width()
{ return m_width; }

int RawImage::height()
{ return m_height; }

int RawImage::readFromPng(const char *filename)
{
    FILE *infp = 0;
    png_structp png_ptr;
    png_infop info_ptr;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
        return 0;

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return 0;
    }

    infp = fopen(filename, "rb");
    if (infp == NULL)
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return 0;
    }
    png_init_io(png_ptr, infp);

    // read it
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 &interlace_type, NULL, NULL);

    if (width != m_width   ||
        height != m_height ||
        bit_depth != 8     ||
        color_type != PNG_COLOR_TYPE_RGB)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(infp);
        return 0;
    }

    // image matches what we expect
    ubyte **rows = new ubyte*[height];
    for (int row=0; row < height; row++)
    {
        rows[row] = getPixelAt(0, row);
        png_read_rows(png_ptr, &rows[row], png_bytepp_NULL, 1);
    }

    png_read_end(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(infp);
    delete[] rows;

    return 1;
}

void RawImage::invertRowsOnWrite(bool flag)
{
    m_invertRows = flag;
}

int RawImage::writeToPng(const char *filename)
{
    png_structp png_ptr = 0;
    png_infop   info_ptr = 0;
    FILE       *outfp = 0;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                      (png_voidp)NULL, NULL, NULL);
    if (!png_ptr)
    {
        return 0;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        png_destroy_write_struct(&png_ptr, NULL);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 0;
    }

    outfp = fopen(filename, "wb");
    if (outfp == NULL)
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 0;
    }
    png_init_io(png_ptr, outfp);

    // write it
    png_set_IHDR(png_ptr, info_ptr, m_width, m_height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_ptr, info_ptr);

    // image matches what we expect
    if (m_invertRows)                       // inverted n-1 .. 0 order
    {
        for (int row=m_height-1; row >= 0; row--)
            png_write_row(png_ptr, getPixelAt(0, row));
    }
    else                                    // normal 0 .. n-1 order
    {
        for (int row=0; row < m_height; row++)
            png_write_row(png_ptr, getPixelAt(0, row));
    }

    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(outfp);

    return 1;
}
