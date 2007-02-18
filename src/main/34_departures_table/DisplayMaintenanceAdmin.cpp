
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

#include "01_util/Html.h"

#include "32_admin/AdminParametersException.h"

#include "34_departures_table/DisplayMaintenanceAdmin.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/UpdateDisplayMaintenanceAction.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

	namespace departurestable
	{
		DisplayMaintenanceAdmin::DisplayMaintenanceAdmin()
			: AdminInterfaceElement("displays", AdminInterfaceElement::DISPLAYED_IF_CURRENT)
			, _displayScreen(NULL)
		{}

		void DisplayMaintenanceAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			try
			{
				AdminRequest::ParametersMap::const_iterator it = map.find(AdminRequest::PARAMETER_OBJECT_ID);
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

		void DisplayMaintenanceAdmin::display(ostream& stream, const AdminRequest* request) const
		{
			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<DisplayMaintenanceAdmin>());
			updateRequest->setAction(Factory<Action>::create<UpdateDisplayMaintenanceAction>());
			updateRequest->setObjectId(_displayScreen->getKey());

			stream
				<< "<h1>Param�tres de maintenance</h1>"
				
				<< updateRequest->getHTMLFormHeader("update")
				<< "<table>"
				<< "<tr><td>Nombre de contr�les par jour</td><td>" << Html::getSelectNumberInput(UpdateDisplayMaintenanceAction::PARAMETER_CONTROLS, 0, 1440, _displayScreen->getMaintenananceChecksPerDay(), 10) << "</td></tr>"
				<< "<tr><td>Afficheur d�clar� en service</td><td>" << Html::getOuiNonRadioInput(UpdateDisplayMaintenanceAction::PARAMETER_ONLINE, _displayScreen->getIsOnline()) << "</td></tr>"
				<< "<tr><td>Message de maintenance</td><td>" << Html::getTextAreaInput(UpdateDisplayMaintenanceAction::PARAMETER_MESSAGE, _displayScreen->getMaintenanceMessage(), 30, 3) << "</td></tr>"
				<< "<tr><td colspan=\"2\">" << Html::getSubmitButton("Enregistrer les modifications") << "</td></tr>"
				<< "</table></form>"

				<< "<h1>Contr�le de coh�rence des donn�es</h1>"
				<< "<table>"
				<< "<tr><td>Etat</td><td>" /*pastille* <FONT face=\"Wingdings\" color=\"#00cc00\">
				<FONT color=\"#ff9900\">l<FONT face=\"Verdana\"> <STRONG>Warning </STRONG><EM>(depuis 4j 22h 
				35min)</EM></FONT></FONT></FONT></P>" */ << "</td></tr>"
				<< "<tr><td>Motif de l'alerte</td><td>Aucune ligne ne dessert l'afficheur</td></tr>"
				<< "<tr><td>Date du dernier contr�le positif</td><td>13/9/2006 8:30</td></tr>"
				<< "</table>"
				<< "<h1>Contr�le du mat�riel d'affichage</h1>"
				<< "<table>"
				<< "<tr><td>Etat</td><td>" /*^pastille <FONT face=\"Wingdings\"><FONT color=\"#ff0000\">l<FONT face=\"Verdana\">
				<STRONG>Error </STRONG><EM>(depuis 1j 12h 23min)</EM></FONT></FONT></FONT></P> */ << "</td><tr>"
				<< "<tr><td>Date du dernier contr�le</td><td>21/09/2006 16:30</td></tr>"
				<< "<tr><td>Date du dernier contr�le positif : 20/09/2006 15:30</td></tr>"
				<< "<tr><td>Motif de l'alerte</td><td>Temp�rature trop �lev�e, terminal �teint</td></tr>"
				<< "<tr><td colspan=\"2\"><a href=\"admin_log_viewer.htm\">Acc�der au journal de maintenance de l'afficheur</a></td></tr>"
				<< "</table>";
		}
	}
}
