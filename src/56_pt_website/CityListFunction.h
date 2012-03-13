
/** CityListFunction class header.
	@file CityListFunction.h

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

#ifndef SYNTHESE_CityListRequest_H__
#define SYNTHESE_CityListRequest_H__

#include "FunctionWithSite.h"
#include "FactorableTemplate.h"
#include "TransportWebsiteTypes.hpp"

#include <string>

namespace synthese
{
	namespace geography
	{
		class City;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace pt_website
	{
		class PlacesListInterfacePage;
		class PTServiceConfig;

		//////////////////////////////////////////////////////////////////////////
		/// City list query public function.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Cities_list
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m56Functions refFunctions
		/// @author Hugues Romain
		class CityListFunction:
			public util::FactorableTemplate<cms::FunctionWithSite<false>, CityListFunction>
		{
		public:
			static const std::string PARAMETER_INPUT;
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_PAGE;
			static const std::string PARAMETER_ITEM_PAGE;
			static const std::string PARAMETER_AT_LEAST_A_STOP;
			static const std::string PARAMETER_SRID;
			static const std::string PARAMETER_CONFIG_ID;

			static const std::string DATA_RESULTS_SIZE;
			static const std::string DATA_CONTENT;

			static const std::string DATA_CITY;
			static const std::string DATA_CITIES;
			static const std::string DATA_NAME;
			static const std::string DATA_RANK;

		private:
			std::string _input;
			boost::optional<std::size_t> _n;
			bool _atLeastAStop;
			boost::shared_ptr<const cms::Webpage>	_page;
			boost::shared_ptr<const cms::Webpage>	_itemPage;
			const CoordinatesSystem* _coordinatesSystem;
			const PTServiceConfig* _config;

		protected:
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Cities_list#Request
			//////////////////////////////////////////////////////////////////////////
			util::ParametersMap _getParametersMap() const;



			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Cities_list#Request
			//////////////////////////////////////////////////////////////////////////
			void _setFromParametersMap(const util::ParametersMap& map);



			void _displayItems(
				std::ostream& stream,
				const util::ParametersMap& pm,
				const server::Request& request
			) const;

		public:
			CityListFunction(): _atLeastAStop(false), _config(NULL) {}

			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Cities_list#Response
			//////////////////////////////////////////////////////////////////////////
			util::ParametersMap run(std::ostream& stream, const server::Request& request) const;

			void setTextInput(const std::string& text){ _input = text; }
			void setNumber(boost::optional<std::size_t> number){ _n = number; }

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_CityListRequest_H__
