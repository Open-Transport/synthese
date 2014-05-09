
/** LineStop class header.
	@file LineStop.h

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

#ifndef SYNTHESE_ENV_LINESTOP_H
#define SYNTHESE_ENV_LINESTOP_H

#include "SchemaMacros.hpp"
#include "NumericField.hpp"
#include "Object.hpp"
#include "PointerField.hpp"
#include "GeometryField.hpp"
#include "JourneyPattern.hpp"

#include <geos/geom/LineString.h>

namespace synthese
{
	namespace pt
	{
		class StopPoint;
	}

	FIELD_POINTER(LineNode, util::Registrable)
	FIELD_POINTER(Line, pt::JourneyPattern)
	FIELD_SIZE_T(RankInPath)
	FIELD_BOOL(IsDeparture)
	FIELD_BOOL(IsArrival)
	FIELD_DOUBLE(MetricOffsetField)
	FIELD_BOOL(ScheduleInput)
	FIELD_BOOL(InternalService)
	FIELD_BOOL(ReservationNeeded)
	FIELD_BOOL(ReverseDRTArea)

	typedef boost::fusion::map<
		FIELD(Key),
		FIELD(LineNode),
		FIELD(Line),
		FIELD(RankInPath),
		FIELD(IsDeparture),
		FIELD(IsArrival),
		FIELD(MetricOffsetField),
		FIELD(ScheduleInput),
		FIELD(InternalService),
		FIELD(ReservationNeeded),
		FIELD(ReverseDRTArea),
		FIELD(LineStringGeometry)
	> LineStopSchema;

	namespace pt
	{
		class LinePhysicalStop;

		/** Association class between line and physical stop.
			The linestop is the implementation of the edge of a transport line.
			TRIDENT = PtLink
			@ingroup m35
		*/
		class LineStop:
			public Object<LineStop, LineStopSchema>,
			virtual public util::Registrable
		{
		public:
			typedef std::vector<boost::shared_ptr<LinePhysicalStop> > GeneratedLineStops;

		private:
			mutable GeneratedLineStops _generatedLineStops;

			void _generateDRTAreaAllStops(
				bool isForArrival
			) const;

		public:

			LineStop(
				util::RegistryKeyType id = 0,
				JourneyPattern* line = NULL,
				std::size_t rankInPath = 0,
				bool isDeparture = true,
				bool isArrival = true,
				double metricOffset = 0,
				boost::optional<util::Registrable&> node = boost::optional<util::Registrable&>()
			);

			~LineStop();


			//! @name Getters
			//@{
				const GeneratedLineStops& getGeneratedLineStops() const { return _generatedLineStops; }
			//@}

			//!	@name Setters
			//@{
			//@}


			//! @name Services
			//@{
				/*! Estimates consistency of line stops sequence according to
					metric offsets and physical stops coordinates.
					@param other Other line stop to compare.
					@return true if data seems consistent, false otherwise.
				*/
//				bool seemsGeographicallyConsistent (const LineStop& other) const;
			//@}


			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
		};
}	}

#endif
