
/** HtmlFormFieldInterfaceElement class implementation.
	@file HtmlFormFieldInterfaceElement.cpp
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

#include "HtmlFormFieldInterfaceElement.h"

#include "11_interfaces/ValueElementList.h"

#include "05_html/HTMLForm.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, server::HtmlFormFieldInterfaceElement>::FACTORY_KEY("htmlinput");
	}

	namespace server
	{
		void HtmlFormFieldInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_name = vel.front();
			_value = vel.front();
		}

		string HtmlFormFieldInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			string name(_name->getValue(parameters,variables,object,request));
			string value(_value->getValue(parameters,variables,object,request));

			stream << HTMLForm::GetTextInput(name, value);

			return string();
		}

		HtmlFormFieldInterfaceElement::~HtmlFormFieldInterfaceElement()
		{
		}
	}
}
