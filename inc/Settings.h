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

namespace ei
{
    class Settings
    {
      public:
        static int activeAddPointMutationRate;
        static int activeAddPolygonMutationRate;
        static int activeAlphaMutationRate;
        static int activeAlphaRangeMax;
        static int activeAlphaRangeMin;
        static int activeBlueMutationRate;
        static int activeBlueRangeMax;
        static int activeBlueRangeMin;
        static int activeGreenMutationRate;
        static int activeGreenRangeMax;
        static int activeGreenRangeMin;
        static int activeMovePointMaxMutationRate;
        static int activeMovePointMidMutationRate;
        static int activeMovePointMinMutationRate;
        static int activeMovePointRangeMid;
        static int activeMovePointRangeMin;
        static int activeMovePolygonMutationRate;
        static int activePointsMax;
        static int activePointsMin;
        static int activePointsPerPolygonMax;
        static int activePointsPerPolygonMin;
        static int activePolygonsMax;
        static int activePolygonsMin;
        static int activeRedMutationRate;
        static int activeRedRangeMax;
        static int activeRedRangeMin;
        static int activeRemovePointMutationRate;
        static int activeRemovePolygonMutationRate;

        //Mutation rates

#define DECLARE_PROPERTY(T, G, S) \
        private: T m_##G;\
        public: T G(); void S(T value);

        DECLARE_PROPERTY(int, addPointMutationRate, setAddPointMutationRate);
        DECLARE_PROPERTY(int, addPolygonMutationRate, setAddPolygonMutationRate);
        DECLARE_PROPERTY(int, alphaMutationRate, setAlphaMutationRate);
        DECLARE_PROPERTY(int, alphaRangeMax, setAlphaRangeMax);
        DECLARE_PROPERTY(int, alphaRangeMin, setAlphaRangeMin);
        DECLARE_PROPERTY(int, blueMutationRate, setBlueMutationRate);
        DECLARE_PROPERTY(int, blueRangeMax, setBlueRangeMax);
        DECLARE_PROPERTY(int, blueRangeMin, setBlueRangeMin);
        DECLARE_PROPERTY(int, greenMutationRate, setGreenMutationRate);
        DECLARE_PROPERTY(int, greenRangeMax, setGreenRangeMax);
        DECLARE_PROPERTY(int, greenRangeMin, setGreenRangeMin);
        DECLARE_PROPERTY(int, movePointMaxMutationRate, setMovePointMaxMutationRate);
        DECLARE_PROPERTY(int, movePointMidMutationRate, setMovePointMidMutationRate);
        DECLARE_PROPERTY(int, movePointMinMutationRate, setMovePointMinMutationRate);
        DECLARE_PROPERTY(int, movePointRangeMid, setMovePointRangeMid);
        DECLARE_PROPERTY(int, movePointRangeMin, setMovePointRangeMin);
        DECLARE_PROPERTY(int, movePolygonMutationRate, setMovePolygonMutationRate);
        DECLARE_PROPERTY(int, pointsMax, setPointsMax);
        DECLARE_PROPERTY(int, pointsMin, setPointsMin);
        DECLARE_PROPERTY(int, pointsPerPolygonMax, setPointsPerPolygonMax);
        DECLARE_PROPERTY(int, pointsPerPolygonMin, setPointsPerPolygonMin);
        DECLARE_PROPERTY(int, polygonsMax, setPolygonsMax);
        DECLARE_PROPERTY(int, polygonsMin, setPolygonsMin);
        DECLARE_PROPERTY(int, redMutationRate, setRedMutationRate);
        DECLARE_PROPERTY(int, redRangeMax, setRedRangeMax);
        DECLARE_PROPERTY(int, redRangeMin, setRedRangeMin);
        DECLARE_PROPERTY(int, removePointMutationRate, setRemovePointMutationRate);
        DECLARE_PROPERTY(int, removePolygonMutationRate, setRemovePolygonMutationRate);

      public:
        Settings();
        void reset();
        void activate();
    };
}
