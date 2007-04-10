
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

#include "05_html/HTMLForm.h"
#include "05_html/HTMLTable.h"

#include "13_dblog/DBLogViewer.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminParametersException.h"

#include "34_departures_table/DisplayMaintenanceAdmin.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/UpdateDisplayMaintenanceAction.h"

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

	namespace departurestable
	{
		DisplayMaintenanceAdmin::DisplayMaintenanceAdmin()
			: AdminInterfaceElement("displays", AdminInterfaceElement::DISPLAYED_IF_CURRENT)
			{}

		void DisplayMaintenanceAdmin::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it = map.find(Request::PARAMETER_OBJECT_ID);
				if (it == map.end())
					throw AdminParametersException("Screen not specified");

				_displayScreen = DeparturesTableModule::getDisplayScreens().get(Conversion::ToLongLong(it->second));
			}
			catch (DisplayScreen::RegistryKeyException e)
			{
				throw AdminParametersException("Specified display screen not found (" + Conversion::ToString(e.getKey()) +")");
			}
		}

		string DisplayMaintenanceAdmin::getTitle() const
		{
			return "Supervision de " + _displayScreen->getFullName();
		}

		void DisplayMaintenanceAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			ActionFunctionRequest<UpdateDisplayMaintenanceAction,AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DisplayMaintenanceAdmin>());
			updateRequest.setObjectId(_displayScreen->getKey());

			FunctionRequest<AdminRequest> goToLogRequest(request);
			goToLogRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<DBLogViewer>());
			goToLogRequest.getFunction()->setParameter(DBLogViewer::PARAMETER_LOG_KEY, "displaymaintenance");
			goToLogRequest.setObjectId(request->getObjectId());

			stream << "<h1>Paramètres de maintenance</h1>";

			HTMLForm f(updateRequest.getHTMLForm("update"));
			HTMLTable t;
			
			stream << f.open() << t.open();

			stream << t.row();
			stream << t.col() << "Nombre de contrôles par jour";
			stream << t.col() << f.getSelectNumberInput(UpdateDisplayMaintenanceAction::PARAMETER_CONTROLS, 0, 1440, _displayScreen->getMaintenananceChecksPerDay(), 10);

			stream << t.row();
			stream << t.col() << "Afficheur déclaré en service";
			stream << t.col() << f.getOuiNonRadioInput(UpdateDisplayMaintenanceAction::PARAMETER_ONLINE, _displayScreen->getIsOnline());

			stream << t.row();
			stream << t.col() << "Message de maintenance";
			stream << t.col() << f.getTextAreaInput(UpdateDisplayMaintenanceAction::PARAMETER_MESSAGE, _displayScreen->getMaintenanceMessage(), 3, 30);

			stream << t.row();
			stream << t.col(2) << f.getSubmitButton("Enregistrer les modifications");

			stream << t.close() << f.close();

			stream << "<h1>Contrôle de cohérence des données</h1>";
			
			HTMLTable ct;
			stream << ct.open();

			stream << ct.row();
			stream << ct.col() << "Etat";
			stream << ct.col() /* << pastille* <FONT face=\"Wingdings\" color=\"#00cc00\">
				<FONT color=\"#ff9900\">l<FONT face=\"Verdana\"> <STRONG>Warning </STRONG><EM>(depuis 4j 22h 
				35min)</EM></FONT></FONT></FONT></P>" */;

			stream << ct.row();
			stream << ct.col() << "Motif de l'alerte";
			stream << ct.col() << "Aucune ligne ne dessert l'afficheur";

			stream << ct.row();
			stream << ct.col() << "Date du dernier contrôle positif";
			stream << ct.col() << "13/9/2006 8:30";

			stream << ct.close();

			stream << "<h1>Contrôle du matériel d'affichage</h1>";

			HTMLTable mt;
			stream << mt.open();

			stream << mt.row();
			stream << mt.col() << "Etat";
			stream << mt.col() /*<< ^pastille <FONT face=\"Wingdings\"><FONT color=\"#ff0000\">l<FONT face=\"Verdana\">
				<STRONG>Error </STRONG><EM>(depuis 1j 12h 23min)</EM></FONT></FONT></FONT></P> */;

			stream << mt.row();
			stream << mt.col() << "Date du dernier contrôle";
			stream << mt.col() << "21/09/2006 16:30";

			stream << mt.row();
			stream << mt.col() << "Date du dernier contrôle positif";
			stream << mt.col() << "20/09/2006 15:30";

			stream << mt.row();
			stream << mt.col() << "Motif de l'alerte";
			stream << mt.col() << "Température trop élevée, terminal éteint";

			stream << mt.row();
			stream << mt.col(2) << HTMLModule::getLinkButton(goToLogRequest.getURL(), "Accéder au journal de maintenance de l'afficheur");

			stream << mt.close();
		}

		bool DisplayMaintenanceAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return true;
		}
	}
}
