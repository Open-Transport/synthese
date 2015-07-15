
/** DrawableLine class header.
	@file DrawableLine.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_CARTO_DRAWABLELINE_H
#define SYNTHESE_CARTO_DRAWABLELINE_H

#include <string>
#include <vector>

#include "UtilTypes.h"
#include "Drawable.h"
#include "RGBColor.h"

#include <boost/shared_ptr.hpp>

namespace geos
{
	namespace geom
	{
		class LineString;
		class Coordinate;
	}
}
namespace synthese
{
	namespace pt
	{
		class JourneyPattern;
	}


	namespace map
	{

		class DrawableLineIndex;

		/** @ingroup m39 */
		class DrawableLine
		{
		private:


			const util::RegistryKeyType _lineId;

			boost::shared_ptr<geos::geom::LineString> _geometry;  //!< Reference line points
			const std::string _shortName;
			const synthese::util::RGBColor _color;
			const bool _withPhysicalStops;

			std::vector<int> _shifts;
			std::vector<bool> _shifted;

			std::vector<geos::geom::Coordinate> _fuzzyfiedPoints;
			mutable std::vector<geos::geom::Coordinate> _shiftedPoints;




		public:

			typedef enum { NONE, SINGLE, SQL_DOUBLE } PointShiftingMode;

			DrawableLine (const synthese::pt::JourneyPattern* line,
				  int fromLineStopIndex,
				  int toLineStopIndex,
				  bool withPhysicalStops = true);


			DrawableLine (const util::RegistryKeyType& lineId,
				boost::shared_ptr<geos::geom::LineString> points,
				  const std::string& shortName,
				  const synthese::util::RGBColor& color,
				  bool withPhysicalStops = true);

			virtual ~DrawableLine();

			//! @name Getters/Setters
			//@{
			const util::RegistryKeyType& getLineId () const;

			const std::string& getShortName () const;
			const synthese::util::RGBColor& getColor () const;
			bool getWithPhysicalStops () const;

			boost::shared_ptr<geos::geom::LineString> getPoints() const { return _geometry; }

			const std::vector<geos::geom::Coordinate>&
			getFuzzyfiedPoints () const;


			bool hasPoint (const geos::geom::Coordinate&) const;

			bool isStopPoint (int pointIndex) const;
			bool isViaPoint (int pointIndex) const;

			int isShifted (int pointIndex) const;
			int getShift (int pointIndex) const;
			void setShift (int pointIndex, int shift);

			const std::vector<geos::geom::Coordinate>& getShiftedPoints () const;

			//@}

			void fuzzyfyPoints (const DrawableLineIndex& lineIndex);

			bool isFullyReverseWay (const DrawableLine* dbl) const;
			bool isFullySameWay (const DrawableLine* dbl) const;
			bool isReverseWayAt (const geos::geom::Coordinate& p, const DrawableLine* dbl) const;
			std::size_t numberOfCommonPointsWith (const DrawableLine* dbl) const;

			virtual void prepare (Map& map, double spacing, PointShiftingMode shiftMode = SQL_DOUBLE) const;

			/** Find first point in this line points
			which is equal (by value) to a given point.
			*/
			int firstIndexOf (const geos::geom::Coordinate& p) const;

			const std::vector<geos::geom::Coordinate>
				calculateShiftedPoints (const std::vector<geos::geom::Coordinate>& points,
						double spacing,
						PointShiftingMode shiftMode) const;

			const std::vector<geos::geom::Coordinate>
			calculateAbsoluteShiftedPoints (const std::vector<geos::geom::Coordinate>& points,
												 double spacing) const;

		private:


			geos::geom::Coordinate
			calculateSingleShiftedPoint (geos::geom::Coordinate a,
							 geos::geom::Coordinate b,
							 double distance) const;

			geos::geom::Coordinate
			calculateSingleShiftedPoint (geos::geom::Coordinate a,
							 geos::geom::Coordinate b,
							 geos::geom::Coordinate c,
							 double distance) const;

			geos::geom::Coordinate calculateDoubleShiftedPoint(
				geos::geom::Coordinate a,
				geos::geom::Coordinate b,
				geos::geom::Coordinate c,
				double incomingDistance,
				double outgoingDistance
			) const;
		};
}	}

#endif

