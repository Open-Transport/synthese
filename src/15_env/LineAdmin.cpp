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

#include "Request.h"
#include "AdminRequest.h"

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
		
		void LineAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			try
			{
				_line = LineTableSync::Get(
					map.getUid(Request::PARAMETER_OBJECT_ID, true, FACTORY_KEY), _getEnv(),
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch (Exception& e)
			{
				throw AdminParametersException("No such line");
			}
		}
		
		
		
		server::ParametersMap LineAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_line.get()) m.insert(Request::PARAMETER_OBJECT_ID, _line->getKey());
			return m;
		}


		
		void LineAdmin::display(ostream& stream, VariablesMap& variables,
					const server::FunctionRequest<admin::AdminRequest>& _request) const
		{
			////////////////////////////////////////////////////////////////////
			// TAB STOPS
			if (openTabContent(stream, TAB_STOPS))
			{
				// Reservation
// 				bool reservation(_line->getReservationRule() && _line->getReservationRule()->getType() == RESERVATION_COMPULSORY);
				LineStopTableSync::SearchResult lineStops(
					LineStopTableSync::Search(
						_getEnv(),
						_line->getKey(),
						UNKNOWN_VALUE,
						0,
						optional<size_t>(),
						true,
						true,
						UP_LINKS_LOAD_LEVEL
				)	);

				HTMLTable::ColsVector v;
				v.push_back("Rang");
				v.push_back("Arr�t");
				v.push_back("A");
				v.push_back("D");
				v.push_back("Hor");
// 				if (reservation)
// 					v.push_back("Resa");
				HTMLTable t(v,"adminresults");

				stream << t.open();

				BOOST_FOREACH(shared_ptr<LineStop> lineStop, lineStops)
				{
					stream << t.row();
					stream << t.col() << lineStop->getRankInPath();
					stream << t.col() << lineStop->getPhysicalStop()->getConnectionPlace()->getFullName();
					stream << t.col() << (lineStop->isArrival() ? HTMLModule::getHTMLImage("bullet_green.png","Arriv�e possible") : HTMLModule::getHTMLImage("bullet_white.png", "Arriv�e impossible"));
					stream << t.col() << (lineStop->isDeparture() ? HTMLModule::getHTMLImage("bullet_green.png", "D�part possible") : HTMLModule::getHTMLImage("bullet_white.png", "D�part impossible"));
					stream << t.col() << (lineStop->getScheduleInput() ? HTMLModule::getHTMLImage("time.png", "Horaire fourni � cet arr�t") : HTMLModule::getHTMLImage("tree_vert.png", "Houraire non fourni � cet arr�t"));
// 					if (reservation)
// 						stream << t.col() << HTMLModule::getHTMLImage("resa_compulsory.png", "R�servation obligatoire au d�part de cet arr�t");
				}

				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB SCHEDULED SERVICES
			if (openTabContent(stream, TAB_SCHEDULED_SERVICES))
			{
				ScheduledServiceTableSync::SearchResult sservices(
					ScheduledServiceTableSync::Search(
						_getEnv(),
						_line->getKey(),
						optional<RegistryKeyType>(),
						optional<RegistryKeyType>(),
						optional<string>(),
						optional<Date>(),
						false,
						0,
						optional<size_t>(),
						true, true,
						UP_DOWN_LINKS_LOAD_LEVEL
				)	);
				if (sservices.empty())
					stream << "<p>Aucun service � horaire</p>";
				else
				{
					HTMLTable::ColsVector vs;
					vs.push_back("Num�ro");
					vs.push_back("D�part");
					vs.push_back("Arriv�e");
					vs.push_back("Dur�e");
					vs.push_back("Dernier jour");
					HTMLTable ts(vs,"adminresults");

					stream << ts.open();

					BOOST_FOREACH(shared_ptr<ScheduledService> service, sservices)
					{
						Schedule ds(service->getDepartureSchedule(false, 0));
						Schedule as(service->getLastArrivalSchedule(false));

						stream << ts.row();
						stream << ts.col() << service->getServiceNumber();

						stream << ts.col() << ds.toString();
						stream << ts.col() << as.toString();

						stream << ts.col() << (as - ds);

						stream << ts.col() << to_iso_extended_string(service->getLastActiveDate());
					}

					stream << ts.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB CONTINUOUS SERVICES
			if (openTabContent(stream, TAB_CONTINUOUS_SERVICES))
			{
				ContinuousServiceTableSync::SearchResult cservices(
					ContinuousServiceTableSync::Search(
						_getEnv(),
						_line->getKey(),
						optional<RegistryKeyType>(),
						0,
						optional<size_t>(),
						true,
						true,
						UP_DOWN_LINKS_LOAD_LEVEL
				)	);
				if (cservices.empty())
					stream << "<p>Aucun service continu</p>";
				else
				{
					HTMLTable::ColsVector vc;
					vc.push_back("D�part premier");
					vc.push_back("D�part dernier");
					vc.push_back("Arriv�e premier");
					vc.push_back("Arriv�e dernier");
					vc.push_back("Dur�e");
					vc.push_back("Amplitude");
					vc.push_back("Fr�quence");
					vc.push_back("Dernier jour");
					HTMLTable tc(vc,"adminresults");

					stream << tc.open();

					BOOST_FOREACH(shared_ptr<ContinuousService> service, cservices)
					{
						stream << tc.row();

						Schedule ds(service->getDepartureSchedule(false, 0));
						Schedule as(service->getLastArrivalSchedule(false));

						stream << tc.col() << ds.toString();
						ds += service->getRange();
						stream << tc.col() << ds.toString();

						stream << tc.col() << as.toString();
						as += service->getRange();
						stream << tc.col() << as.toString();

						stream << tc.col() << (as - ds);

						stream << tc.col() << service->getRange();
						stream << tc.col() << service->getMaxWaitingTime();

						stream << tc.col() << to_iso_extended_string(service->getLastActiveDate());
					}

					stream << tc.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}

		bool LineAdmin::isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const
		{
			if (_line.get() == NULL) return false;
			return _request.isAuthorized<TransportNetworkRight>(READ);
		}
		
		

		std::string LineAdmin::getTitle() const
		{
			return _line.get() ? "Route " + _line->getName() : DEFAULT_TITLE;
		}

		boost::shared_ptr<const Line> LineAdmin::getLine() const
		{
			return _line;
		}



		void LineAdmin::_buildTabs(
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const {
			_tabs.clear();

			_tabs.push_back(Tab("Arr�ts desservis", TAB_STOPS, true));
			_tabs.push_back(Tab("Services � horaire", TAB_SCHEDULED_SERVICES, true));
			_tabs.push_back(Tab("Services continus", TAB_CONTINUOUS_SERVICES, true));

			_tabBuilded = true;
		}
		
		void LineAdmin::setLine(boost::shared_ptr<Line> value)
		{
			_line = const_pointer_cast<const Line>(value);
		}
		
		
		bool LineAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _line == static_cast<const LineAdmin&>(other)._line;
		}
			
	}
}
