
/** ProfileAdmin class implementation.
	@file ProfileAdmin.cpp

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

#include "02_db/DBEmptyResultException.h"

#include "12_security/Profile.h"
#include "12_security/ProfileAdmin.h"
#include "12_security/ProfileTableSync.h"

#include "32_admin/AdminRequest.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace db;

	namespace security
	{
		const string ProfileAdmin::PARAM_PROFILE_ID = "roid";

		ProfileAdmin::ProfileAdmin()
			: AdminInterfaceElement("profiles", AdminInterfaceElement::DISPLAYED_IF_CURRENT)
			, _profile(NULL), _profileError(false)
		{

		}

		std::string ProfileAdmin::getTitle() const
		{
			return (_profile != NULL)
				? _profile->getName()
				: "";
		}

		void ProfileAdmin::display(std::ostream& stream, const Request* request) const
		{
/*			stream
				<< "<P>Nom : <INPUT id="Text2" type="text" value="RL 14 41" name="Text2"><INPUT id="Button2" type="button" value="Modifier" name="Button7"></P>"
				<< "<P>Habilitations du profil&nbsp;:</P>"
				<< "<P>"
				<< "<TABLE><TR>"
				<< "<TD style=\"WIDTH: 140px\"><STRONG><FONT size=\"2\" color=\"#000000\">Nature</FONT></STRONG></TD>"
				<< "<TD align=\"center\" style=\"WIDTH: 82px\"><STRONG><FONT color=\"#000000\" size=\"2\">Périmètre</FONT></STRONG></TD>"
				<< "<TD style=\"WIDTH: 56px\" align=\"center\"><STRONG><FONT color=\"#000000\" size=\"2\">Type</FONT></STRONG></TD>"
				<< "<TD align=\"center\" style=\"WIDTH: 94px\"><STRONG><FONT color=\"#000000\" size=\"2\">Droit</FONT></STRONG></TD>"
				<< "<TD align=\"center\" colSpan=\"1\" rowSpan=\"1\"><STRONG><FONT size=\"2\" color=\"#000000\">Actions</FONT></STRONG></TD>"
				<< "</TR>";
*/
			// Habilitations list
/*				<< "<tr><TD style=\"WIDTH: 140px\"><FONT color=\"#000000\" size=\"1\">Tout</FONT></TD>"
				<< "<TD style=\"WIDTH: 82px\"><FONT color=\"#000000\" size=\"1\">Tout</FONT></TD>"
				<< "<TD style=\"WIDTH: 56px\"><FONT color=\"#000000\" size=\"1\">Public</FONT></TD>"
				<< "<TD style=\"WIDTH: 94px\"><FONT color=\"#000000\" size=\"1\">Utilisation</FONT></TD>"
				<TD><FONT size=\"1\"><INPUT language=\"javascript\" id=\"Button6\" onclick=\"alert('Etes vous sur de vouloir supprimer l\'habilitation sélectionnées ?')\"
				type=\"button\" value=\"Supprimer\" name=\"Modifier\"><FONT color=\"#000000\"></FONT></FONT></TD>
				</TR>
*/				
/*			stream
				<< "<TR>"
				<< "<TD style=\"WIDTH: 140px\"><FONT size=\"1\"><SELECT id=\"Select1\" name=\"Select1\">"
				<< "<OPTION value=\"\" selected>Tableaux de départs</OPTION>"
				<< "<OPTION value=\"\">Environnement</OPTION>"
				<< "</SELECT></FONT></TD>"
				<< "<TD style=\"WIDTH: 82px\"><SELECT id=\"Select4\" name=\"Select4\">";

			// numéro lignes

			stream
				<< "</SELECT></TD>"

				<< "<TD style=\"WIDTH: 56px\"><SELECT id=\"Select5\" name=\"Select2\">"
				<< "<OPTION value=\"\" selected>Public</OPTION>"
				<< "<OPTION value=\"\">Privé</OPTION>"
				<< "</SELECT></TD>"
				<< "<TD style=\"WIDTH: 94px\"><SELECT id=\"Select3\" name=\"Select2\">"
				<< "<OPTION value=\"\" selected>Utilisation</OPTION>"
				<< "<OPTION value=\"\">Lecture</OPTION>"
				<< "<OPTION value=\"\">Modification</OPTION>"
				<< "<OPTION value=\"\">Suppression</OPTION>"
				<< "</SELECT></TD>"
				<< "<TD><INPUT id=\"Button7\" type=\"button\" value=\"Ajouter\" name=\"Button7\"></TD>"
				<< "</TR></TABLE>"
				;
*/		}

		void ProfileAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			try
			{
				Request::ParametersMap::const_iterator it = map.find(AdminRequest::PARAMETER_OBJECT_ID);
				if (it != map.end())
					_profile = ProfileTableSync::get(Conversion::ToLongLong(it->second));
			}
			catch (DBEmptyResultException& e)
			{
				// throw AdminElementInitializationException("Bad user");
			}
		}

		ProfileAdmin::~ProfileAdmin()
		{
			delete _profile;
		}
	}
}
