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
#include "DnaPoint.h"
#include "Tools.h"
#include "Settings.h"
#include "DnaDrawing.h"

namespace ei
{
    DnaPoint::DnaPoint(int X, int Y) : x(X), y(Y)
    { }

    DnaPoint::~DnaPoint()
    { }

    void DnaPoint::init()
    {
        x = Tools::getRandomNumber(0, Tools::maxWidth);
        y = Tools::getRandomNumber(0, Tools::maxHeight);
    }

    DnaPoint *DnaPoint::clone()
    {
        return new DnaPoint(x, y);
    }

    void DnaPoint::mutate(DnaDrawing *drawing)
    {
        if (Tools::willMutate(Settings::activeMovePointMaxMutationRate))
        {
            x = Tools::getRandomNumber(0, Tools::maxWidth);
            y = Tools::getRandomNumber(0, Tools::maxHeight);
            drawing->setDirty();
        }

        if (Tools::willMutate(Settings::activeMovePointMidMutationRate))
        {
            x = std::min( std::max(0,
                                   x + Tools::getRandomNumber(-Settings::activeMovePointRangeMid,
                                                              Settings::activeMovePointRangeMid)),
                          Tools::maxWidth);
            y = std::min( std::max(0,
                                   y + Tools::getRandomNumber(-Settings::activeMovePointRangeMid,
                                                              Settings::activeMovePointRangeMid)),
                          Tools::maxHeight);
            drawing->setDirty();
        }

        if (Tools::willMutate(Settings::activeMovePointMinMutationRate))
        {
            x = std::min( std::max(0,
                                   x + Tools::getRandomNumber(-Settings::activeMovePointRangeMin,
                                                              Settings::activeMovePointRangeMin)),
                          Tools::maxWidth);
            y = std::min( std::max(0,
                                   y + Tools::getRandomNumber(-Settings::activeMovePointRangeMin,
                                                              Settings::activeMovePointRangeMin)),
                          Tools::maxHeight);
            drawing->setDirty();
        }
    }
}
