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

#include <vector>
#include "DnaPoint.h"
#include "DnaBrush.h"

namespace ei
{
    class DnaDrawing;

    class DnaPolygon
    {
      protected:
        DnaPointList m_points;
        DnaBrush     m_brush;

      public:
        DnaPolygon();
        ~DnaPolygon();

        void init();

        DnaPointList &points();
        void setPoints(DnaPointList const &points);

        DnaBrush &brush();
        void setBrush(DnaBrush const &Brush);

        DnaPolygon *clone();

        size_t pointCount();

        void mutate(DnaDrawing &drawing);
        void addPoint(DnaDrawing &drawing);
        void removePoint(DnaDrawing &drawing);
    };

    typedef std::vector<DnaPolygon> DnaPolygonList;
}
