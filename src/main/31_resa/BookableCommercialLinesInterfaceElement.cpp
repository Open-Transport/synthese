
/** BookableCommercialLinesInterfaceElement class implementation.
	@file BookableCommercialLinesInterfaceElement.cpp
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

#include "BookableCommercialLinesInterfaceElement.h"

#include "31_resa/ResaRight.h"
#include "31_resa/ReservationsListFunction.h"

#include "30_server/Request.h"
#include "30_server/FunctionRequest.h"

#include "15_env/CommercialLine.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/LineMarkerInterfacePage.h"

#include "12_security/User.h"
#include "12_security/Profile.h"

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/Interface.h"

#include "05_html/HTMLTable.h"
#include "05_html/HTMLModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace html;
	using namespace interfaces;
	using namespace env;
	using namespace security;
	
	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, resa::BookableCommercialLinesInterfaceElement>::FACTORY_KEY("bookable_lines_list");
	}

	namespace resa
	{
		void BookableCommercialLinesInterfaceElement::storeParameters(ValueElementList& vel)
		{
		}

		string BookableCommercialLinesInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			vector<shared_ptr<CommercialLine> > lines(CommercialLineTableSync::search(
				request->getUser()->getProfile()->getRightsForModuleClass<ResaRight>()
				, request->getUser()->getProfile()->getGlobalPublicRight<ResaRight>() >= READ
				, READ
				, 0, 0
				, false, true, true, true
			));
			const LineMarkerInterfacePage* page(_page->getInterface()->getPage<LineMarkerInterfacePage>());

			FunctionRequest<ReservationsListFunction> reservationList(request);
			
			HTMLTable t;
			string nothing;
			stream << t.open();
			for (vector<shared_ptr<CommercialLine> >::const_iterator it(lines.begin()); it != lines.end(); ++it)
			{
				reservationList.getFunction()->setLine(*it);
				stream << t.row();
				page->display(stream, variables, nothing, nothing, 23, 16, it->get(), request);
				stream << t.col() << HTMLModule::getHTMLLink(reservationList.getURL(), (*it)->getName());
			}
			stream << t.close();

			return string();
		}

		BookableCommercialLinesInterfaceElement::~BookableCommercialLinesInterfaceElement()
		{
		}
	}
}
