
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

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/gregorian/greg_duration_types.hpp>

namespace synthese
{
	namespace interfaces
	{
		class Interface;
	}

	namespace cms
	{
	    class Webpage;
	}

	namespace pt_website
	{
		class TransportWebsite;

		//////////////////////////////////////////////////////////////////////////
		/// 56.15 Action : Transport web site update.
		/// @ingroup m56Actions refActions
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Key : SiteUpdateAction
		/// Parameters :
		///	<ul>
		///		<li>actionParamsi : site to update</li>
		///		<li>actionParamna : new value for site name</li>
		///		<li>actionParamcu : new value for client URL</li>
		///		<li>actionParamti : new value for default page template id</li>
		///		<li>...</li>
		///		<li>actionParamda : new value for the display of the details of road approach</li>
		///	</ul>
		class SiteUpdateAction
			: public util::FactorableTemplate<server::Action, SiteUpdateAction>
		{
		public:
			static const std::string PARAMETER_SITE_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_START_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_ONLINE_BOOKING;
			static const std::string PARAMETER_USE_OLD_DATA;
			static const std::string PARAMETER_MAX_CONNECTIONS;
			static const std::string PARAMETER_USE_DATES_RANGE;
			static const std::string PARAMETER_DISPLAY_ROAD_APPROACH_DETAIL;
			static const std::string PARAMETER_CLIENT_URL;
			static const std::string PARAMETER_DEFAULT_PAGE_TEMPLATE_ID;

		private:
			boost::shared_ptr<TransportWebsite>							_site;
			std::string										_name;
			boost::gregorian::date										_startDate;
			boost::gregorian::date										_endDate;
			bool											_onlineBooking;
			bool											_useOldData;
			int												_maxConnections;
			boost::gregorian::date_duration							_useDatesRange;
			bool											_displayRoadApproachDetail;
			std::string							_clientURL;
			boost::shared_ptr<cms::Webpage>			_defaultPageTemplate;

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

			void setSiteId(util::RegistryKeyType id);

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_SiteUpdateAction_H__
