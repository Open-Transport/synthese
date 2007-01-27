
/** DisplayAdmin class implementation.
	@file DisplayAdmin.cpp

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

#include "32_admin/AdminRequest.h"

#include "34_departures_table/DisplayAdmin.h"

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace util;

	namespace departurestable
	{
		DisplayAdmin::DisplayAdmin()
			: AdminInterfaceElement("displays", AdminInterfaceElement::DISPLAYED_IF_CURRENT) {}


		std::string DisplayAdmin::getTitle() const
		{
			return "Afficheur "; // + nom afficheur
		}

		void DisplayAdmin::display(std::ostream& stream, const server::Request* request /*= NULL*/ ) const
		{
			// Update bike request
//			AdminRequest* updateBikeRequest = Factory<Request>::create<AdminRequest>();
//			updateBikeRequest->copy(request);
//			updateBikeRequest->setPage(Factory<AdminInterfaceElement>::create<VinciBikeAdminInterfaceElement>());

			stream
				<< "<h1>Emplacement</h1>"
				<< "<table><tr><td>Lieu logique</td>"
				<< "<td><select name=\"\">"; // Script on update reload the page

			// Logical stops list

			stream
				<< "</select></td></tr>"
				<< "<tr><td>Lieu physique</td>"
				<< "<td><select name=\"\">";

			// Broadcast points list

			stream
				<< "</select></td></tr>"
				<< "<tr><td>Complément de précision</td>"
				<< "<td><input type=\"text\" name=\"\" value=\"\" /></td></tr>" // Input with value
				<< "</table>"

				<< "<h1>Données techniques</h1>"
				<< "<table>"
				<< "<tr><td>Type d'afficheur</td>"
				<< "<td><select name=\"\">";

			// Display types list

			stream
				<< "</select></td></tr>"
				<< "<tr><td>Code de branchement</td>"
				<< "<td><select name=\"\">";

			for (int i=0; i<20; ++i)
				stream << "<option value=\"" << i << "\">" << i << "</option>"; // Add selected for display

			stream
				<< "</select></td></tr>"
				<< "<tr><td>UID</td><td><input type=\"text\" name=\"\" value=\"\" /></td></tr>"
				<< "</table>"

				<< "<h1>Apparence</h1>"
				<< "<table><tr><td>Titre</td><td><input type=\"text\" name=\"\" value=\"\" /></td></tr>"
				<< "<tr><td>Clignotement</td><td><select name=\"\">"
				<< "<option value=\"0\">Pas de clignotement</option>"
				<< "<option value=\"1\">1 minute avant disparition</option>";

			for (i=2; i<6; ++i)
				stream << "<option value=\"" << i << "\">" << i << " minutes avant disparition</option>";

			stream
				<< "</select></td></tr>"
				<< "<tr><td>Affichage numéro de quai</td><td><input type=\"radio\" name=\"RadioGroupd\">OUI <INPUT type=\"radio\" CHECKED value=\"Radio1\" name=\"RadioGroupd\" />NON</td></tr>"
				<< "<tr><td>Affichage numéro de service</td><td><INPUT type=\"radio\" value=\"Radio1\" name=\"RadioGroup\">OUI <INPUT type=\"radio\" CHECKED value=\"Radio1\" name=\"RadioGroup\">NON</td></tr>"
				<< "</table>"

				<< "<h1>Contenu</h1>"
				<< "<table>"
				;
/*											</TR>
											<TR>
												<TD style="WIDTH: 240px; HEIGHT: 23px"><FONT size="1"><FONT size="1"><FONT color="#000000" size="1">Horaires</FONT></FONT></FONT></TD>
												<TD style="HEIGHT: 23px"><SELECT id="Select7" name="Select1">
														<OPTION value="" selected>Départ</OPTION>
													</SELECT></TD>
											</TR>
											<TR>
												<TD style="WIDTH: 240px"><FONT size="1"><FONT size="1"><FONT color="#000000" size="1">Sélection 
																terminus</FONT></FONT></FONT></TD>
												<TD><SELECT id="Select6" name="Origines seulement">
														<OPTION value="" selected>Origines seulement</OPTION>
														<OPTION value="">Origines et passages</OPTION>
													</SELECT></TD>
											</TR>
											<TR>
												<TD style="WIDTH: 240px"><FONT size="1"><FONT size="1"><FONT color="#000000" size="1">Délai 
																maximum d'affichage</FONT></FONT></FONT></TD>
												<TD><INPUT id="Text3" style="WIDTH: 32px; HEIGHT: 22px" type="text" size="1" value="300" name="Text1"><FONT color="#000000" size="2">minutes</FONT></TD>
											</TR>
											<TR>
												<TD style="WIDTH: 240px"><FONT size="1"><FONT size="1"><FONT color="#000000" size="1">Délai 
																d'effacement</FONT></FONT></FONT></TD>
												<TD><SELECT id="Select5" name="Origines seulement">
														<OPTION value="">5 minutes avant départ</OPTION>
														<OPTION value="">4 minutes avant départ</OPTION>
														<OPTION value="">3 minutes avant départ</OPTION>
														<OPTION value="">2 minutes avant départ</OPTION>
														<OPTION value="" selected>1 minute avant départ</OPTION>
														<OPTION value="">Heure du départ</OPTION>
														<OPTION value="">1 minute après départ</OPTION>
														<OPTION value="">2 minutes après départ</OPTION>
														<OPTION value="">3 minutes après départ</OPTION>
														<OPTION value="">4 minutes après départ</OPTION>
														<OPTION value="">5 minutes après départ</OPTION>
													</SELECT></TD>
											</TR>
											<TR>
												<TD style="WIDTH: 240px; HEIGHT: 65px" vAlign="middle"><FONT size="1"><FONT size="1"><FONT color="#000000" size="1">Sélection 
																sur arrêt physique</FONT></FONT></FONT></TD>
												<TD style="HEIGHT: 65px">
													<P>
														<TABLE language="javascript" id="Table3" style="WIDTH: 268px; HEIGHT: 80px" onclick="return Table3_onclick()"
															cellSpacing="0" cellPadding="0" width="268" border="0">
															<TR>
																<TD style="WIDTH: 14px"><INPUT id="Checkbox1" type="checkbox" CHECKED name="Checkbox1"></TD>
																<TD><FONT size="2">Marengo quai 1</FONT></TD>
															</TR>
															<TR>
																<TD style="WIDTH: 14px"><INPUT id="Checkbox2" type="checkbox" name="Checkbox1"></TD>
																<TD><FONT size="2">Marengo quai 2</FONT></TD>
															</TR>
															<TR>
																<TD style="WIDTH: 14px"><INPUT id="Checkbox3" type="checkbox" name="Checkbox1"></TD>
																<TD><FONT size="2">Matabiau face gare (ligne 22)</FONT></TD>
															</TR>
															<TR>
																<TD style="WIDTH: 14px"><INPUT id="Checkbox4" type="checkbox" name="Checkbox1"></TD>
																<TD><FONT size="2">Tous (y compris nouveaux)</FONT></TD>
															</TR>
														</TABLE>
													</P>
												</TD>
											</TR>
											<TR>
												<TD style="WIDTH: 240px; HEIGHT: 61px" vAlign="middle"><FONT size="1"><FONT size="1"><FONT color="#000000" size="1">Sélection 
																sur arrêt logique à ne pas desservir</FONT></FONT></FONT></TD>
												<TD style="HEIGHT: 61px">
													<TABLE language="javascript" id="Table4" style="WIDTH: 264px; HEIGHT: 72px" onclick="return Table3_onclick()"
														cellSpacing="0" cellPadding="0" width="264" border="0">
														<TR>
															<TD style="WIDTH: 179px"><FONT size="2">TOULOUSE Izards</FONT></TD>
															<TD><INPUT language="javascript" id="Button1" onclick="alert('Etes vous sur de vouloir supprimer l\'arrêt sélectionné ?')"
																	type="button" value="Supprimer" name="Button1"></TD>
														</TR>
														<TR>
															<TD style="WIDTH: 179px"><FONT size="2">TOULOUSE Chamois</FONT></TD>
															<TD><INPUT language="javascript" id="Button2" onclick="alert('Etes vous sur de vouloir supprimer l\'arrêt sélectionné ?')"
																	type="button" value="Supprimer" name="Button1"></TD>
														</TR>
														<TR>
															<TD style="WIDTH: 179px"><FONT size="2"><SELECT id="Select9" name="Origines seulement">
																		<OPTION value="" selected>TOULOUSE Amat Massot</OPTION>
																		<OPTION value="">TOULOUSE LEP Bayard</OPTION>
																	</SELECT></FONT></TD>
															<TD><INPUT language="javascript" id="Button3" onclick="return Button1_onclick()" type="button"
																	value="Ajouter" name="Button1"></TD>
														</TR>
													</TABLE>
												</TD>
											</TR>
											<TR>
												<TD style="WIDTH: 240px" bgColor="gainsboro" colSpan="2"><FONT size="2"><STRONG>Présélection</STRONG></FONT></TD>
											</TR>
											<TR>
												<TD style="WIDTH: 240px"><FONT size="1"><FONT size="1"><FONT color="#000000" size="1">Activer</FONT></FONT></FONT></TD>
												<TD><FONT size="2"><INPUT id="Radio5" type="radio" value="Radio1" name="RadioGroupp">OUI
														<INPUT id="Radio6" type="radio" CHECKED value="Radio1" name="RadioGroupp">NON</FONT></TD>
											</TR>
											<TR>
												<TD style="WIDTH: 240px"><FONT size="1"><FONT size="1"><FONT color="#000000" size="1">Délai 
																maximum présélection</FONT></FONT></FONT></TD>
												<TD><INPUT id="Text5" style="WIDTH: 32px; HEIGHT: 22px" type="text" size="1" value="120" name="Text1"><FONT color="#000000" size="2">minutes</FONT></TD>
											</TR>
											<TR>
												<TD style="WIDTH: 240px" vAlign="middle"><FONT size="1"><FONT size="1"><FONT color="#000000" size="1">Arrêts 
																de desserte intermédiaire supplémentaires (les terminus des lignes sont 
																automatiquement présélectionnés)</FONT></FONT></FONT></TD>
												<TD>
													<TABLE language="javascript" id="Table5" style="WIDTH: 256px; HEIGHT: 48px" onclick="return Table3_onclick()"
														cellSpacing="0" cellPadding="0" width="256" border="0">
														<TR>
															<TD><FONT size="2">TOULOUSE Colombette</FONT></TD>
															<TD><INPUT language="javascript" id="Button4" onclick="alert('Etes vous sur de vouloir supprimer l\'arrêt sélectionné ?')"
																	type="button" value="Supprimer" name="Button1"></TD>
														</TR>
														<TR>
															<TD><FONT size="2"><SELECT id="Select10" name="Origines seulement">
																		<OPTION value="" selected>TOULOUSE Guilhemery</OPTION>
																		<OPTION value="">TOULOUSE LEP Bayard</OPTION>
																	</SELECT></FONT></TD>
															<TD><INPUT language="javascript" id="Button6" onclick="return Button1_onclick()" type="button"
																	value="Ajouter" name="Button1"></TD>
														</TR>
													</TABLE>
												</TD>
											</TR>
											<TR>
												<TD align="center" colSpan="2"><INPUT language="javascript" id="Button7" style="WIDTH: 191px; HEIGHT: 24px" onclick="location='http://www.connex.tisseo.fr/itineraires?fonction=tdg&amp;date=A&amp;tb=463427346823642'"
														type="button" value="Simuler les paramètres" name="Button1"><INPUT language="javascript" id="Button5" style="WIDTH: 191px; HEIGHT: 24px" type="button"
														value="Enregistrer les modifications" name="Button1"></TD>
											</TR>
										</TABLE>
									</P>
									<P>
								</FONT></FONT></FONT></FONT><FONT face="Verdana" size="2">NB : Certains 
						types d'afficheurs ne prennent pas en charge toutes les fonctionnalités 
						proposées. Selon le type de l'afficheur, certains champs peuvent donc être sans 
						effet sur l'affichage.</FONT></P></TD>
			</TR>
		</TABLE>
	</BODY>
</HTML>

*/
			// Cleaning
//			delete updateBikeRequest;
		}

		void DisplayAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			Request::ParametersMap::const_iterator it = map.find(Request::PARAMETER_OBJECT_ID);
		//	if (it != map.end())
		//		_displayScreen = VinciBikeTableSync::get(Conversion::ToLongLong(it->second));
		}
	}
}
