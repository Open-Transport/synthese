
/** LineStyleInterfaceElement class implementation.
	@file LineStyleInterfaceElement.cpp

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

#include <string>

#include "01_util/UId.h"
#include "01_util/Conversion.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"

#include "15_env/Line.h"
#include "15_env/CommercialLine.h"
#include "15_env/LineStyleInterfaceElement.h"

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using std::string;

	namespace env
	{
		string LineStyleInterfaceElement::getValue( const ParametersVector& parameters, interfaces::VariablesMap& variables, const void* object, const server::Request* request) const
		{
			return (object != NULL) ? ((Line*) object)->getCommercialLine()->getStyle() : "";
		}

		void LineStyleInterfaceElement::storeParameters(ValueElementList& vel)
		{
		}
	}

}
