
/** LineArea class header.
	@file LineArea.hpp

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

#ifndef SYNTHESE_pt_LineArea_hpp__
#define SYNTHESE_pt_LineArea_hpp__

#include "LineStop.h"

namespace synthese
{
	namespace pt
	{
		class JourneyPattern;
		class DRTArea;
		class AreaGeneratedLineStop;

		/** LineArea class.
			@ingroup m35
		*/
		class LineArea:
			public LineStop
		{
		public:
			typedef std::vector<boost::shared_ptr<AreaGeneratedLineStop> > GeneratedLineStops;


		private:
			GeneratedLineStops _generatedLineStops;
			bool _internalService;

		public:
			Edge::SubEdges getSubEdges() const;

			LineArea(
				util::RegistryKeyType id = 0,
				JourneyPattern* journeyPattern = NULL,
				std::size_t rankInPath = 0,
				bool isDeparture = true,
				bool isArrival = true,
				double metricOffset = 0,
				DRTArea* area = NULL,
				bool internalService = false
			);

			//! @name Getters
			//@{
				bool getInternalService() const { return _internalService; }
			//@}

			//! @name Setters
			//@{
				void setInternalService(bool value){ _internalService = value; }
			//@}

			//! @name Modifiers
			//@{
				void setArea(DRTArea& value);

				void clearArea();

				void addAllStops(
					bool isForArrival
				);
			//@}

			//! @name Services
			//@{
				DRTArea* getArea() const;

				virtual bool getScheduleInput() const { return true; }

				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;
			//@}
		};
}	}

#endif // SYNTHESE_pt_LineArea_hpp__
