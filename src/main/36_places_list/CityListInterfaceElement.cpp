
/** CityListInterfaceElement class implementation.
	@file CityListInterfaceElement.cpp

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

#include "CityListInterfaceElement.h"

#include <string>
#include <vector>

#include "01_util/Conversion.h"

#include "15_env/City.h"
#include "15_env/EnvModule.h"

#include "11_interfaces/ValueInterfaceElement.h"
#include "11_interfaces/ValueElementList.h"

using namespace std;

namespace synthese
{
	using namespace env;
	using namespace util;
	using namespace interfaces;

	namespace transportwebsite
	{
		string CityListInterfaceElement::display(
			std::ostream& stream
			, const ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request
		) const	{
			// Parameters
			string errorMessage = _errorMessage->getValue(parameters, variables, object, request);
			string openingText = _openingText->getValue(parameters, variables, object, request);
			string closingText = _closingText->getValue(parameters, variables, object, request);
			int n = Conversion::ToInt(_n->getValue(parameters, variables, object, request));
			string inputText = _inputText->getValue(parameters, variables, object, request);
			string emptyLineText = _emptyLineText->getValue(parameters, variables, object, request);

			const CityList& tbCommunes = *static_cast<const CityList*>(object);


			if ( tbCommunes[ 1 ] == NULL )
				stream << errorMessage;
			else
			{
				stream << "<script>Nom = new Array; Num = new Array;</script>";
				for ( int i = 1; i <= n; i++ )
					if ( tbCommunes[ i ] != NULL )
					{
						stream
							<< "<script>Nom[" << i << "]=\"" << tbCommunes[ i ] ->getName() << "\";Num[" << i << "]=" << tbCommunes[ i ] ->getKey() << ";</script>"
							<< openingText
                            << "<a href=\"javascript:MAJ(" << i << ")\">" << tbCommunes[ i ] ->getName() << "</a>"
							<< closingText;
					}
					else
						stream << emptyLineText;
			}
			return string();
		}

		void CityListInterfaceElement::storeParameters( interfaces::ValueElementList& pv)
		{
			_errorMessage = pv.front();
			_openingText = pv.front();
			_closingText = pv.front();
			_n = pv.front();
			_inputText = pv.front();
			_emptyLineText = pv.front();
		}
	}
}

