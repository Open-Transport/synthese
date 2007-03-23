
/** LineMarkerInterfacePage class implementation.
	@file LineMarkerInterfacePage.cpp

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

#include "01_util/Conversion.h"

#include "15_env/LineMarkerInterfacePage.h"

namespace synthese
{
	using namespace util;
	using namespace interfaces;

	namespace env
	{
		void LineMarkerInterfacePage::display( std::ostream& stream, interfaces::VariablesMap& vars, const std::string& tableOpeningHTML , const std::string& tableClosingHTML , int pixelWidth , int pixelHeight , const synthese::env::Line* line , const server::Request* request /*= NULL */ ) const
		{
			ParametersVector pv;
			pv.push_back(tableOpeningHTML);
			pv.push_back(tableClosingHTML);
			pv.push_back(Conversion::ToString(pixelWidth));
			pv.push_back(Conversion::ToString(pixelHeight));
			InterfacePage::display(stream, pv, vars, (const void*) line, request);
		}
	}
}
