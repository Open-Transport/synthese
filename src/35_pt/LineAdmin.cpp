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
#include "Profile.h"
#include "CommercialLine.h"
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
#include "ServiceAdmin.h"
#include "Request.h"
#include "AdminFunctionRequest.hpp"

#include "AdminParametersException.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;


namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace env;
	using namespace html;
	using namespace security;
	using namespace graph;
	using namespace pt;
	

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, LineAdmin>::FACTORY_KEY("LineAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<LineAdmin>::ICON("chart_line.png");
		template<> const string AdminInterfaceElementTemplate<LineAdmin>::DEFAULT_TITLE("Ligne inconnue");
	}

	namespace pt
	{
		const string LineAdmin::TAB_STOPS("stops");
		const string LineAdmin::TAB_SCHEDULED_SERVICES("sserv");
		const string LineAdmin::TAB_CONTINUOUS_SERVICES("cserv");
		const string LineAdmin::TAB_INDICES("in");

		LineAdmin::LineAdmin()
			: AdminInterfaceElementTemplate<LineAdmin>()
		{ }
		
		void LineAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_line = LineTableSync::Get(
					map.getUid(Request::PARAMETER_OBJECT_ID, true, FACTORY_KEY),
					Env::GetOfficialEnv()
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


		
		void LineAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const admin::AdminRequest& _request
		) const	{
			map<const Service*, string> services;

			////////////////////////////////////////////////////////////////////
			// TAB STOPS
			if (openTabContent(stream, TAB_STOPS))
			{
				// Reservation
// 				bool reservation(_line->getReservationRule() && _line->getReservationRule()->getType() == RESERVATION_COMPULSORY);
				LineStopTableSync::SearchResult lineStops(
					LineStopTableSync::Search(
						Env::GetOfficialEnv(),
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
				v.push_back("Arrêt");
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
				ScheduledServiceTableSync::SearchResult sservices(
					ScheduledServiceTableSync::Search(
						Env::GetOfficialEnv(),
						_line->getKey(),
						optional<RegistryKeyType>(),
						optional<RegistryKeyType>(),
						optional<string>(),
						optional<date>(),
						false,
						0,
						optional<size_t>(),
						true, true,
						UP_DOWN_LINKS_LOAD_LEVEL
				)	);
				if (sservices.empty())
					stream << "<p>Aucun service à horaire</p>";
				else
				{
					AdminFunctionRequest<ServiceAdmin> serviceRequest(_request);

					HTMLTable::ColsVector vs;
					vs.push_back("Num");
					vs.push_back("Numéro");
					vs.push_back("Départ");
					vs.push_back("Arrivée");
					vs.push_back("Durée");
					vs.push_back("Dernier jour");
					HTMLTable ts(vs,"adminresults");

					stream << ts.open();

					size_t i(0);
					BOOST_FOREACH(shared_ptr<ScheduledService> service, sservices)
					{
						serviceRequest.getPage()->setService(service);

						string number("S"+ lexical_cast<string>(i++));
						services[service.get()] = number;

						time_duration ds(service->getDepartureSchedule(false, 0));
						time_duration as(service->getLastArrivalSchedule(false));

						stream << ts.row();

						stream << ts.col() << number;

						stream << ts.col() << service->getServiceNumber();

						stream << ts.col() << ds;
						stream << ts.col() << as;

						stream << ts.col() << (as - ds);

						stream << ts.col() << to_iso_extended_string(service->getLastActiveDate());

						stream << ts.col() << HTMLModule::getLinkButton(serviceRequest.getURL(), "Ouvrir", string(), ServiceAdmin::ICON);
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
						Env::GetOfficialEnv(),
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
					AdminFunctionRequest<ServiceAdmin> serviceRequest(_request);

					HTMLTable::ColsVector vc;
					vc.push_back("Num");
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

					size_t i(0);
					BOOST_FOREACH(shared_ptr<ContinuousService> service, cservices)
					{
						serviceRequest.getPage()->setService(service);

						string number("C"+ lexical_cast<string>(i++));
						services[service.get()] = number;

						stream << tc.row();

						stream << tc.col() << number;

						time_duration ds(service->getDepartureSchedule(false, 0));
						time_duration as(service->getLastArrivalSchedule(false));

						stream << tc.col() << ds;
						ds += service->getRange();
						stream << tc.col() << ds;

						stream << tc.col() << as;
						as += service->getRange();
						stream << tc.col() << as;

						stream << tc.col() << (as - ds);

						stream << tc.col() << service->getRange();
						stream << tc.col() << service->getMaxWaitingTime();

						stream << tc.col() << to_iso_extended_string(service->getLastActiveDate());

						stream << tc.col() << HTMLModule::getLinkButton(serviceRequest.getURL(), "Ouvrir", string(), ServiceAdmin::ICON);
					}

					stream << tc.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB INDICES
			if (openTabContent(stream, TAB_INDICES))
			{
				stream << "<style>td.red {background-color:red;width:8px; height:8px; color:white; text-align:center; } td.green {background-color:#008000;width:10px; height:10px; color:white; text-align:center; } .mini {font-size:9px;}</style>"; 
				HTMLTable::ColsVector cols;
				cols.push_back("Arrêt");
				cols.push_back("D/A");
				for(int i(0); i<=23; ++i)
				{
					cols.push_back(Conversion::ToString(i));
				}
				HTMLTable t(cols, ResultHTMLTable::CSS_CLASS);
				stream << t.open();

				BOOST_FOREACH(const Path::Edges::value_type& edge, _line->getEdges())
				{
					const LineStop& lineStop(dynamic_cast<const LineStop&>(*edge));
					lineStop.getDepartureFromIndex(false,0);
					lineStop.getDepartureFromIndex(true,0);

					if(lineStop.isArrival())
					{
						stream << t.row();
						stream << t.col(1, string(), true) << lineStop.getPhysicalStop()->getConnectionPlace()->getFullName();
						stream << t.col(1, string(), true) << "A";

						BOOST_FOREACH(const Edge::ArrivalServiceIndices::value_type& index, lineStop.getArrivalIndices())
						{
							stream << t.col();

							if(index.get(false) == lineStop.getParentPath()->getServices().rend())
							{
								stream << "-";
							}
							else
							{
								const Service* service(*index.get(false));
								stream << services[service];
								stream << "<br /><span class=\"mini\">" << service->getArrivalBeginScheduleToIndex(false, lineStop.getRankInPath()) << "</span>";
							}
						}
					}

					if(lineStop.isDeparture())
					{
						stream << t.row();
						stream << t.col(1, string(), true) << lineStop.getPhysicalStop()->getConnectionPlace()->getFullName();
						stream << t.col(1, string(), true) << "D";

						BOOST_FOREACH(const Edge::DepartureServiceIndices::value_type& index, lineStop.getDepartureIndices())
						{
							stream << t.col();

							if(index.get(false) == lineStop.getParentPath()->getServices().end())
							{
								stream << "-";
							}
							else
							{
								const Service* service(*index.get(false));
								stream << services[service];
								stream << "<br /><span class=\"mini\">" << service->getDepartureBeginScheduleToIndex(false, lineStop.getRankInPath()) << "</span>";
							}
						}
					}
				}
				stream << t.close();

			}

			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		bool LineAdmin::isAuthorized(
			const security::User& user
		) const	{
			if (_line.get() == NULL) return false;
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
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
			const security::Profile& profile
		) const {
			_tabs.clear();

			_tabs.push_back(Tab("Arrêts desservis", TAB_STOPS, true));
			_tabs.push_back(Tab("Services à horaire", TAB_SCHEDULED_SERVICES, true));
			_tabs.push_back(Tab("Services continus", TAB_CONTINUOUS_SERVICES, true));
			_tabs.push_back(Tab("Index", TAB_INDICES, true));

			_tabBuilded = true;
		}
		


		void LineAdmin::setLine(boost::shared_ptr<const Line> value)
		{
			_line = value;
		}
		
		

		bool LineAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _line->getKey() == static_cast<const LineAdmin&>(other)._line->getKey();
		}



		AdminInterfaceElement::PageLinks LineAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	currentPage == *this ||
				currentPage.getCurrentTreeBranch().find(*this)
			){
				ScheduledServiceTableSync::SearchResult services(
					ScheduledServiceTableSync::Search(*_env, _line->getKey())
				);
				BOOST_FOREACH(shared_ptr<const ScheduledService> service, services)
				{
					shared_ptr<ServiceAdmin> p(
						getNewOtherPage<ServiceAdmin>()
					);
					p->setService(service);
					links.push_back(p);
				}
				ContinuousServiceTableSync::SearchResult cservices(
					ContinuousServiceTableSync::Search(*_env, _line->getKey())
				);
				BOOST_FOREACH(shared_ptr<const ContinuousService> service, cservices)
				{
					shared_ptr<ServiceAdmin> p(
						getNewOtherPage<ServiceAdmin>()
					);
					p->setService(service);
					links.push_back(p);
				}
			}
			return links;

		}



		AdminInterfaceElement::PageLinks LineAdmin::_getCurrentTreeBranch() const
		{
			shared_ptr<CommercialLineAdmin> p(
				getNewOtherPage<CommercialLineAdmin>()
			);
			p->setCommercialLine(Env::GetOfficialEnv().getSPtr(_line->getCommercialLine()));

			PageLinks links(p->_getCurrentTreeBranch());
			links.push_back(p);
			return links;
		}
	}
}
