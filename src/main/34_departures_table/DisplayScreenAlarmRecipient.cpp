
/** DisplayScreenAlarmRecipient class implementation.
	@file DisplayScreenAlarmRecipient.cpp

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

#include <vector>

#include "01_util/Html.h"

#include "15_env/ConnectionPlace.h"

#include "17_messages/Alarm.h"
#include "17_messages/AlarmObjectLink.h"
#include "17_messages/AlarmObjectLinkTableSync.h"

#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/DisplayScreenAlarmRecipient.h"

using namespace std;

namespace synthese
{
	using namespace messages;
	using namespace util;

	namespace departurestable
	{


		DisplayScreenAlarmRecipient::DisplayScreenAlarmRecipient()
			: AlarmRecipientTemplate<DisplayScreen>("Afficheurs")
		{

		}

		void DisplayScreenAlarmRecipient::displayBroadcastListEditor( std::ostream& stream, const messages::Alarm* alarm, const server::Request* request )
		{
			stream << "<table><tr><th>Emplacement</th><th>Etat</th><th>Actions</th></tr>";

			vector<AlarmObjectLink<DisplayScreen> >& dsv = AlarmObjectLinkTableSync::search<DisplayScreen> (alarm, this->getFactoryKey());
			for (vector<AlarmObjectLink<DisplayScreen> >::iterator dsit = dsv.begin(); dsit != dsv.end(); ++dsit)
			{
				DisplayScreen* ds = dsit->getObject();

				stream << "<tr><td>" << ds->getLocalization()->getConnectionPlace()->getFullName() << "/" << ds->getLocalization()->getName();

				if (ds->getLocalizationComment() != "")
					stream << "/" << ds->getLocalizationComment();

				stream
					<< "</td><td><FONT face=\"Wingdings\" color=\"#00cc00\">l</FONT></td>"
					<< "<td>" << Html::getSubmitButton("Supprimer") << "</td></tr>";

				delete ds->getLocalization();
				delete ds;
			}

			stream
				<< "</table>";

/*				<TR>
				<TD style=\"WIDTH: 162px; HEIGHT: 371px\"><FONT size=\"1\"><FONT size=\"1\"><FONT color=\"#000000\" size=\"1\">Ajout 
				d'afficheur</FONT></FONT></FONT></TD>
				<TD style=\"HEIGHT: 371px\"><FONT face=\"Verdana\"><FONT size=\"2\"><FONT face=\"Verdana\"><FONT size=\"2\">
				<P>UID <INPUT id=\"Text5\" style=\"WIDTH: 80px; HEIGHT: 22px\" type=\"text\" size=\"8\" name=\"Text2\">,&nbsp;Emplacement
				<SELECT id=\"Select6\" name=\"Select1\">
				<OPTION value=\"\" selected>TOULOUSE Matabiau</OPTION>
				</SELECT>, Ligne&nbsp;<SELECT id=\"Select7\" name=\"Select4\">
				<OPTION value=\"\" selected>(toutes)</OPTION>
				<OPTION value=\"\">MP/TLS/41</OPTION>
				</SELECT>
				,Type
				<SELECT language=\"javascript\" id=\"Select8\" onclick=\"return Select2_onclick()\" name=\"Select2\">
				<OPTION value=\"\" selected>(tous)</OPTION>
				<OPTION value=\"\">Oscar</OPTION>
				<OPTION value=\"\">TFT 21\"</OPTION>
				</SELECT>, Etat
				<SELECT id=\"Select9\" name=\"Select3\">
				<OPTION value=\"\" selected>(tous)</OPTION>
				<OPTION value=\"\">OK</OPTION>
				<OPTION value=\"\">Warning</OPTION>
				<OPTION value=\"\">Warning+Error</OPTION>
				<OPTION value=\"\">Error</OPTION>
				</SELECT>,&nbsp;Message
				<SELECT id=\"Select10\" name=\"Message\">
				<OPTION value=\"\" selected>(tous)</OPTION>
				<OPTION value=\"\">Un message</OPTION>
				<OPTION value=\"\">Conflit</OPTION>
				<OPTION value=\"\">Messages</OPTION>
				</SELECT>&nbsp;<INPUT id=\"Button12\" type=\"button\" value=\"Rechercher\" name=\"Button6\"></P>
				<P>
				Résultats de la recherche (tous) :</FONT></FONT>
				</P>
				<P>
				<TABLE language=\"javascript\" id=\"Table4\" style=\"WIDTH: 432px; HEIGHT: 240px\" cellSpacing=\"0\"
				cellPadding=\"1\" width=\"432\" border=\"1\">
				<TBODY>
				<TR>
				<TD style=\"WIDTH: 19px; HEIGHT: 23px\"><STRONG><FONT size=\"2\">Sel</FONT></STRONG></TD>
				<TD style=\"WIDTH: 120px; HEIGHT: 23px\"><STRONG><FONT size=\"2\"><STRONG><FONT size=\"2\"><FONT color=\"#0000ff\"><U>UID
				</U></FONT><FONT color=\"#000000\">(<FONT face=\"Wingdings 3\">r</FONT>)</FONT></FONT></STRONG></FONT></STRONG></TD>
				<TD style=\"WIDTH: 186px; HEIGHT: 23px\" align=\"center\"><STRONG><FONT color=\"#0000ff\" size=\"2\"><U><STRONG><FONT color=\"#0000ff\" size=\"2\"><U><STRONG><FONT color=\"#0000ff\" size=\"2\"><U>Emplacement</U></FONT></STRONG></U></FONT></STRONG></U></FONT></STRONG></TD>
				<TD style=\"WIDTH: 54px; HEIGHT: 23px\" align=\"center\"><STRONG><FONT color=\"#0000ff\" size=\"2\"><U><STRONG><FONT color=\"#0000ff\" size=\"2\"><U><STRONG><FONT color=\"#0000ff\" size=\"2\"><U>Type</U></FONT></STRONG></U></FONT></STRONG></U></FONT></STRONG></TD>
				<TD style=\"WIDTH: 43px; HEIGHT: 23px\" align=\"center\"><STRONG><FONT color=\"#0000ff\" size=\"2\"><U><STRONG><FONT color=\"#0000ff\" size=\"2\"><U><STRONG><FONT color=\"#0000ff\" size=\"2\"><U>Etat</U></FONT></STRONG></U></FONT></STRONG></U></FONT></STRONG></TD>
				<TD style=\"HEIGHT: 23px\" align=\"center\"><STRONG><FONT color=\"#0000ff\" size=\"2\"><U><STRONG><FONT color=\"#0000ff\" size=\"2\"><U><STRONG><FONT color=\"#0000ff\" size=\"2\"><U>Msg</U></FONT></STRONG></U></FONT></STRONG></U></FONT></STRONG></TD>
				</TR>
				<TR>
				<TD style=\"WIDTH: 19px\"><FONT size=\"1\"><INPUT id=\"Checkbox1\" type=\"checkbox\" name=\"Checkbox3\"></FONT></TD>
				<TD style=\"WIDTH: 120px\"><FONT color=\"#0000ff\" size=\"1\"><U><A href=\"admin_display.htm\">5896215762324576</A></U></FONT></TD>
				<TD style=\"WIDTH: 186px\"><FONT color=\"#0000ff\" size=\"1\"><U><A href=\"admin_display.htm\">TOULOUSE 
				Matabiau/Marengo quai 1</A></U></FONT></TD>
				<TD style=\"WIDTH: 54px\"><FONT color=\"#0000ff\" size=\"1\"><U><A href=\"admin_display.htm\">Oscar</A></U></FONT></TD>
				<TD style=\"WIDTH: 43px\"><FONT face=\"Wingdings\" color=\"#00cc00\">l<FONT face=\"Verdana\"> </FONT>
				<FONT face=\"Wingdings\" color=\"#00cc00\">l</FONT></FONT></TD>
				<TD align=\"center\"><FONT face=\"Wingdings\" color=\"#00cc00\">l</FONT></TD>
				</TR>
				<TR>
				<TD style=\"WIDTH: 19px\"><FONT size=\"1\"><INPUT id=\"Checkbox2\" type=\"checkbox\" name=\"Checkbox3\"></FONT></TD>
				<TD style=\"WIDTH: 120px\"><FONT color=\"#0000ff\" size=\"1\"><U><FONT color=\"#0000ff\" size=\"1\"><U><A href=\"admin_display.htm\">6174821345698741</A></U></FONT></U></FONT></TD>
				<TD style=\"WIDTH: 186px\"><FONT color=\"#0000ff\" size=\"1\"><U><A href=\"admin_display.htm\">TOULOUSE 
				Matabiau/Salle d'attente Marengo/Dessus porte</A></U></FONT></TD>
				<TD style=\"WIDTH: 54px\"><FONT color=\"#0000ff\" size=\"1\"><U><A href=\"admin_display.htm\">TFT 21\"</A></U></FONT></TD>
				<TD style=\"WIDTH: 43px\"><FONT face=\"Wingdings\" color=\"#00cc00\"><FONT color=\"#ff9900\">l</FONT><FONT face=\"Verdana\">
				</FONT><FONT style=\"BACKGROUND-COLOR: #ffffff\" face=\"Wingdings\" color=\"#ff0033\">l</FONT></FONT></TD>
				<TD align=\"center\"><FONT face=\"Wingdings\" color=\"#00cc00\"><FONT color=\"#ff9900\">l</FONT></FONT></TD>
				</TR>
				<TR>
				<TD style=\"WIDTH: 19px\"><FONT size=\"1\"><INPUT id=\"Checkbox3\" type=\"checkbox\" name=\"Checkbox3\"></FONT></TD>
				<TD style=\"WIDTH: 120px\"><FONT color=\"#0000ff\" size=\"1\"><U><FONT color=\"#0000ff\" size=\"1\"><U><FONT color=\"#0000ff\" size=\"1\"><U><FONT color=\"#0000ff\" size=\"1\"><U><FONT color=\"#0000ff\" size=\"1\"><U><A href=\"admin_display.htm\">6548746213210254</A></U></FONT></U></FONT></U></FONT></U></FONT></U></FONT></TD>
				<TD style=\"WIDTH: 186px\"><FONT color=\"#0000ff\" size=\"1\"><U><A href=\"admin_display.htm\">TOULOUSE 
				Matabiau/Salle d'attente Marengo/Fond de salle</A></U></FONT></TD>
				<TD style=\"WIDTH: 54px\"><FONT color=\"#0000ff\" size=\"1\"><U><A href=\"admin_display.htm\">TFT 21\"</A></U></FONT></TD>
				<TD style=\"WIDTH: 43px\" align=\"center\" colSpan=\"1\" rowSpan=\"1\"><FONT size=\"2\"><FONT color=\"#ff0000\">HS
				<FONT size=\"1\">10/9</FONT></FONT></FONT></TD>
				<TD align=\"center\"><FONT style=\"BACKGROUND-COLOR: #ffffff\" face=\"Wingdings\" color=\"#ff0033\">l</FONT></TD>
				</TR>
				<TR>
				<TD style=\"WIDTH: 19px\">&nbsp;</TD>
				<TD style=\"WIDTH: 315px\" colSpan=\"5\"><INPUT language=\"javascript\" id=\"Button19\" style=\"WIDTH: 328px; HEIGHT: 24px\" onclick=\"location='admin_display.htm';\"
				type=\"button\" value=\"Ajouter les afficheurs sélectionnés\" name=\"btn12\">
				</TD>
				<P></P>
				</FONT></FONT>
				</TD>
				</TR>
				</TABLE>
				</P></TD></TR>
				<TR>
				<TD align=\"center\" colSpan=\"2\"><INPUT language=\"javascript\" id=\"Button5\" style=\"WIDTH: 191px; HEIGHT: 24px\" onclick=\"return Button1_onclick()\"
				type=\"button\" value=\"OK\" name=\"Button1\"></TD>
				</TR>
*/

		}
	}
}
