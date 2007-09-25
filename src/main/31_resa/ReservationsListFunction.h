
/** ReservationsListFunction class header.
	@file ReservationsListFunction.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_ReservationsListFunction_H__
#define SYNTHESE_ReservationsListFunction_H__

#include "36_places_list/FunctionWithSite.h"

#include "04_time/DateTime.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace env
	{
		class CommercialLine;
	}

	namespace security
	{
		class User;
	}

	namespace resa
	{
		/** ReservationsListFunction Function class.
			@author Hugues Romain
			@date 2007
			@ingroup m31Functions refFunctions
		*/
		class ReservationsListFunction : public transportwebsite::FunctionWithSite
		{
		public:
			static const std::string PARAMETER_USER_ID;
			static const std::string PARAMETER_USER_NAME;
			static const std::string PARAMETER_LINE_ID;
			static const std::string PARAMETER_DATE;
			static const std::string PARAMETER_DISPLAY_CANCELLED;
			
		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const security::User>			_user;
				std::string										_userName;
				boost::shared_ptr<const env::CommercialLine>	_line;
				time::DateTime									_startDateTime;
				time::DateTime									_endDateTime;
				bool											_displayCancelled;
			//@}
			
			
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap _getParametersMap() const;
			
			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const server::ParametersMap& map);
			
			
		public:
			/** Action to run, defined by each subclass.
			*/
			void _run(std::ostream& stream) const;

			void setLine(boost::shared_ptr<const env::CommercialLine> line);

			ReservationsListFunction();
		};
	}
}

#endif // SYNTHESE_ReservationsListFunction_H__
