
/** LineAdmin class implementation.
	@file LineAdmin.cpp
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

#include "LineAdmin.h"
#include "CommercialLineAdmin.h"
#include "EnvModule.h"

#include "05_html/HTMLTable.h"

#include "15_env/Line.h"
#include "15_env/LineTableSync.h"
#include "15_env/LineStop.h"
#include "15_env/LineStopTableSync.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"

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
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, LineAdmin>::FACTORY_KEY("LineAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<LineAdmin>::ICON("chart_line.png");
		template<> const string AdminInterfaceElementTemplate<LineAdmin>::DEFAULT_TITLE("Ligne inconnue");
	}

	namespace env
	{
		LineAdmin::LineAdmin()
			: AdminInterfaceElementTemplate<LineAdmin>()
		{ }
		
		void LineAdmin::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_line = LineTableSync::Get(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			}
			catch (...)
			{
				throw AdminParametersException("No such line");
			}
		}
		
		void LineAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			stream << "<h1>Arrêts desservis</h1>";

			vector<shared_ptr<LineStop> > linestops(LineStopTableSync::search(_line->getKey()));
			
			HTMLTable::ColsVector v;
			v.push_back("Rang");
			v.push_back("Arrêt");
			v.push_back("DA");
			v.push_back("Hor");
			HTMLTable t(v,"adminresults");

			stream << t.open();

			for (vector<shared_ptr<LineStop> >::const_iterator it(linestops.begin()); it != linestops.end(); ++it)
			{
				stream << t.row();
				stream << t.col() << (*it)->getRankInPath();
				stream << t.col() << (*it)->getConnectionPlace()->getFullName();
				stream << t.col() << ((*it)->isArrival() ? "A" : "") << ((*it)->isDeparture() ? "D" : "");
				stream << t.col() << ((*it)->getScheduleInput() ? "X" : "|");
			}

			stream << t.close();
		}

		bool LineAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return true;
		}
		
		AdminInterfaceElement::PageLinks LineAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			return links;
		}
		

		std::string LineAdmin::getTitle() const
		{
			return _line.get() ? "Route " + _line->getName() : DEFAULT_TITLE;
		}

		std::string LineAdmin::getParameterName() const
		{
			return _line.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string LineAdmin::getParameterValue() const
		{
			return _line.get() ? Conversion::ToString(_line->getKey()) : string();
		}

		boost::shared_ptr<const Line> LineAdmin::getLine() const
		{
			return _line;
		}
	}
}
