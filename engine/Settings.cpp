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

namespace ei
{
    int Settings::activeAddPointMutationRate = 1500;
    int Settings::activeAddPolygonMutationRate = 700;
    int Settings::activeAlphaMutationRate = 1500;
    int Settings::activeAlphaRangeMax = 60;
    int Settings::activeAlphaRangeMin = 30;
    int Settings::activeBlueMutationRate = 1500;
    int Settings::activeBlueRangeMax = 255;
    int Settings::activeBlueRangeMin = 0;
    int Settings::activeGreenMutationRate = 1500;
    int Settings::activeGreenRangeMax = 255;
    int Settings::activeGreenRangeMin = 0;
    int Settings::activeMovePointMaxMutationRate = 1500;
    int Settings::activeMovePointMidMutationRate = 1500;
    int Settings::activeMovePointMinMutationRate = 1500;
    int Settings::activeMovePointRangeMid = 20;
    int Settings::activeMovePointRangeMin = 3;
    int Settings::activeMovePolygonMutationRate = 700;
    int Settings::activePointsMax = 1500;
    int Settings::activePointsMin = 3;
    int Settings::activePointsPerPolygonMax = 10;
    int Settings::activePointsPerPolygonMin = 3;
    int Settings::activePolygonsMax = 255;
    int Settings::activePolygonsMin = 2;
    int Settings::activeRedMutationRate = 1500;
    int Settings::activeRedRangeMax = 255;
    int Settings::activeRedRangeMin = 0;
    int Settings::activeRemovePointMutationRate = 1500;
    int Settings::activeRemovePolygonMutationRate = 1500;

    Settings::Settings()
    {
        reset();
    }

    void Settings::reset()
    {
        m_addPointMutationRate = 1500;
        m_addPolygonMutationRate = 700;
        m_alphaMutationRate = 1500;
        m_alphaRangeMax = 60;
        m_alphaRangeMin = 30;
        m_blueMutationRate = 1500;
        m_blueRangeMax = 255;
        m_blueRangeMin = 0;
        m_greenMutationRate = 1500;
        m_greenRangeMax = 255;
        m_greenRangeMin = 0;
        m_movePointMaxMutationRate = 1500;
        m_movePointMidMutationRate = 1500;
        m_movePointMinMutationRate = 1500;
        m_movePointRangeMid = 20;
        m_movePointRangeMin = 3;
        m_movePolygonMutationRate = 700;
        m_pointsMax = 1500;
        m_pointsMin = 3;
        m_pointsPerPolygonMax = 10;
        m_pointsPerPolygonMin = 3;
        m_polygonsMax = 255;
        m_polygonsMin = 2;
        m_redMutationRate = 1500;
        m_redRangeMax = 255;
        m_redRangeMin = 0;
        m_removePointMutationRate = 1500;
        m_removePolygonMutationRate = 1500;
    }

#define DEFINE_PROPERTY(T, G, S) \
    T Settings::G()\
    { return m_##G; }\
    void Settings::S(T value)\
    { m_##G = value; }

    DEFINE_PROPERTY(int, addPointMutationRate, setAddPointMutationRate)
    DEFINE_PROPERTY(int, addPolygonMutationRate, setAddPolygonMutationRate)
    DEFINE_PROPERTY(int, alphaMutationRate, setAlphaMutationRate)
    DEFINE_PROPERTY(int, alphaRangeMax, setAlphaRangeMax)
    DEFINE_PROPERTY(int, alphaRangeMin, setAlphaRangeMin)
    DEFINE_PROPERTY(int, blueMutationRate, setBlueMutationRate)
    DEFINE_PROPERTY(int, blueRangeMax, setBlueRangeMax)
    DEFINE_PROPERTY(int, blueRangeMin, setBlueRangeMin)
    DEFINE_PROPERTY(int, greenMutationRate, setGreenMutationRate)
    DEFINE_PROPERTY(int, greenRangeMax, setGreenRangeMax)
    DEFINE_PROPERTY(int, greenRangeMin, setGreenRangeMin)
    DEFINE_PROPERTY(int, movePointMaxMutationRate, setMovePointMaxMutationRate)
    DEFINE_PROPERTY(int, movePointMidMutationRate, setMovePointMidMutationRate)
    DEFINE_PROPERTY(int, movePointMinMutationRate, setMovePointMinMutationRate)
    DEFINE_PROPERTY(int, movePointRangeMid, setMovePointRangeMid)
    DEFINE_PROPERTY(int, movePointRangeMin, setMovePointRangeMin)
    DEFINE_PROPERTY(int, movePolygonMutationRate, setMovePolygonMutationRate)
    DEFINE_PROPERTY(int, pointsMax, setPointsMax)
    DEFINE_PROPERTY(int, pointsMin, setPointsMin)
    DEFINE_PROPERTY(int, pointsPerPolygonMax, setPointsPerPolygonMax)
    DEFINE_PROPERTY(int, pointsPerPolygonMin, setPointsPerPolygonMin)
    DEFINE_PROPERTY(int, polygonsMax, setPolygonsMax)
    DEFINE_PROPERTY(int, polygonsMin, setPolygonsMin)
    DEFINE_PROPERTY(int, redMutationRate, setRedMutationRate)
    DEFINE_PROPERTY(int, redRangeMax, setRedRangeMax)
    DEFINE_PROPERTY(int, redRangeMin, setRedRangeMin)
    DEFINE_PROPERTY(int, removePointMutationRate, setRemovePointMutationRate)
    DEFINE_PROPERTY(int, removePolygonMutationRate, setRemovePolygonMutationRate)

    void Settings::activate()
    {
        activeAddPointMutationRate = m_addPointMutationRate;
        activeAddPolygonMutationRate = m_addPolygonMutationRate;
        activeAlphaMutationRate = m_alphaMutationRate;
        activeAlphaRangeMax = m_alphaRangeMax;
        activeAlphaRangeMin = m_alphaRangeMin;
        activeBlueMutationRate = m_blueMutationRate;
        activeBlueRangeMax = m_blueRangeMax;
        activeBlueRangeMin = m_blueRangeMin;
        activeGreenMutationRate = m_greenMutationRate;
        activeGreenRangeMax = m_greenRangeMax;
        activeGreenRangeMin = m_greenRangeMin;
        activeMovePointMaxMutationRate = m_movePointMaxMutationRate;
        activeMovePointMidMutationRate = m_movePointMidMutationRate;
        activeMovePointMinMutationRate = m_movePointMinMutationRate;
        activeMovePointRangeMid = m_movePointRangeMid;
        activeMovePointRangeMin = m_movePointRangeMin;
        activeMovePolygonMutationRate = m_movePolygonMutationRate;
        activePointsMax = m_pointsMax;
        activePointsMin = m_pointsMin;
        activePointsPerPolygonMax = m_pointsPerPolygonMax;
        activePointsPerPolygonMin = m_pointsPerPolygonMin;
        activePolygonsMax = m_polygonsMax;
        activePolygonsMin = m_polygonsMin;
        activeRedMutationRate = m_redMutationRate;
        activeRedRangeMax = m_redRangeMax;
        activeRedRangeMin = m_redRangeMin;
        activeRemovePointMutationRate = m_removePointMutationRate;
        activeRemovePolygonMutationRate = m_removePolygonMutationRate;
    }
}
