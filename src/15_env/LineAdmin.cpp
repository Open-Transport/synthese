////////////////////////////////////////////////////////////////////////////////
/// LineAdmin class implementation.
///	@file LineAdmin.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "LineAdmin.h"
#include "CommercialLineAdmin.h"
#include "EnvModule.h"

#include "Schedule.h"

#include "HTMLTable.h"
#include "HTMLModule.h"
#include "PhysicalStop.h"
#include "Line.h"
#include "LineTableSync.h"
#include "LineStop.h"
#include "LineStopTableSync.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "ContinuousService.h"
#include "ContinuousServiceTableSync.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "TransportNetworkRight.h"

#include "QueryString.h"
#include "Request.h"

#include "AdminParametersException.h"

#include <boost/foreach.hpp>

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
		const string LineAdmin::TAB_STOPS("stops");
		const string LineAdmin::TAB_SCHEDULED_SERVICES("sserv");
		const string LineAdmin::TAB_CONTINUOUS_SERVICES("cserv");

		LineAdmin::LineAdmin()
			: AdminInterfaceElementTemplate<LineAdmin>()
		{ }
		
		void LineAdmin::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_line = LineTableSync::Get(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY), _env, UP_LINKS_LOAD_LEVEL);
				LineStopTableSync::Search(_env, _line->getKey(), UNKNOWN_VALUE, 0, 0, true, true, UP_LINKS_LOAD_LEVEL);
				ScheduledServiceTableSync::Search(_env, _line->getKey(), UNKNOWN_VALUE, TIME_UNKNOWN, 0, 0, true, true, UP_DOWN_LINKS_LOAD_LEVEL);
				ContinuousServiceTableSync::Search(_env, _line->getKey(), 0, 0, true, true, UP_DOWN_LINKS_LOAD_LEVEL);
			}
			catch (...)
			{
				throw AdminParametersException("No such line");
			}
		}
		
		void LineAdmin::display(ostream& stream, VariablesMap& variables) const
		{
			////////////////////////////////////////////////////////////////////
			// TAB STOPS
			if (openTabContent(stream, TAB_STOPS))
			{
				// Reservation
// 				bool reservation(_line->getReservationRule() && _line->getReservationRule()->getType() == RESERVATION_COMPULSORY);

				HTMLTable::ColsVector v;
				v.push_back("Rang");
				v.push_back("Arrêt");
				v.push_back("A");
				v.push_back("D");
				v.push_back("Hor");
// 				if (reservation)
// 					v.push_back("Resa");
				HTMLTable t(v,"adminresults");

				stream << t.open();

				BOOST_FOREACH(shared_ptr<LineStop> lineStop, _env.getRegistry<LineStop>())
				{
					stream << t.row();
					stream << t.col() << lineStop->getRankInPath();
					stream << t.col() << lineStop->getPhysicalStop()->getConnectionPlace()->getFullName();
					stream << t.col() << (lineStop->isArrival() ? HTMLModule::getHTMLImage("bullet_green.png","Arrivée possible") : HTMLModule::getHTMLImage("bullet_white.png", "Arrivée impossible"));
					stream << t.col() << (lineStop->isDeparture() ? HTMLModule::getHTMLImage("bullet_green.png", "Départ possible") : HTMLModule::getHTMLImage("bullet_white.png", "Départ impossible"));
					stream << t.col() << (lineStop->getScheduleInput() ? HTMLModule::getHTMLImage("time.png", "Horaire fourni à cet arrêt") : HTMLModule::getHTMLImage("tree_vert.png", "Houraire non fourni à cet arrêt"));
// 					if (reservation)
// 						stream << t.col() << HTMLModule::getHTMLImage("resa_compulsory.png", "Réservation obligatoire au départ de cet arrêt");
				}

				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB SCHEDULED SERVICES
			if (openTabContent(stream, TAB_SCHEDULED_SERVICES))
			{
				const Registry<ScheduledService>& services(_env.getRegistry<ScheduledService>());
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

					BOOST_FOREACH(shared_ptr<ScheduledService> service, services)
					{
						Schedule ds(service->getDepartureSchedule());
						Schedule as(service->getLastArrivalSchedule());

						stream << ts.row();
						stream << ts.col() << service->getServiceNumber();

						stream << ts.col() << ds.toString();
						stream << ts.col() << as.toString();

						stream << ts.col() << (as - ds);

						stream << ts.col() << service->getLastActiveDate().toString();
					}

					stream << ts.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB CONTINUOUS SERVICES
			if (openTabContent(stream, TAB_CONTINUOUS_SERVICES))
			{
				const Registry<ContinuousService>& cservices(_env.getRegistry<ContinuousService>());
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

					BOOST_FOREACH(shared_ptr<ContinuousService> service, cservices)
					{
						stream << tc.row();

						Schedule ds(service->getDepartureSchedule());
						Schedule as(service->getLastArrivalSchedule());

						stream << tc.col() << ds.toString();
						ds += service->getRange();
						stream << tc.col() << ds.toString();

						stream << tc.col() << as.toString();
						as += service->getRange();
						stream << tc.col() << as.toString();

						stream << tc.col() << (as - ds);

						stream << tc.col() << service->getRange();
						stream << tc.col() << service->getMaxWaitingTime();

						stream << tc.col() << service->getLastActiveDate().toString();
					}

					stream << tc.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}

		bool LineAdmin::isAuthorized() const
		{
			if (_line.get() == NULL) return false;
			return _request->isAuthorized<TransportNetworkRight>(READ);
		}
		
		AdminInterfaceElement::PageLinks LineAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
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



		void LineAdmin::_buildTabs(
		) const {
			_tabs.clear();

			_tabs.push_back(Tab("Arrêts desservis", TAB_STOPS, true));
			_tabs.push_back(Tab("Services à horaire", TAB_SCHEDULED_SERVICES, true));
			_tabs.push_back(Tab("Services continus", TAB_CONTINUOUS_SERVICES, true));

			_tabBuilded = true;
		}
	}
}
