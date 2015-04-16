
//////////////////////////////////////////////////////////////////////////
/// FreeDRTBookingAdmin class header.
///	@file FreeDRTBookingAdmin.hpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_FreeDRTBookingAdmin_H__
#define SYNTHESE_FreeDRTBookingAdmin_H__

#include "AdminInterfaceElementTemplate.h"
#include "BaseReservationActionAdmin.hpp"

#include "ResultHTMLTable.h"

#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace geography
	{
		class Place;
	}

	namespace pt
	{
		class FreeDRTArea;
	}

	namespace resa
	{
		//////////////////////////////////////////////////////////////////////////
		/// FreeDRTBookingAdmin Admin compound class.
		///	@ingroup m51Admin refAdmin
		///	@author Hugues Romain
		///	@date 2012
		class FreeDRTBookingAdmin:
			public admin::AdminInterfaceElementTemplate<FreeDRTBookingAdmin>,
			public BaseReservationActionAdmin
		{
		public:
			/// @name Parameter identifiers
			//@{
				static const std::string PARAMETER_AREA_ID;
				static const std::string PARAMETER_DEPARTURE_PLACE;
				static const std::string PARAMETER_ARRIVAL_PLACE;
				static const std::string PARAMETER_DATE;
				static const std::string PARAMETER_TIME;
			//@}

		private:
			/// @name Search parameters
			//@{
				boost::shared_ptr<const pt::FreeDRTArea> _area;
				boost::posix_time::ptime _dateTime;
				boost::shared_ptr<const geography::Place> _departurePlace;
				boost::shared_ptr<const geography::Place> _arrivalPlace;
			//@}


		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues Romain
			///	@date 2012
			FreeDRTBookingAdmin();



			/// @name Setters
			//@{
				void setArea(boost::shared_ptr<const pt::FreeDRTArea> value){ _area = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues Romain
			///	@date 2012
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues Romain
			///	@date 2012
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			///	@author Hugues Romain
			///	@date 2012
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Authorization check.
			/// Returns if the page can be displayed. In most cases, the needed right
			/// level is READ.
			///	@param request The current request
			///	@return bool True if the displayed page can be displayed
			///	@author Hugues Romain
			///	@date 2012
			bool isAuthorized(
				const security::User& user
			) const;



			virtual PageLinks _getCurrentTreeBranch() const;
		};
}	}

#endif // SYNTHESE_FreeDRTBookingAdmin_H__
