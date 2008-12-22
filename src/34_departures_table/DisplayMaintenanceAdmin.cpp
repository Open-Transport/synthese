////////////////////////////////////////////////////////////////////////////////
/// DisplayMaintenanceAdmin class implementation.
///	@file DisplayMaintenanceAdmin.cpp
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

#include "DisplayMaintenanceAdmin.h"
#include "DisplayScreen.h"
#include "DisplayType.h"
#include "DisplayScreenTableSync.h"
#include "DeparturesTableModule.h"
#include "UpdateDisplayMaintenanceAction.h"
#include "DisplaySearchAdmin.h"
#include "DisplayAdmin.h"
#include "DisplayMaintenanceLog.h"
#include "PropertiesHTMLTable.h"
#include "05_html/Constants.h"
#include "HTMLList.h"
#include "DBLogViewer.h"
#include "ActionFunctionRequest.h"
#include "QueryString.h"
#include "AdminParametersException.h"
#include "AdminRequest.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayMonitoringStatusTableSync.h"

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
	using namespace time;

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
				_displayScreen = DisplayScreenTableSync::Get(id, _env, UP_LINKS_LOAD_LEVEL);
				DisplayMonitoringStatusTableSync::Search(_env, _displayScreen->getKey(), 0, 0, true, true, UP_LINKS_LOAD_LEVEL);
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
			updateRequest.getAction()->setScreenId(_displayScreen->getKey());
			updateRequest.setObjectId(_displayScreen->getKey());

			FunctionRequest<AdminRequest> goToLogRequest(request);
			goToLogRequest.getFunction()->setPage<DBLogViewer>();
			static_pointer_cast<DBLogViewer,AdminInterfaceElement>(goToLogRequest.getFunction()->getPage())->setLogKey(DisplayMaintenanceLog::FACTORY_KEY);
			goToLogRequest.setObjectId(request->getObjectId());

			stream << "<h1>Paramètres de maintenance</h1>";

			PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));

			stream << t.open();
			stream << t.cell("Afficheur déclaré en service", t.getForm().getOuiNonRadioInput(UpdateDisplayMaintenanceAction::PARAMETER_ONLINE, _displayScreen->getIsOnline()));
			stream << t.cell("Message de maintenance", t.getForm().getTextAreaInput(UpdateDisplayMaintenanceAction::PARAMETER_MESSAGE, _displayScreen->getMaintenanceMessage(), 3, 30));
			stream << t.close();

			stream << "<h1>Informations de supervision</h1>";
			
			HTMLList l;
			stream << l.open();

			stream << l.element() << "Type d'afficheur : ";
			if (_displayScreen->getType() != NULL)
			{
				stream << _displayScreen->getType()->getName();
				stream << l.element() << "Durée théorique entre les contacts de supervision : " << _displayScreen->getType()->getTimeBetweenChecks() << " min";
			}
			else
			{
				stream << "ATTENTION : veuillez définir le type d'afficheur dans l'écran de configuration.";
			}

			if (_env.getRegistry<DisplayMonitoringStatus>().empty())
			{
				stream << l.element() << "ATTENTION : Cet afficheur n'est jamais entré en contact.";
			}
			else
			{
				shared_ptr<DisplayMonitoringStatus> status(_env.getEditableRegistry<DisplayMonitoringStatus>().front());
				if (_displayScreen->getIsOnline() && _now - status->getTime() > _displayScreen->getType()->getTimeBetweenChecks())
				{
					stream << l.element() << "ERREUR : Cet afficheur n'est plus en contact alors qu'il est déclaré online.";
				}

				stream << l.element() << "Dernière mesure le " << status->getTime().toString();
				stream << l.element() << "Dernier état mesuré : "
					<< DisplayMonitoringStatus::GetStatusString(status->getGlobalStatus());
				stream << l.element() << "Température : "
					<< status->getTemperatureValue();
				stream << l.element() << "Détail anomalies : "
					<< status->getDetail();
			}

			stream << l.element("log") << HTMLModule::getHTMLLink(goToLogRequest.getURL(), "Accéder au journal de maintenance de l'afficheur");

			stream << l.close();
		}

		bool DisplayMaintenanceAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return true;
		}

		DisplayMaintenanceAdmin::DisplayMaintenanceAdmin()
		:	AdminInterfaceElementTemplate<DisplayMaintenanceAdmin>(),
			_now(TIME_CURRENT)
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

		std::string DisplayMaintenanceAdmin::getParameterName() const
		{
			return _displayScreen.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string DisplayMaintenanceAdmin::getParameterValue() const
		{
			return _displayScreen.get() ? Conversion::ToString(_displayScreen->getKey()) : string();
		}

		bool DisplayMaintenanceAdmin::isPageVisibleInTree(const AdminInterfaceElement& currentPage) const
		{
			return currentPage.getFactoryKey() == DisplayAdmin::FACTORY_KEY
				&& currentPage.getPageLink().parameterValue == getPageLink().parameterValue;
		}
	}
}
