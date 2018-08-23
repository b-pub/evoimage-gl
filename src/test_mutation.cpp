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
#include <iostream>
#include "Settings.h"
#include "DnaDrawing.h"

int main(int argc, char *argv[])
{
    std::cout << "Initializing settings" << std::endl;
    ei::Settings settings;
    settings.activate();

    std::cout << "Creating a drawing" << std::endl;
    ei::DnaDrawing *drawing = new ei::DnaDrawing();
    drawing->init();

    // do stuff
    std::cout << "Drawing has " << drawing->pointCount() << " points in "
              << drawing->polygons()->size() << " polygons" << std::endl;
    std::cout << "Cloning drawing into d2" << std::endl;
    ei::DnaDrawing *d2 = drawing->clone();
    std::cout << "Drawing2 has " << d2->pointCount() << " points" << std::endl;

    std::cout << "Mutating d2 100,000 times..." << std::endl;
    for (int i=0; i<100000; i++)
    {
        d2->mutate();
    }
    std::cout << "Drawing2 has " << d2->pointCount() << " points in "
              << d2->polygons()->size() << " polygons" << std::endl;

    std::cout << "Cleaning up" << std::endl;
    delete drawing;
    delete d2;

    return 0;
}
