
/** Junction class header.
	@file Junction.hpp

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

#ifndef SYNTHESE_pt_Junction_hpp__
#define SYNTHESE_pt_Junction_hpp__

#include "Object.hpp"

#include "FrameworkTypes.hpp"
#include "Path.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace pt
	{
		class StopPoint;

		FIELD_POINTER(StartPhysicalStop, StopPoint)
		FIELD_POINTER(EndPhysicalStop, StopPoint)
		FIELD_INT(Length)
		FIELD_INT(DurationMinutes)
		FIELD_BOOL(Bidirectional)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(StartPhysicalStop),
			FIELD(EndPhysicalStop),
			FIELD(Length),
			FIELD(DurationMinutes),
			FIELD(Bidirectional)
		> JunctionSchema;

		//////////////////////////////////////////////////////////////////////////
		/// Link between two stops allowing pedestrians to do a transfer outside of a StopArea.
		///	@ingroup m35
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.16
		///
		/// A junction corresponds to a fake public transportation line, supporting a PermanentService.
		class Junction:
			public Object<Junction, JunctionSchema>,
			public graph::Path
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<Junction>	Registry;

		private:
			boost::shared_ptr<Junction> _back;

			void _clean();

		public:
			Junction(
				util::RegistryKeyType id = 0
			);
			~Junction();

			//! @name Getters
			//@{
				boost::shared_ptr<Junction> getBack() const;
			//@}

			//! @name Services
			//@{
				virtual bool isActive(const boost::gregorian::date& date) const;
				virtual std::string getRuleUserName() const;
				StopPoint* getStart() const;
				StopPoint* getEnd() const;
				double getLength() const;
				boost::posix_time::time_duration getDuration() const;
				virtual bool isPedestrianMode() const;
				virtual bool isRoad() const;
				bool isValid() const;
			//@}

			//! @name Modifiers
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Builds the fake "pedestrian line" between the two stops.
				/// @param start Start physical stop
				/// @param end End physical stop
				/// @param length Length of the junction, in meters
				/// @param duration Duration of the junction for a pedestrian
				/// @param doBack Build the return junction too
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				void setStops(
					StopPoint* start,
					StopPoint* end,
					double length,
					boost::posix_time::time_duration duration,
					bool doBack,
					bool setDBValues = true
				);
			//@}

				
				
			virtual void toParametersMap(
				util::ParametersMap& pm,
				bool withAdditionalParameters,
				boost::logic::tribool withFiles = boost::logic::indeterminate,
				std::string prefix = std::string()
			) const;

			virtual SubObjects getSubObjects() const;

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;

		};
	}
}

#endif // SYNTHESE_pt_Junction_hpp__
