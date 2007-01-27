
/** LineStop class header.
	@file LineStop.h

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

#ifndef SYNTHESE_ENV_LINESTOP_H
#define SYNTHESE_ENV_LINESTOP_H


#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "04_time/DateTime.h"
#include "04_time/Schedule.h"

#include "15_env/Edge.h"

namespace synthese
{
	namespace env

	{

		class PhysicalStop;
		class Line;



		/** Association class between line and physical stop.
		@ingroup m15
		*/
		class LineStop : 
			public synthese::util::Registrable<uid,LineStop>, public Edge
		{
		public:
		    
		private:

			const PhysicalStop*  _physicalStop;   //!< Physical stop

			double _metricOffset;      //!< Metric offset of stop on line


		public:


			LineStop (const uid& id,
				const Line* line,
				int rankInPath,
				bool isDeparture,
				bool isArrival,
				double metricOffset,
				const PhysicalStop* physicalStop);

			~LineStop();


			//! @name Getters/Setters
			//@{
				const Vertex* getFromVertex () const;


				double getMetricOffset () const;
				void setMetricOffset (double metricOffset);

				Line*			getLine() const;
				const PhysicalStop*	getPhysicalStop() const;
			//@}


			//! @name Query methods
			//@{

				/*! Estimates consistency of line stops sequence according to 
					metric offsets and physical stops coordinates.
					@param other Other line stop to compare.
					@return true if data seems consistent, false otherwise.
				*/
				bool seemsGeographicallyConsistent (const LineStop& other) const;


			//@}


			//! @name Update methods
			//@{
		 
		    //@}
		};
	}
}

#endif
