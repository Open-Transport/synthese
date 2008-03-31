
/** BarCodeHTMLFormValueInterfaceElement class implementation.
	@file BarCodeHTMLFormValueInterfaceElement.cpp

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

#include "71_vinci_bike_rental/BarCodeHTMLFormValueInterfaceElement.h"
#include "71_vinci_bike_rental/BarCodeInterpretFunction.h"

#include "01_util/Conversion.h"

#include "05_html/HTMLForm.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/HomeAdmin.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace server;
	using namespace html;
	using namespace admin;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, vinci::BarCodeHTMLFormValueInterfaceElement>::FACTORY_KEY("barcodefield");
	}

	namespace vinci
	{
		string BarCodeHTMLFormValueInterfaceElement::display(
			ostream& s
			, const ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object
			, const server::Request* request
		) const	{
			FunctionRequest<BarCodeInterpretFunction> newRequest(request);
			HTMLForm f = newRequest.getHTMLForm("barcode");
			f.addHiddenField(BarCodeInterpretFunction::PARAMETER_LAST_OBJECT_ID, Conversion::ToString(request->getObjectId()));
			const shared_ptr<const AdminInterfaceElement>* aie = (const shared_ptr<const AdminInterfaceElement>*) object;
			f.addHiddenField(BarCodeInterpretFunction::PARAMETER_LAST_PAGE, (*aie)->getFactoryKey());

			s << f.open();
			s << f.getTextInput(BarCodeInterpretFunction::PARAMETER_READED_CODE, string());
			s << f.getSubmitButton("OK");
			s << f.close();
			s << f.setFocus(BarCodeInterpretFunction::PARAMETER_READED_CODE);

			return string();
		}

		void BarCodeHTMLFormValueInterfaceElement::storeParameters(ValueElementList& vel)
		{
		}
	}

}
