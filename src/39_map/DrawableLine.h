
/** DrawableLine class header.
	@file DrawableLine.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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
#include "Point2D.h"

namespace synthese
{
	namespace pt
	{
		class JourneyPattern;
	}


	namespace map
	{

		class DrawableLineIndex;

		/** @ingroup m59 */
		class DrawableLine 
		{
		private:


			const util::RegistryKeyType _lineId;

			std::vector<const geometry::Point2D*> _points;  //!< Reference line points
			const std::string _shortName;
			const synthese::util::RGBColor _color;
			const bool _withPhysicalStops;

			std::vector<int> _shifts;
			std::vector<bool> _shifted;
			
			std::vector<geometry::Point2D> _fuzzyfiedPoints;
			mutable std::vector<geometry::Point2D> _shiftedPoints;

			


		public:

			typedef enum { NONE, SINGLE, SQL_DOUBLE } PointShiftingMode;
		    
			DrawableLine (const synthese::pt::JourneyPattern* line,
				  int fromLineStopIndex,
				  int toLineStopIndex,
				  bool withPhysicalStops = true);


			DrawableLine (const util::RegistryKeyType& lineId,
				  const std::vector<const geometry::Point2D*>& points,
				  const std::string& shortName,
				  const synthese::util::RGBColor& color,
				  bool withPhysicalStops = true);

			~DrawableLine();

			//! @name Getters/Setters
			//@{
			const util::RegistryKeyType& getLineId () const;

			const std::string& getShortName () const;
			const synthese::util::RGBColor& getColor () const;
			bool getWithPhysicalStops () const;

			const std::vector<const geometry::Point2D*>& 
			getPoints () const;

			const std::vector<geometry::Point2D>& 
			getFuzzyfiedPoints () const;


			bool hasPoint (const geometry::Point2D&) const;

			bool isStopPoint (int pointIndex) const;
			bool isViaPoint (int pointIndex) const;

			int isShifted (int pointIndex) const;
			int getShift (int pointIndex) const;
			void setShift (int pointIndex, int shift);

			const std::vector<geometry::Point2D>& getShiftedPoints () const;

			//@}

			void fuzzyfyPoints (const DrawableLineIndex& lineIndex);

			bool isFullyReverseWay (const DrawableLine* dbl) const;
			bool isFullySameWay (const DrawableLine* dbl) const;
			bool isReverseWayAt (const geometry::Point2D& p, const DrawableLine* dbl) const;
			int numberOfCommonPointsWith (const DrawableLine* dbl) const;

			virtual void prepare (Map& map, double spacing, PointShiftingMode shiftMode = SQL_DOUBLE) const;
		    
			/** Find first point in this line points
			which is equal (by value) to a given point.
			*/
			int firstIndexOf (const geometry::Point2D& p) const;

			const std::vector<geometry::Point2D> 
			calculateShiftedPoints (const std::vector<geometry::Point2D>& points, 
						double spacing, 
						PointShiftingMode shiftMode) const;

			const std::vector<geometry::Point2D>
			calculateAbsoluteShiftedPoints (const std::vector<geometry::Point2D>& points, 
												 double spacing) const;

		private:


			geometry::Point2D  
			calculateSingleShiftedPoint (geometry::Point2D a, 
							 geometry::Point2D b, 
							 double distance) const;

			geometry::Point2D  
			calculateSingleShiftedPoint (geometry::Point2D a, 
							 geometry::Point2D b, 
							 geometry::Point2D c, 
							 double distance) const;
			geometry::Point2D  
			calculateDoubleShiftedPoint (geometry::Point2D a, 
							 geometry::Point2D b, 
							 geometry::Point2D c, 
							 double incomingDistance, 
							 double outgoingDistance) const;
		    
		    
		    
		};
	 
	}
}

#endif

