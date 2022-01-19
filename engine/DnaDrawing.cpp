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
    DnaDrawing::DnaDrawing()
        : m_dirty(true)
    {
        init();
    }

    void DnaDrawing::init()
    {
        m_polygons.clear();
        for (int i=0; i < Settings::activePolygonsMin; i++)
            addPolygon();
        setDirty();
    }

    DnaPolygonList& DnaDrawing::polygons()
    { return m_polygons; }

    void DnaDrawing::setPolygons(DnaPolygonList const &polygons)
    {
        m_polygons.clear();
        m_polygons = polygons;
    }

    bool DnaDrawing::dirty()
    { return m_dirty; }

    void DnaDrawing::setDirty()
    { m_dirty = true; }

    int DnaDrawing::pointCount()
    {
        int sum = 0;

        // iterate over polygons, get size of each.
        DnaPolygonList::iterator iter;
        for (iter = m_polygons.begin(); iter != m_polygons.end(); iter++)
        {
            sum += iter->pointCount();
        }
        return sum;
    }

    DnaDrawing* DnaDrawing::clone()
    {
        DnaDrawing *dd = new DnaDrawing();
        dd->m_polygons = m_polygons;
        return dd;
    }

    void DnaDrawing::mutate()
    {
        if (Tools::willMutate(Settings::activeAddPolygonMutationRate))
        {
            addPolygon();
        }

        if (Tools::willMutate(Settings::activeRemovePolygonMutationRate))
        {
            removePolygon();
        }

        if (Tools::willMutate(Settings::activeMovePolygonMutationRate))
        {
            movePolygon();
        }

        DnaPolygonList::iterator iter;
        for (iter = m_polygons.begin(); iter != m_polygons.end(); iter++)
        {
            iter->mutate(*this);
        }
    }

    void DnaDrawing::addPolygon()
    {
        if (m_polygons.size() < Settings::activePolygonsMax)
        {
            DnaPolygon poly;
            if (m_polygons.size() > 2)
            {
                int index = Tools::getRandomNumber(0, m_polygons.size()-1);
                m_polygons.insert(m_polygons.begin() + index, poly);
            }
            else
            {
                m_polygons.push_back(poly);
            }
            setDirty();
        }
    }

    void DnaDrawing::removePolygon()
    {
        if (m_polygons.size() > Settings::activePolygonsMin)
        {
            int index = Tools::getRandomNumber(0, m_polygons.size()-1);
            m_polygons.erase(m_polygons.begin() + index);
            setDirty();
        }
    }

    void DnaDrawing::movePolygon()
    {
        if (m_polygons.size() < 2)
            return;

        // Move a polygon = change the drawing order of two polygons
        int a = Tools::getRandomNumber(0, m_polygons.size()-1),
            b = Tools::getRandomNumber(0, m_polygons.size()-1);
        if (a != b) {
            std::swap(m_polygons[a], m_polygons[b]);
            setDirty();
        }
    }
}
