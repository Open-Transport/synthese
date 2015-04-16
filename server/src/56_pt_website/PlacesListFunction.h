
/** PlacesListFunction class header.
	@file PlacesListFunction.h
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

#ifndef SYNTHESE_PlacesListFunction_H__
#define SYNTHESE_PlacesListFunction_H__

#include "FunctionWithSite.h"
#include "FactorableTemplate.h"
#include "TransportWebsiteTypes.hpp"
#include "CoordinatesSystem.hpp"

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
		class PTServiceConfig;

		//////////////////////////////////////////////////////////////////////////
		/// Places list query public function.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Places_list
		//////////////////////////////////////////////////////////////////////////
		/// @author Hugues Romain
		///	@date 2007
		///	@ingroup m56Functions refFunctions
		class PlacesListFunction:
			public util::FactorableTemplate<cms::FunctionWithSite<false>,PlacesListFunction>
		{
		public:
			static const std::string PARAMETER_CONFIG_ID;
			static const std::string PARAMETER_INPUT;
			static const std::string PARAMETER_OLD_INPUT;
			static const std::string PARAMETER_CITY_TEXT;
			static const std::string PARAMETER_CITY_ID;
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_PAGE;
			static const std::string PARAMETER_ITEM_PAGE;
			static const std::string PARAMETER_SRID;

			static const std::string DATA_RESULTS_SIZE;
			static const std::string DATA_CITY_ID;
			static const std::string DATA_CITY_NAME;
			static const std::string DATA_CONTENT;
			static const std::string DATA_PLACES;
			static const std::string DATA_PLACE;

			static const std::string DATA_NAME;
			static const std::string DATA_RANK;

		protected:
			//! \name Page parameters
			//@{
				const PTServiceConfig*						_config;
				std::string									_input;
				std::string									_cityText;
				boost::optional<std::size_t>				_n;
				boost::shared_ptr<const cms::Webpage>		_page;
				boost::shared_ptr<const cms::Webpage>		_itemPage;
				boost::shared_ptr<geography::City>	_city;
				const CoordinatesSystem* _coordinatesSystem;
			//@}


			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

			void _displayItems(
				std::ostream& stream,
				const PlacesList& items,
				const server::Request& request
			) const;

		public:
			PlacesListFunction();

			/** Action to run, defined by each subclass.
			*/
			util::ParametersMap run(std::ostream& stream, const server::Request& request) const;

			void setTextInput(const std::string& text);
			void setNumber(int number);
			void setCityTextInput(const std::string& text);

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_PlacesListFunction_H__
