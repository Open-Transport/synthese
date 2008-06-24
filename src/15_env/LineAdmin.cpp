
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

#include "04_time/Schedule.h"

#include "05_html/HTMLTable.h"
#include "05_html/HTMLModule.h"

#include "15_env/ReservationRule.h"
#include "15_env/Line.h"
#include "15_env/LineTableSync.h"
#include "15_env/LineStop.h"
#include "15_env/LineStopTableSync.h"
#include "15_env/ScheduledService.h"
#include "15_env/ScheduledServiceTableSync.h"
#include "15_env/ContinuousService.h"
#include "15_env/ContinuousServiceTableSync.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"
#include "15_env/TransportNetworkRight.h"

#include "30_server/QueryString.h"
#include "30_server/Request.h"

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
	using namespace time;
	using namespace security;

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
			// Reservation
			bool reservation(_line->getReservationRule() && _line->getReservationRule()->isCompliant() == true);

			stream << "<h1>Arrêts desservis</h1>";

			vector<shared_ptr<LineStop> > linestops(LineStopTableSync::search(_line->getKey()));
			
			HTMLTable::ColsVector v;
			v.push_back("Rang");
			v.push_back("Arrêt");
			v.push_back("A");
			v.push_back("D");
			v.push_back("Hor");
			if (reservation)
				v.push_back("Resa");
			HTMLTable t(v,"adminresults");

			stream << t.open();

			for (vector<shared_ptr<LineStop> >::const_iterator it(linestops.begin()); it != linestops.end(); ++it)
			{
				stream << t.row();
				stream << t.col() << (*it)->getRankInPath();
				stream << t.col() << (*it)->getConnectionPlace()->getFullName();
				stream << t.col() << ((*it)->isArrival() ? HTMLModule::getHTMLImage("bullet_green.png","Arrivée possible") : HTMLModule::getHTMLImage("bullet_white.png", "Arrivée impossible"));
				stream << t.col() << ((*it)->isDeparture() ? HTMLModule::getHTMLImage("bullet_green.png", "Départ possible") : HTMLModule::getHTMLImage("bullet_white.png", "Départ impossible"));
				stream << t.col() << ((*it)->getScheduleInput() ? HTMLModule::getHTMLImage("time.png", "Horaire fourni à cet arrêt") : HTMLModule::getHTMLImage("tree_vert.png", "Houraire non fourni à cet arrêt"));
				if (reservation)
					stream << t.col() << HTMLModule::getHTMLImage("resa_compulsory.png", "Réservation obligatoire au départ de cet arrêt");
			}

			stream << t.close();

			stream << "<h1>Services à horaires</h1>";

			vector<shared_ptr<ScheduledService> > services(ScheduledServiceTableSync::search(_line->getKey()));

			if (services.empty())
				stream << "<p>Aucun service à horaire</p>";
			else
			{
				HTMLTable::ColsVector vs;
				vs.push_back("Numéro");
				vs.push_back("Départ");
				vs.push_back("Arrivée");
				vs.push_back("Durée");
				vs.push_back("Dernier jour");
				HTMLTable ts(vs,"adminresults");

				stream << ts.open();

				for (vector<shared_ptr<ScheduledService> >::const_iterator it(services.begin()); it != services.end(); ++it)
				{
					Schedule ds((*it)->getDepartureSchedule());
					Schedule as((*it)->getLastArrivalSchedule());
					
					stream << ts.row();
					stream << ts.col() << (*it)->getServiceNumber();
					
					stream << ts.col() << ds.toString();
					stream << ts.col() << as.toString();

					stream << ts.col() << (as - ds);

					stream << ts.col();
				}

				stream << ts.close();
			}


			stream << "<h1>Services continus</h1>";

			vector<shared_ptr<ContinuousService> > cservices(ContinuousServiceTableSync::search(_line->getKey()));

			if (cservices.empty())
				stream << "<p>Aucun service continu</p>";
			else
			{
				HTMLTable::ColsVector vc;
				vc.push_back("Départ premier");
				vc.push_back("Départ dernier");
				vc.push_back("Arrivée premier");
				vc.push_back("Arrivée dernier");
				vc.push_back("Durée");
				vc.push_back("Amplitude");
				vc.push_back("Fréquence");
				vc.push_back("Dernier jour");
				HTMLTable tc(vc,"adminresults");

				stream << tc.open();

				for (vector<shared_ptr<ContinuousService> >::const_iterator it(cservices.begin()); it != cservices.end(); ++it)
				{
					stream << tc.row();

					Schedule ds((*it)->getDepartureSchedule());
					Schedule as((*it)->getLastArrivalSchedule());

					stream << tc.col() << ds.toString();
					ds += (*it)->getRange();
					stream << tc.col() << ds.toString();

					stream << tc.col() << as.toString();
					as += (*it)->getRange();
					stream << tc.col() << as.toString();
					
					stream << tc.col() << (as - ds);

					stream << tc.col() << (*it)->getRange();
					stream << tc.col() << (*it)->getMaxWaitingTime();
					
					stream << tc.col();
				}

				stream << tc.close();
			}
		}

		bool LineAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return request->isAuthorized<TransportNetworkRight>(READ);
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
