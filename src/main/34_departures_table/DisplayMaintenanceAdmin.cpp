
/** DisplayMaintenanceAdmin class implementation.
	@file DisplayMaintenanceAdmin.cpp

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
#include "34_departures_table/DisplayMaintenanceAdmin.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayScreenTableSync.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/UpdateDisplayMaintenanceAction.h"
#include "34_departures_table/DisplaySearchAdmin.h"

#include "05_html/PropertiesHTMLTable.h"
#include "05_html/Constants.h"

#include "13_dblog/DBLogViewer.h"

#include "30_server/ActionFunctionRequest.h"
#include "30_server/QueryString.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace dblog;
	using namespace departurestable;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DisplayMaintenanceAdmin>::FACTORY_KEY("dmaint");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DisplayMaintenanceAdmin>::ICON("monitor_lightning.png");
		template<> const string AdminInterfaceElementTemplate<DisplayMaintenanceAdmin>::DEFAULT_TITLE("Supervision et maintenance");
	}

	namespace departurestable
	{
		void DisplayMaintenanceAdmin::setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));

			try
			{
				_displayScreen = DisplayScreenTableSync::Get(id);
				_pageLink.parameterName = QueryString::PARAMETER_OBJECT_ID;
				_pageLink.parameterValue = Conversion::ToString(id);
			}
			catch(...)
			{
				throw AdminParametersException("Specified display screen not found");
			}
		}


		void DisplayMaintenanceAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			ActionFunctionRequest<UpdateDisplayMaintenanceAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<DisplayMaintenanceAdmin>();
			updateRequest.setObjectId(_displayScreen->getKey());

			FunctionRequest<AdminRequest> goToLogRequest(request);
			goToLogRequest.getFunction()->setPage<DBLogViewer>();
			goToLogRequest.getFunction()->setParameter(DBLogViewer::PARAMETER_LOG_KEY, "displaymaintenance");
			goToLogRequest.setObjectId(request->getObjectId());

			FunctionRequest<AdminRequest> goToDataLogRequest(request);
			goToDataLogRequest.getFunction()->setPage<DBLogViewer>();
			goToDataLogRequest.getFunction()->setParameter(DBLogViewer::PARAMETER_LOG_KEY, "displaydata");
			goToDataLogRequest.setObjectId(request->getObjectId());

			stream << "<h1>Paramètres de maintenance</h1>";

			PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));

			stream << t.open();
			stream << t.cell("Nombre de contrôles par jour", t.getForm().getSelectNumberInput(UpdateDisplayMaintenanceAction::PARAMETER_CONTROLS, 0, 1440, _displayScreen->getMaintenanceChecksPerDay(), 10));
			stream << t.cell("Afficheur déclaré en service", t.getForm().getOuiNonRadioInput(UpdateDisplayMaintenanceAction::PARAMETER_ONLINE, _displayScreen->getIsOnline()));
			stream << t.cell("Message de maintenance", t.getForm().getTextAreaInput(UpdateDisplayMaintenanceAction::PARAMETER_MESSAGE, _displayScreen->getMaintenanceMessage(), 3, 30));
			stream << t.close();

			stream << "<h1>Contrôle de cohérence des données</h1>";
			
			HTMLTable ct;
			stream << ct.open();

			stream << ct.row();
			stream << ct.col() << "Etat";
			stream << ct.col();
			if (_displayScreen->getComplements().dataControl == DISPLAY_DATA_CORRUPTED)
				stream << HTMLModule::getHTMLImage(IMG_URL_ERROR, _displayScreen->getComplements().dataControlText);
			if (_displayScreen->getComplements().dataControl == DISPLAY_DATA_NO_LINES)
				stream << HTMLModule::getHTMLImage(IMG_URL_WARNING, _displayScreen->getComplements().dataControlText);
			if (_displayScreen->getComplements().dataControl == DISPLAY_DATA_OK)
				stream << HTMLModule::getHTMLImage(IMG_URL_INFO, "OK");


			stream << ct.row();
			stream << ct.col() << "Détail";
			stream << ct.col() << _displayScreen->getComplements().dataControlText;

			stream << ct.row();
			stream << ct.col() << "Date du dernier contrôle positif";
			stream << ct.col() << _displayScreen->getComplements().lastOKDataControl.toString();

			stream << ct.row();
			stream << ct.col(2) << HTMLModule::getLinkButton(goToDataLogRequest.getURL(), "Accéder au journal de surveillance des données de l'afficheur", string(), "book_open.png");

			stream << ct.close();

			stream << "<h1>Contrôle du matériel d'affichage</h1>";

			HTMLTable mt;
			stream << mt.open();

			stream << mt.row();
			stream << mt.col() << "Etat";
			stream << mt.col();
			if (_displayScreen->getComplements().status == DISPLAY_STATUS_NO_NEWS_WARNING
				|| _displayScreen->getComplements().status == DISPLAY_STATUS_HARDWARE_WARNING)
				stream << HTMLModule::getHTMLImage(IMG_URL_WARNING, _displayScreen->getComplements().statusText);
			if (_displayScreen->getComplements().status == DISPLAY_STATUS_NO_NEWS_ERROR
				|| _displayScreen->getComplements().status == DISPLAY_STATUS_HARDWARE_ERROR)
				stream << HTMLModule::getHTMLImage(IMG_URL_ERROR, _displayScreen->getComplements().statusText);
			if (_displayScreen->getComplements().status == DISPLAY_STATUS_OK)
				stream << HTMLModule::getHTMLImage(IMG_URL_INFO, "OK");

			stream << mt.row();
			stream << mt.col() << "Détail";
			stream << mt.col() << _displayScreen->getComplements().statusText;

			stream << mt.row();
			stream << mt.col() << "Date du dernier contrôle";
			stream << mt.col() << _displayScreen->getComplements().lastControl.toString();

			stream << mt.row();
			stream << mt.col() << "Date du dernier contrôle positif";
			stream << mt.col() << _displayScreen->getComplements().lastOKStatus.toString();

			stream << mt.row();
			stream << mt.col(2) << HTMLModule::getLinkButton(goToLogRequest.getURL(), "Accéder au journal de maintenance de l'afficheur", string(), "book_open.png");

			stream << mt.close();
		}

		bool DisplayMaintenanceAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return true;
		}

		DisplayMaintenanceAdmin::DisplayMaintenanceAdmin()
			: AdminInterfaceElementTemplate<DisplayMaintenanceAdmin>()
		{
	
		}

		AdminInterfaceElement::PageLinks DisplayMaintenanceAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			return AdminInterfaceElement::PageLinks();
		}

		AdminInterfaceElement::PageLinks DisplayMaintenanceAdmin::getSubPages( const AdminInterfaceElement& currentPage, const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return AdminInterfaceElement::PageLinks();
		}

		boost::shared_ptr<const DisplayScreen> DisplayMaintenanceAdmin::getDisplayScreen() const
		{
			return _displayScreen;
		}
	}
}
