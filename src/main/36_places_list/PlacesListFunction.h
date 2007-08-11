
/** PlacesListFunction class header.
	@file PlacesListFunction.h
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

#ifndef SYNTHESE_PlacesListFunction_H__
#define SYNTHESE_PlacesListFunction_H__

#include "36_places_list/FunctionWithSite.h"

namespace synthese
{
	namespace env
	{
		class City;
	}

	namespace transportwebsite
	{
		class PlacesListInterfacePage;

		/** PlacesListFunction Function class.
			@author Hugues Romain
			@date 2007
			@ingroup m36Functions refFunctions
		*/
		class PlacesListFunction : public FunctionWithSite
		{
		public:
			static const std::string PARAMETER_INPUT;
			static const std::string PARAMETER_CITY_TEXT;
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_IS_FOR_ORIGIN;
			
		protected:
			//! \name Page parameters
			//@{
				std::string							_input;
				std::string							_cityText;
				int									_n;
				bool								_isForOrigin;
				boost::shared_ptr<const PlacesListInterfacePage>	_page;
			//@}
			
			
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap _getParametersMap() const;
			
			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const server::ParametersMap& map);
			
			/// @todo Getters/Setters for parsed parameters
			
		public:
			/** Action to run, defined by each subclass.
			*/
			void _run(std::ostream& stream) const;

			void setTextInput(const std::string& text);
			void setNumber(int number);
			void setIsForOrigin(bool isForOrigin);
			void setCityTextInput(const std::string& text);
		};
	}
}

#endif // SYNTHESE_PlacesListFunction_H__
