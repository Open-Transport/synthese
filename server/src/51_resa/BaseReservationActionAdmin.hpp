
//////////////////////////////////////////////////////////////////////////
/// BaseReservationActionAdmin class header.
///	@file BaseReservationActionAdmin.hpp
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

#ifndef SYNTHESE_BaseReservationActionAdmin_H__
#define SYNTHESE_BaseReservationActionAdmin_H__

#include "AdminInterfaceElement.h"

#include <string>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace html
	{
		class HTMLForm;
	}

	namespace util
	{
		class ParametersMap;
	}

	namespace security
	{
		class User;
	}

	namespace resa
	{
		class ReservationTransaction;

		//////////////////////////////////////////////////////////////////////////
		/// BaseReservationActionAdmin Admin compound class.
		///	@ingroup m51Admin refAdmin
		///	@author Hugues Romain
		///	@date 2012
		class BaseReservationActionAdmin
		{
		public:
			/// @name Parameter identifiers
			//@{
				static const std::string PARAMETER_CUSTOMER_ID;
				static const std::string PARAMETER_SEATS_NUMBER;
			//@}

		protected:
			/// @name Search parameters
			//@{
				boost::shared_ptr<const security::User>		_customer;
				size_t										_seatsNumber;
				boost::shared_ptr<ReservationTransaction>	_confirmedTransaction;
			//@}



		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues Romain
			///	@date 2012
			BaseReservationActionAdmin();



			/// @name Setters
			//@{
				void setCustomer(boost::shared_ptr<const security::User> value){ _customer = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues Romain
			///	@date 2012
			void setBaseReservationFromParametersMap(
				util::Env& env,
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues Romain
			///	@date 2012
			util::ParametersMap getBaseReservationParametersMap() const;



			void displayReservationForm(
				std::ostream& stream,
				html::HTMLForm& rf,
				const server::Request& request
			) const;
		};
}	}

#endif // SYNTHESE_BaseReservationActionAdmin_H__

