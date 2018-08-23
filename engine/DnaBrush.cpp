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
#include "DnaBrush.h"
#include "Tools.h"
#include "Settings.h"
#include "DnaDrawing.h"

namespace ei
{
    DnaBrush::DnaBrush(int R, int G, int B, int A) : r(R), g(G), b(B), a(A)
    { }

    DnaBrush::~DnaBrush()
    { }

    void DnaBrush::init()
    {
        r = Tools::getRandomNumber(0, 255);
        g = Tools::getRandomNumber(0, 255);
        b = Tools::getRandomNumber(0, 255);
        a = Tools::getRandomNumber(0, 255);
    }

    DnaBrush *DnaBrush::clone()
    {
        return new DnaBrush(r,g,b,a);
    }

    void DnaBrush::mutate(DnaDrawing *drawing)
    {
        if (Tools::willMutate(Settings::activeRedMutationRate))
        {
            r = Tools::getRandomNumber(Settings::activeRedRangeMin, Settings::activeRedRangeMax);
            drawing->setDirty();
        }

        if (Tools::willMutate(Settings::activeGreenMutationRate))
        {
            g = Tools::getRandomNumber(Settings::activeGreenRangeMin, Settings::activeGreenRangeMax);
            drawing->setDirty();
        }

        if (Tools::willMutate(Settings::activeBlueMutationRate))
        {
            b = Tools::getRandomNumber(Settings::activeBlueRangeMin, Settings::activeBlueRangeMax);
            drawing->setDirty();
        }

        if (Tools::willMutate(Settings::activeAlphaMutationRate))
        {
            a = Tools::getRandomNumber(Settings::activeAlphaRangeMin, Settings::activeAlphaRangeMax);
            drawing->setDirty();
        }
    }
}
