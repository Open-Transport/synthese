
/** SiteUpdateAction class header.
	@file SiteUpdateAction.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_SiteUpdateAction_H__
#define SYNTHESE_SiteUpdateAction_H__

#include "30_server/Action.h"

#include "01_util/FactorableTemplate.h"

#include "04_time/Date.h"

namespace synthese
{
	namespace interfaces
	{
		class Interface;
	}

	namespace transportwebsite
	{
		class Site;

		/** SiteUpdateAction action class.
			@ingroup m36Actions refActions
		*/
		class SiteUpdateAction
			: public util::FactorableTemplate<server::Action, SiteUpdateAction>
		{
		public:
			static const std::string PARAMETER_SITE_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_INTERFACE_ID;
			static const std::string PARAMETER_START_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_ONLINE_BOOKING;
			static const std::string PARAMETER_USE_OLD_DATA;
			static const std::string PARAMETER_MAX_CONNECTIONS;
			static const std::string PARAMETER_USE_DATES_RANGE;

		private:
			boost::shared_ptr<Site>							_site;
			std::string										_name;
			boost::shared_ptr<const interfaces::Interface>	_interface;
			time::Date										_startDate;
			time::Date										_endDate;
			bool											_onlineBooking;
			bool											_useOldData;
			int												_maxConnections;
			int												_useDatesRange;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);
			
			SiteUpdateAction();

			void setSiteId(uid id);

			virtual bool isAuthorized(const security::Profile& profile) const;
		};
	}
}

#endif // SYNTHESE_SiteUpdateAction_H__
