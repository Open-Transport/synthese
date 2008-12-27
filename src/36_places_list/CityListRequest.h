
/** CityListRequest class header.
	@file CityListRequest.h

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

#ifndef SYNTHESE_CityListRequest_H__
#define SYNTHESE_CityListRequest_H__

#include "36_places_list/FunctionWithSite.h"

#include "01_util/FactorableTemplate.h"

#include <string>

namespace synthese
{
	namespace env
	{
		class City;
	}

	namespace transportwebsite
	{
		class PlacesListInterfacePage;

		/** City list request.
			@ingroup m36Functions refFunctions

		*/
		class CityListRequest : public util::FactorableTemplate<FunctionWithSite,CityListRequest>
		{
		public:
			static const std::string PARAMETER_INPUT;
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_IS_FOR_ORIGIN;

		private:
			std::string _input;
			int _n;
			bool _isForOrigin;
			boost::shared_ptr<const env::City>					_city;
			const PlacesListInterfacePage*	_page;

		protected:
			server::ParametersMap _getParametersMap() const;
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			void _run(std::ostream& stream) const;

			void setTextInput(const std::string& text);
			void setNumber(int number);
			void setIsForOrigin(bool isForOrigin);

			virtual bool _isAuthorized() const;
		};
	}
}

#endif // SYNTHESE_CityListRequest_H__
