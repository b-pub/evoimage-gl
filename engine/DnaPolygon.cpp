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
#include "DnaPolygon.h"
#include "Tools.h"
#include "Settings.h"
#include "DnaDrawing.h"

namespace ei
{
    DnaPolygon::DnaPolygon() : m_points(0), m_brush(0)
    {}

    DnaPolygon::~DnaPolygon()
    {
        setBrush(0);
        setPoints(0);
    }

    void DnaPolygon::init()
    {
        DnaPointList *points = new DnaPointList();
        DnaPoint *origin = new DnaPoint(0, 0);
        origin->init();

        for (int i=0; i < Settings::activePointsPerPolygonMin; i++)
        {
            int x, y;
            x = std::min(std::max(0, origin->x + Tools::getRandomNumber(-3, 3)), Tools::maxWidth);
            y = std::min(std::max(0, origin->y + Tools::getRandomNumber(-3, 3)), Tools::maxHeight);
            DnaPoint *point = new DnaPoint(x, y);
            points->push_back(point);
        }
        setPoints(points);

        DnaBrush *b = new DnaBrush(0,0,0,0);
        b->init();
        setBrush(b);
    }

    DnaPointList *DnaPolygon::points()
    { return m_points; }

    void DnaPolygon::setPoints(DnaPointList *points)
    {
        // clear old
        if (m_points)
        {
            DnaPointList::iterator iter;
            for (iter = m_points->begin(); iter != m_points->end(); iter++)
                delete *iter;
            delete m_points;
        }
        // save new
        m_points = points;
    }

    DnaBrush *DnaPolygon::brush()
    { return m_brush; }

    void DnaPolygon::setBrush(DnaBrush *brush)
    {
        delete m_brush;
        m_brush = brush;
    }

    DnaPolygon *DnaPolygon::clone()
    {
        DnaPolygon *poly = new DnaPolygon();
        DnaPointList *points = new DnaPointList();
        DnaPointList::iterator iter;
        for (iter = m_points->begin(); iter != m_points->end(); iter++)
        {
            points->push_back( (*iter)->clone() );
        }
        poly->setPoints(points);
        poly->setBrush( m_brush->clone() );
        return poly;
    }

    int DnaPolygon::pointCount()
    {
        return m_points ? m_points->size() : 0;
    }

    void DnaPolygon::mutate(DnaDrawing *drawing)
    {
        if (Tools::willMutate(Settings::activeAddPointMutationRate))
            addPoint(drawing);

        if (Tools::willMutate(Settings::activeRemovePointMutationRate))
            removePoint(drawing);

        m_brush->mutate(drawing);

        DnaPointList::iterator iter;
        for (iter = m_points->begin(); iter != m_points->end(); iter++)
            (*iter)->mutate(drawing);
    }

    void DnaPolygon::removePoint(DnaDrawing *drawing)
    {
        if (m_points->size() > Settings::activePointsPerPolygonMin &&
            drawing->pointCount() > Settings::activePointsMin)
        {
            int index = Tools::getRandomNumber(0, m_points->size()-1);
            delete (*m_points)[index];
            m_points->erase(m_points->begin() + index);
            drawing->setDirty();
        }
    }

    void DnaPolygon::addPoint(DnaDrawing *drawing)
    {
        if (m_points->size() < Settings::activePointsPerPolygonMax &&
            drawing->pointCount() < Settings::activePointsMax)
        {
            int index = Tools::getRandomNumber(1, m_points->size()-1);
            DnaPoint
                *prev = (*m_points)[index-1],
                *next = (*m_points)[index];
            DnaPoint
                *point = new DnaPoint( (prev->x + next->x)/2, (prev->y + next->y)/2 );

            m_points->insert( m_points->begin() + index, point);
            drawing->setDirty();
        }
    }
}
