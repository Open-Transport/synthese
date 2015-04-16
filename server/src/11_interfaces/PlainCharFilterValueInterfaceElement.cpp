
/** PlainCharFilterValueInterfaceElement class implementation.
	@file PlainCharFilterValueInterfaceElement.cpp

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

#include <string>

#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"

#include "PlainCharFilterValueInterfaceElement.h"

#include "01_util/PlainCharFilter.h"

#include <sstream>

#include <boost/iostreams/operations.hpp>


using namespace std;
namespace io = boost::iostreams;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace interfaces;

	namespace util
	{
		template<> const std::string FactorableTemplate<LibraryInterfaceElement, PlainCharFilterValueInterfaceElement>::FACTORY_KEY = "plain_char";
	}

	namespace interfaces
	{
		string PlainCharFilterValueInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters, interfaces::VariablesMap& variables, const void* object, const server::Request* request) const
		{
			string text = _text->getValue(parameters, variables, object, request);

			std::stringstream sout;

			PlainCharFilter filter;

			io::filtering_ostream out;
			out.push (filter);
			out.push (sout);
			out << text << std::flush;
			stream << sout.str();
			return string();
		}

		void PlainCharFilterValueInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() < 1)
				throw InterfacePageException("1 parameter must be provided");

			_text = vel.front();
		}
	}

}
