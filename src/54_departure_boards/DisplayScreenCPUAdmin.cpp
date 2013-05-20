////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenCPUAdmin class implementation.
///	@file DisplayScreenCPUAdmin.cpp
///	@author Hugues Romain
///	@date 2008-12-26 16:37
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include "DisplayScreenCPUAdmin.h"

#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUUpdateAction.h"
#include "DisplayScreenCPUTableSync.h"
#include "DisplaySearchAdmin.h"
#include "DeparturesTableModule.h"
#include "AdminInterfaceElement.h"
#include "StaticActionFunctionRequest.h"
#include "User.h"
#include "AdminParametersException.h"
#include "ArrivalDepartureTableRight.h"
#include "ArrivalDepartureTableLog.h"
#include "PropertiesHTMLTable.h"
#include "ResultHTMLTable.h"
#include "DisplayAdmin.h"
#include "DisplayScreenTableSync.h"
#include "CreateDisplayScreenAction.h"
#include "ObjectNotFoundException.h"
#include "DisplayMonitoringStatusTableSync.h"
#include "DisplayScreenCPUMaintenanceUpdateAction.h"
#include "DisplayMaintenanceLog.h"
#include "HTMLList.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "Profile.h"

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace departure_boards;
	using namespace security;
	using namespace html;
	using namespace util;


	namespace util
	{
		template<> const string util::FactorableTemplate<AdminInterfaceElement, DisplayScreenCPUAdmin>::FACTORY_KEY("DisplayScreenCPUAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DisplayScreenCPUAdmin>::ICON("server.png");
		template<> const string AdminInterfaceElementTemplate<DisplayScreenCPUAdmin>::DEFAULT_TITLE("Unité centrale");
	}

	namespace departure_boards
	{
		const string DisplayScreenCPUAdmin::TAB_DISPLAYS("di");
		const string DisplayScreenCPUAdmin::TAB_MAINTENANCE("ma");
		const string DisplayScreenCPUAdmin::TAB_LOG("lo");
		const string DisplayScreenCPUAdmin::TAB_TECHNICAL("te");

		DisplayScreenCPUAdmin::DisplayScreenCPUAdmin()
		:	AdminInterfaceElementTemplate<DisplayScreenCPUAdmin>(),
			_generalLogView(TAB_LOG),
			_maintenanceLogView(TAB_MAINTENANCE)
		{ }



		void DisplayScreenCPUAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_cpu = DisplayScreenCPUTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					_getEnv()
				);
			}
			catch (ObjectNotFoundException<DisplayScreenCPU>& e)
			{
				throw AdminParametersException("Specified central unit "+ FACTORY_KEY +" not found "+ e.getMessage());
			}

			_generalLogView.set(map, ArrivalDepartureTableLog::FACTORY_KEY, _cpu->getKey());
			_maintenanceLogView.set(map, DisplayMaintenanceLog::FACTORY_KEY, _cpu->getKey());
		}



		util::ParametersMap DisplayScreenCPUAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_cpu.get()) m.insert(Request::PARAMETER_OBJECT_ID, _cpu->getKey());
			return m;
		}



		void DisplayScreenCPUAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TECHNICAL TAB
			if (openTabContent(stream, TAB_TECHNICAL))
			{
				AdminActionFunctionRequest<DisplayScreenCPUUpdateAction, DisplayScreenCPUAdmin> updateRequest(_request, *this);
				updateRequest.getAction()->setCPU(_cpu->getKey());

				stream << "<h1>Propriétés</h1>";

				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				stream << t.open();
				stream << t.cell("ID", lexical_cast<string>(_cpu->getKey()));
				stream << t.cell("Nom", t.getForm().getTextInput(DisplayScreenCPUUpdateAction::PARAMETER_NAME, _cpu->getName()));
				stream << t.cell("Adresse MAC", t.getForm().getTextInput(DisplayScreenCPUUpdateAction::PARAMETER_MAC_ADDRESS, _cpu->getMacAddress()));
				stream << t.close();

			}


			////////////////////////////////////////////////////////////////////
			// MAINTENANCE TAB
			if (openTabContent(stream, TAB_MAINTENANCE))
			{
				// Update action
				AdminActionFunctionRequest<DisplayScreenCPUMaintenanceUpdateAction,DisplayScreenCPUAdmin> updateRequest(_request, *this);
				updateRequest.getAction()->setCPU(_cpu->getKey());

				// Log search
				AdminFunctionRequest<DisplayScreenCPUAdmin> searchRequest(_request, *this);

				stream << "<h1>Paramètres de maintenance</h1>";

				PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
				t.getForm().setUpdateRight(tabHasWritePermissions());

				stream << t.open();
				stream <<
					t.cell(
						"Unité centrale déclarée en service",
						t.getForm().getOuiNonRadioInput(
							DisplayScreenCPUMaintenanceUpdateAction::PARAMETER_IS_ONLINE,
							_cpu->getIsOnline()
					)	)
				;
				stream <<
					t.cell(
						"Message de maintenance",
						t.getForm().getTextAreaInput(
							DisplayScreenCPUMaintenanceUpdateAction::PARAMETER_MAINTENANCE_MESSAGE,
							_cpu->getMaintenanceMessage(),
							3, 60, false
					)	)
				;
				stream <<
					t.cell(
						"Durée entre les requêtes",
						t.getForm().getSelectNumberInput(
							DisplayScreenCPUMaintenanceUpdateAction::PARAMETER_MONITORING_DELAY,
							1, 120,
							_cpu->getMonitoringDelay().minutes(),
							1
						)+ " minutes"
					)
				;
				stream << t.close();

				stream << "<h1>Informations de supervision</h1>";

				bool monitored(_cpu->isMonitored());

				HTMLList l;
				stream << l.open();


				if(!monitored)
				{
					stream <<
						l.element() <<
						HTMLModule::getHTMLImage("/admin/img/help.png", "Information") <<
						" Cette unité centrale n'est pas supervisée."
						;
				} else {

					boost::posix_time::ptime _lastContact(
						DisplayMonitoringStatusTableSync::GetLastContact(*_cpu)
					);

					stream <<
						l.element() <<
						"Durée théorique entre les contacts : " <<
						_cpu->getMonitoringDelay().minutes() << " min"
					;


					if(_lastContact.is_not_a_date_time())
					{
						stream <<
							l.element() <<
							HTMLModule::getHTMLImage("/admin/img/exclamation.png", "Statut KO") <<
							" KO : Cette unité centrale n'est jamais entré en contact.";
					}
					else
					{
						if(	_cpu->isDown(_lastContact)
						){
							stream <<
								l.element() <<
								HTMLModule::getHTMLImage("/admin/img/exclamation.png", "Statut KO") <<
								" KO : Cette unité centrale n'est plus en contact alors qu'elle est déclarée online."
							;
						}
					}

					if(!_lastContact.is_not_a_date_time())
					{
						stream << l.element() << "Dernier contact le " << to_simple_string(_lastContact);

					}
				}

				stream << l.close();

				stream << "<h1>Journal de maintenance</h1>";

				_maintenanceLogView.display(
					stream,
					searchRequest
				);
			}


			////////////////////////////////////////////////////////////////////
			// DISPLAYS TAB
			if (openTabContent(stream, TAB_DISPLAYS))
			{
				AdminFunctionRequest<DisplayAdmin> displayRequest(_request);

				AdminActionFunctionRequest<CreateDisplayScreenAction,DisplayAdmin> createDisplayRequest(
					_request
				);
				createDisplayRequest.setActionWillCreateObject();
				createDisplayRequest.getAction()->setCPU(_cpu);

				DisplayScreenTableSync::SearchResult screens(
					DisplayScreenTableSync::SearchFromCPU(_getEnv(), _cpu->getKey())
				);

				HTMLTable::ColsVector c;
				c.push_back("Nom");
				c.push_back("Port COM");
				c.push_back("Code BUS");
				c.push_back("ID");
				c.push_back("Actions");
				HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(const boost::shared_ptr<DisplayScreen>& screen, screens)
				{
					displayRequest.getPage()->setScreen(screen);
					stream << t.row();
					stream << t.col() << screen->getName();
					stream << t.col() << screen->getComPort();
					stream << t.col() << screen->getWiringCode();
					stream << t.col() << screen->getKey();
					stream << t.col() << displayRequest.getHTMLForm().getLinkButton("Ouvrir", string(), "monitor.png");
				}

				stream << t.close();

				stream << createDisplayRequest.getHTMLForm().getLinkButton("Créer un nouvel afficheur sur l'unité centrale", string(), "monitor_add.png");
			}

			////////////////////////////////////////////////////////////////////
			// LOG TAB
			if (openTabContent(stream, TAB_LOG))
			{
				_generalLogView.display(stream,	_request);
			}
			closeTabContent(stream);
		}

		bool DisplayScreenCPUAdmin::isAuthorized(
			const security::User& user
		) const	{
			if (_cpu.get() == NULL) return false;
			if (_cpu->getPlace() == NULL)
			{
				return user.getProfile()->isAuthorized<ArrivalDepartureTableRight>(READ);
			}
			return user.getProfile()->isAuthorized<ArrivalDepartureTableRight>(
				READ,
				UNKNOWN_RIGHT_LEVEL,
				lexical_cast<string>(_cpu->getPlace()->getKey())
			);
		}



		std::string DisplayScreenCPUAdmin::getTitle() const
		{
			return _cpu.get() ? _cpu->getName() : DEFAULT_TITLE;
		}


		void DisplayScreenCPUAdmin::_buildTabs(
			const security::Profile& profile
		) const {
			_tabs.clear();

			bool writePermission(
				_cpu->getPlace() ?
				profile.isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_cpu->getPlace()->getKey())) :
				profile.isAuthorized<ArrivalDepartureTableRight>(WRITE)
			);
			_tabs.push_back(Tab("Technique", TAB_TECHNICAL, writePermission, "cog.png"));

			if(_cpu->getPlace())
			{
				_tabs.push_back(Tab("Maintenance", TAB_MAINTENANCE, writePermission, "wrench.png"));
				_tabs.push_back(Tab("Afficheurs", TAB_DISPLAYS, writePermission, "monitor.png"));
			}
			_tabs.push_back(Tab("Journal", TAB_LOG, writePermission, "book.png"));

			_tabBuilded = true;
		}

		void DisplayScreenCPUAdmin::setCPU(boost::shared_ptr<const DisplayScreenCPU> value)
		{
			_cpu = value;
		}

		boost::shared_ptr<const DisplayScreenCPU> DisplayScreenCPUAdmin::getCPU() const
		{
			return _cpu;
		}

		bool DisplayScreenCPUAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _cpu == static_cast<const DisplayScreenCPUAdmin&>(other)._cpu;
		}



		AdminInterfaceElement::PageLinks DisplayScreenCPUAdmin::getSubPages( const AdminInterfaceElement& currentPage, const server::Request& request ) const
		{
			AdminInterfaceElement::PageLinks links;

			DisplayScreenTableSync::SearchResult screens(
				DisplayScreenTableSync::SearchFromCPU(_getEnv(), _cpu->getKey())
			);
			BOOST_FOREACH(const boost::shared_ptr<DisplayScreen>& screen, screens)
			{
				boost::shared_ptr<DisplayAdmin> p(getNewPage<DisplayAdmin>());
				p->setScreen(screen);
				links.push_back(p);
			}

			return links;
		}

	}
}
