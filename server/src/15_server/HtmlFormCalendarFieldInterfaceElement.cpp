
/** HtmlFormCalendarFieldInterfaceElement class implementation.
	@file HtmlFormCalendarFieldInterfaceElement.cpp
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

#include "HtmlFormCalendarFieldInterfaceElement.h"
#include "ValueElementList.h"
#include "HTMLForm.h"

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, server::HtmlFormCalendarFieldInterfaceElement>::FACTORY_KEY("htmlcalendar");
	}

	namespace server
	{
		void HtmlFormCalendarFieldInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_name = vel.front();
			_withHour = vel.front();
			_value = vel.front();
		}

		string HtmlFormCalendarFieldInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {

			string name(_name->getValue(parameters,variables,object,request));
			if (lexical_cast<bool>(_withHour->getValue(parameters,variables,object,request)))
			{
				posix_time::ptime dt(not_a_date_time);
				string dtparameter(_value->getValue(parameters,variables,object,request));
				if(!dtparameter.empty())
				{
					dt = time_from_string(dtparameter);
				}
				stream << HTMLForm::GetCalendarInput(name, dt);
			}
			else
			{
				gregorian::date dt(not_a_date_time);
				string dtparameter(_value->getValue(parameters,variables,object,request));
				if(!dtparameter.empty())
				{
					dt = from_string(dtparameter);
				}
				stream << HTMLForm::GetCalendarInput(name, dt);
			}

			return string();
		}

		HtmlFormCalendarFieldInterfaceElement::~HtmlFormCalendarFieldInterfaceElement()
		{
		}
	}
}
