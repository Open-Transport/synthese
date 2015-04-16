
//////////////////////////////////////////////////////////////////////////////////////////
/// ReservationsListService class header.
///	@file ReservationsListService.hpp
///	@author Hugues Romain
///	@date 2011
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

#ifndef SYNTHESE_ReservationsListService_H__
#define SYNTHESE_ReservationsListService_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include "CommercialLine.h"
#include "Language.hpp"
#include "ServiceReservations.h"

#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace pt
	{
		class CommercialLine;
		class ScheduledService;
	}

	namespace resa
	{
		class Reservation;

		//////////////////////////////////////////////////////////////////////////
		///	51.15 Function : ReservationsListService.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Reservations_List
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m51Functions refFunctions
		///	@author Hugues Romain
		///	@date 2011
		/// @since 3.2.1
		class ReservationsListService:
			public util::FactorableTemplate<server::Function,ReservationsListService>
		{
		public:
			static const std::string PARAMETER_LINE_ID;
			static const std::string PARAMETER_SERVICE_NUMBER;
			static const std::string PARAMETER_DATE;
			static const std::string PARAMETER_DATE2;
			static const std::string PARAMETER_RESERVATION_PAGE_ID;
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_LANGUAGE;
			static const std::string PARAMETER_MINIMAL_DEPARTURE_RANK;
			static const std::string PARAMETER_MAXIMAL_DEPARTURE_RANK;
			static const std::string PARAMETER_MINIMAL_ARRIVAL_RANK;
			static const std::string PARAMETER_MAXIMAL_ARRIVAL_RANK;
			static const std::string PARAMETER_LINKED_WITH_VEHICLE_ONLY;
			static const std::string PARAMETER_USE_CACHE;
			static const std::string PARAMETER_OUTPUT_FORMAT;
			static const std::string PARAMETER_WITH_CANCELLATIONS;
			static const std::string PARAMETER_WITH_NOT_ACKNOWLEDGED_CANCELLATIONS;
			static const std::string PARAMETER_ONLY_NOT_ACKNOWLEDGED_RESERVATIONS;

			static const std::string DATA_RESERVATIONS;
			static const std::string DATA_RESERVATION;
			static const std::string DATA_RANK;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const pt::CommercialLine> _line;
				boost::optional<std::string> _serviceNumber;
				boost::shared_ptr<const pt::ScheduledService> _service;
				boost::gregorian::date _date;
				boost::gregorian::date _date2;
				boost::shared_ptr<const cms::Webpage> _reservationPage;
				boost::optional<Language> _language;
				boost::optional<std::size_t> _minDepartureRank;
				boost::optional<std::size_t> _maxDepartureRank;
				boost::optional<std::size_t> _minArrivalRank;
				boost::optional<std::size_t> _maxArrivalRank;
				bool _linkedWithVehicleOnly;
				bool _useCache;
				std::string _outputFormat;
				bool _withCancellations;
				bool _withNotAcknowledgedCancellations;
				bool _onlyNotAcknowledgedReservations;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Reservations_List#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2011
			/// @since 3.2.1
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Reservations_List#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2011
			/// @since 3.2.1
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


			typedef std::map<std::string, ServiceReservations> ReservationsByServiceNumber;

			void _fiterAndDisplayReservations(
				util::ParametersMap& pm,
				const pt::CommercialLine::ServicesVector& services,
				const ReservationsByServiceNumber& reservations
			) const;


		public:
			//! @name Setters
			//@{
			//	void setObject(boost::shared_ptr<const Object> value) { _object = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Reservations_List#Request
			//////////////////////////////////////////////////////////////////////////
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2011
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2011
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2011
			virtual std::string getOutputMimeType() const;


			ReservationsListService();
		};
}	}

#endif // SYNTHESE_ReservationsListService_H__
