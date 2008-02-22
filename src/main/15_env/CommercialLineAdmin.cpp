
/** CommercialLineAdmin class implementation.
	@file CommercialLineAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "CommercialLineAdmin.h"
#include "TransportNetworkAdmin.h"
#include "EnvModule.h"

#include "15_env/CommercialLine.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/Line.h"
#include "15_env/LineAdmin.h"
#include "15_env/LineTableSync.h"

#include "30_server/QueryString.h"

#include "32_admin/AdminParametersException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, CommercialLineAdmin>::FACTORY_KEY("CommercialLineAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<CommercialLineAdmin>::ICON("chart_line.png");
		template<> const string AdminInterfaceElementTemplate<CommercialLineAdmin>::DEFAULT_TITLE("Ligne inconnue");
	}

	namespace env
	{
		CommercialLineAdmin::CommercialLineAdmin()
			: AdminInterfaceElementTemplate<CommercialLineAdmin>()
		{ }
		
		void CommercialLineAdmin::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_cline = CommercialLineTableSync::Get(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			}
			catch (...)
			{
				throw AdminParametersException("No such line");
			}
		}
		
		void CommercialLineAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			/// @todo Implement the display by streaming the output to the stream variable
			stream << "Not yet implemented, use treeview to navigate";
		}

		bool CommercialLineAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return true;
		}
		
		AdminInterfaceElement::PageLinks CommercialLineAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			return links;
		}
		
		std::string CommercialLineAdmin::getTitle() const
		{
			return _cline.get() ? "<span class=\"" + _cline->getStyle() +"\">" + _cline->getShortName() + "</span>" : DEFAULT_TITLE;
		}

		std::string CommercialLineAdmin::getParameterName() const
		{
			return _cline.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string CommercialLineAdmin::getParameterValue() const
		{
			return _cline.get() ? Conversion::ToString(_cline->getKey()) : string();
		}

		AdminInterfaceElement::PageLinks CommercialLineAdmin::getSubPages( const AdminInterfaceElement& currentPage , const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			AdminInterfaceElement::PageLinks links;
			if (currentPage.getFactoryKey() == CommercialLineAdmin::FACTORY_KEY && _cline->getKey() == static_cast<const CommercialLineAdmin&>(currentPage).getCommercialLine()->getKey()
				|| currentPage.getFactoryKey() == LineAdmin::FACTORY_KEY && _cline->getKey() == static_cast<const LineAdmin&>(currentPage).getLine()->getCommercialLine()->getKey()
				)
			{
				vector<shared_ptr<Line> > lines(LineTableSync::search(_cline->getKey()));
				for (vector<shared_ptr<Line> >::const_iterator it(lines.begin()); it != lines.end(); ++it)
				{
					PageLink link;
					link.factoryKey = LineAdmin::FACTORY_KEY;
					link.icon = LineAdmin::ICON;
					link.name = (*it)->getName();
					link.parameterName = QueryString::PARAMETER_OBJECT_ID;
					link.parameterValue = Conversion::ToString((*it)->getKey());
					links.push_back(link);
				}
			}
			return links;
		}

		boost::shared_ptr<const CommercialLine> CommercialLineAdmin::getCommercialLine() const
		{
			return _cline;
		}
	}
}
