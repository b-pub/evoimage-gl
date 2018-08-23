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
#pragma once

#include "DnaPolygon.h"

namespace ei
{
    class DnaDrawing
    {
      protected:
        DnaPolygonList *m_polygons; // property
        bool            m_dirty;

      public:
        DnaDrawing();
        ~DnaDrawing();

        void init();

        // polygons property
        DnaPolygonList* polygons();
        void setPolygons(DnaPolygonList *polygons);

        bool dirty();
        void setDirty();

        int pointCount();

        DnaDrawing* clone();

        // mutation methods
        void mutate();
        void movePolygon();
        void removePolygon();
        void addPolygon();
    };

}

