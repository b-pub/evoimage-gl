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
#include <algorithm>
#include "DnaDrawing.h"
#include "Settings.h"
#include "Tools.h"

namespace ei
{
    DnaDrawing::DnaDrawing() : m_polygons(0), m_dirty(true)
    {
    }

    DnaDrawing::~DnaDrawing()
    {
        setPolygons(0);
    }

    void DnaDrawing::init()
    {
        setPolygons(new DnaPolygonList());
        for (int i=0; i < Settings::activePolygonsMin; i++)
            addPolygon();
        setDirty();
    }

    DnaPolygonList* DnaDrawing::polygons()
    { return m_polygons; }

    void DnaDrawing::setPolygons(DnaPolygonList *polygons)
    {
        // free old
        if (m_polygons)
        {
            DnaPolygonList::iterator iter;
            for (iter = m_polygons->begin(); iter != m_polygons->end(); iter++)
            {
                delete *iter;
            }
            delete m_polygons;
        }
        // save new
        m_polygons = polygons;
    }

    bool DnaDrawing::dirty()
    { return m_dirty; }

    void DnaDrawing::setDirty()
    { m_dirty = true; }

    int DnaDrawing::pointCount()
    {
        int sum = 0;

        if (0 == m_polygons)
            return sum;

        // iterate over polygons, get size of each.
        DnaPolygonList::iterator iter;
        for (iter = m_polygons->begin(); iter != m_polygons->end(); iter++)
        {
            sum += (*iter)->pointCount();
        }
        return sum;
    }

    DnaDrawing* DnaDrawing::clone()
    {
        DnaDrawing *dd = new DnaDrawing();

        if (0 == m_polygons)
            return dd;

        DnaPolygonList *newplist = new DnaPolygonList();

        DnaPolygonList::iterator iter;
        for (iter = m_polygons->begin(); iter != m_polygons->end(); iter++)
        {
            newplist->push_back( (*iter)->clone() );
        }

        dd->setPolygons(newplist);
        return dd;
    }

    void DnaDrawing::mutate()
    {
        if (Tools::willMutate(Settings::activeAddPolygonMutationRate))
            addPolygon();

        if (Tools::willMutate(Settings::activeRemovePolygonMutationRate))
            removePolygon();

        if (Tools::willMutate(Settings::activeMovePolygonMutationRate))
            movePolygon();

        DnaPolygonList::iterator iter;
        for (iter = m_polygons->begin(); iter != m_polygons->end(); iter++)
        {
            (*iter)->mutate(this);
        }
    }

    void DnaDrawing::addPolygon()
    {
        if (0 == m_polygons)
            return;

        if (m_polygons->size() < Settings::activePolygonsMax)
        {
            DnaPolygon *poly = new DnaPolygon();
            poly->init();
            if (m_polygons->size() > 0)
            {
                int index = Tools::getRandomNumber(0, m_polygons->size()-1);
                m_polygons->insert(m_polygons->begin() + index, poly);
            }
            else
            {
                m_polygons->push_back(poly);
            }
            setDirty();
        }
    }

    void DnaDrawing::removePolygon()
    {
        if (0 == m_polygons)
            return;

        if (m_polygons->size() > Settings::activePolygonsMin)
        {
            int index = Tools::getRandomNumber(0, m_polygons->size()-1);
            DnaPolygon *poly = (*m_polygons)[index];
            m_polygons->erase(m_polygons->begin() + index);
            delete poly;
            setDirty();
        }
    }

    void DnaDrawing::movePolygon()
    {
        if (0 == m_polygons ||
            m_polygons->size() < 2)
            return;

        // Move a polygon = change the drawing order of one polygon
        int a = Tools::getRandomNumber(0, m_polygons->size()-1),
            b = Tools::getRandomNumber(0, m_polygons->size()-1);
        std::swap((*m_polygons)[a], (*m_polygons)[b]);
        setDirty();
    }
}
