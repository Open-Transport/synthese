
/** AddUserFavoriteJourneyAction class header.
	@file AddUserFavoriteJourneyAction.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_AddUserFavoriteJourneyAction_H__
#define SYNTHESE_AddUserFavoriteJourneyAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace pt_website
	{
		/** AddUserFavoriteJourneyAction action class.
			@ingroup m56Actions refActions
		*/
		class AddUserFavoriteJourneyAction
			: public util::FactorableTemplate<server::Action, AddUserFavoriteJourneyAction>
		{
		public:
			static const std::string PARAMETER_USER_ID;
			static const std::string PARAMETER_ORIGIN_CITY_NAME;
			static const std::string PARAMETER_ORIGIN_PLACE_NAME;
			static const std::string PARAMETER_DESTINATION_CITY_NAME;
			static const std::string PARAMETER_DESTINATION_PLACE_NAME;

		private:
			boost::shared_ptr<security::User>	_user;
			std::string						_originCityName;
			std::string						_originPlaceName;
			std::string						_destinationCityName;
			std::string						_destinationPlaceName;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			AddUserFavoriteJourneyAction();


			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_AddUserFavoriteJourneyAction_H__
