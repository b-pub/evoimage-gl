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
#include <stdlib.h>
#include "Tools.h"

namespace ei
{

    const int Tools::maxWidth    = 200;
    const int Tools::maxHeight   = 200;
    const int Tools::maxPolygons = 250;

    int Tools::getRandomNumber(int min, int max)
    {
        return min + (rand() % (max-min+1));
    }

    bool Tools::willMutate(int mutationRate)
    {
        // i.e., a (1 in mutationRate) chance
        return getRandomNumber(0, mutationRate) == 1;
    }
}
