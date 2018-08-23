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
/*
 * RawImage
 * An representation of a 24-bit RGB bitmap with
 * slow, convenient accessors, as well as fast accessibility.
 * 28 December 2008
 */
#pragma once

typedef unsigned char ubyte;
typedef unsigned int pixel;

class RawImage
{
  private:
    int m_width;
    int m_height;
    ubyte *m_pixels;                        // RGB values stored row-major, top to bottom.
    bool m_invertRows;

  public:
    RawImage(int width, int height);
    virtual ~RawImage();

    int width();
    int height();

    pixel getPixel(int x, int y); // Fetch RGB at (x,y): R:(23:16), G:(15:8) B:(7:0)
    ubyte *getPixels();
    ubyte *getPixelAt(int x, int y);

    int readFromPng(const char *filename);
    int writeToPng(const char *filename);
    void invertRowsOnWrite(bool flag);      // write rows from (n-1) to (0) if flag==true.
};
