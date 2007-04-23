
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

#include "34_departures_table/DisplayScreenAlarmRecipient.h"

#include <vector>

#include "05_html/ResultHTMLTable.h"

#include "11_interfaces/InterfaceModule.h"

#include "15_env/ConnectionPlace.h"
#include "15_env/Line.h"
#include "15_env/EnvModule.h"

#include "17_messages/Alarm.h"
#include "17_messages/AlarmObjectLink.h"
#include "17_messages/AlarmObjectLinkTableSync.h"
#include "17_messages/AlarmRemoveLinkAction.h"
#include "17_messages/AlarmAddLinkAction.h"

#include "32_admin/AdminModule.h"

#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/DeparturesTableModule.h"
#include "34_departures_table/DisplaySearchAdmin.h"
#include "34_departures_table/DisplayScreenTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{


	using namespace messages;
	using namespace util;
	using namespace departurestable;
	using namespace server;
	using namespace env;
	using namespace admin;
	using namespace interfaces;
	using namespace html;

	namespace messages
	{
	    /* IMPORTANT : for some reason, probably a gcc bug, is was necessary to 
	       explicitly call constructor with () in order to avoid undefined references.
	       This should be investigate further.
	    */

	    template<> AlarmRecipientTemplate<DisplayScreen>::AlarmLinks
	    AlarmRecipientTemplate<DisplayScreen>::_linksAlarm = 
		std::map<const SentAlarm*, std::set<const DisplayScreen*> > ();

	    template<> AlarmRecipientTemplate<DisplayScreen>::ObjectLinks
	      AlarmRecipientTemplate<DisplayScreen>::_linksObject = 
		std::map<const DisplayScreen*, std::set<const SentAlarm*> > () ;

	}


	namespace departurestable
	{
		const std::string DisplayScreenAlarmRecipient::PARAMETER_SEARCH_UID = "dsarsu";
		const std::string DisplayScreenAlarmRecipient::PARAMETER_SEARCH_PLACE = "dsarsp";
		const std::string DisplayScreenAlarmRecipient::PARAMETER_SEARCH_LINE = "dsarsl";
		const std::string DisplayScreenAlarmRecipient::PARAMETER_SEARCH_TYPE = "dsarst";
		const std::string DisplayScreenAlarmRecipient::PARAMETER_SEARCH_STATUS = "dsarss";
		const std::string DisplayScreenAlarmRecipient::PARAMETER_SEARCH_MESSAGE = "dsarsm";

		DisplayScreenAlarmRecipient::DisplayScreenAlarmRecipient()
			: AlarmRecipientTemplate<DisplayScreen>("Afficheurs")
		{

		}

		void DisplayScreenAlarmRecipient::displayBroadcastListEditor(
			std::ostream& stream
			, const messages::Alarm* alarm
			, const server::ParametersMap& parameters
			, server::FunctionRequest<admin::AdminRequest>& searchRequest
			, server::ActionFunctionRequest<messages::AlarmAddLinkAction,admin::AdminRequest>& addRequest
			, server::ActionFunctionRequest<messages::AlarmRemoveLinkAction, admin::AdminRequest>& removeRequest
		){
			vector<shared_ptr<DisplayScreen> > dsv = AlarmObjectLinkTableSync::search<DisplayScreen> (alarm, this->getFactoryKey());
			set<uid> usedDisplayScreens;

			if (!dsv.empty())
			{
				vector<string> v;
				v.push_back("Emplacement");
				v.push_back("Etat");
				v.push_back("Actions");
				HTMLTable t(v);

				stream << t.open();

				for (vector<shared_ptr<DisplayScreen> >::iterator dsit = dsv.begin(); dsit != dsv.end(); ++dsit)
				{
					shared_ptr<DisplayScreen> ds = *dsit;
					usedDisplayScreens.insert(ds->getKey());
					removeRequest.getAction()->setObjectId(ds->getKey());
					
					stream << t.row();
					stream << t.col() << ds->getLocalization()->getConnectionPlace()->getFullName() << "/" << ds->getLocalization()->getName();
					if (ds->getLocalizationComment() != "")
						stream << "/" << ds->getLocalizationComment();

					stream << t.col() << "<FONT face=\"Wingdings\" color=\"#00cc00\">l</FONT>"; // Bullet
					stream << t.col() << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir retirer l\\'afficheur des destinataires du message ?");
				}

				stream << t.close();
			}

			stream << "<p>Ajout d'afficheur</p>";

			ParametersMap::const_iterator it;
			it = parameters.find(PARAMETER_SEARCH_UID);
			uid searchUid = (it == parameters.end()) ? 0 : Conversion::ToLongLong(it->second);
			it = parameters.find(PARAMETER_SEARCH_PLACE);
			uid searchPlace = (it == parameters.end()) ? 0 : Conversion::ToLongLong(it->second);
			uid searchLine = 0;
			uid searchType = 0;
			int searchState = 0;
			int searchMessage = 0;

			stream << DisplaySearchAdmin::getHtmlSearchForm(searchRequest.getHTMLForm(), searchUid, searchPlace, searchLine, searchType, searchState, searchMessage);


			ResultHTMLTable::HeaderVector v1;
			v1.push_back(make_pair(PARAMETER_SEARCH_PLACE, "Emplacement"));
			v1.push_back(make_pair(PARAMETER_SEARCH_TYPE, "Type"));
			v1.push_back(make_pair(PARAMETER_SEARCH_STATUS, "Etat"));
			v1.push_back(make_pair(PARAMETER_SEARCH_MESSAGE, "Msg"));
			v1.push_back(make_pair(string(), "Add"));
			ResultHTMLTable t1(v1,searchRequest.getHTMLForm(), ResultHTMLTable::RequestParameters(), ResultHTMLTable::ResultParameters());

			stream << "<p>Résultats de la recherche :</p>";

			stream << t1.open();

			vector<boost::shared_ptr<DisplayScreen> > result2 = DisplayScreenTableSync::search(searchUid, searchPlace, searchLine, searchType, searchState, searchMessage);

			for (vector<shared_ptr<DisplayScreen> >::const_iterator it = result2.begin(); it != result2.end(); ++it)
			{
				shared_ptr<const DisplayScreen> screen = *it;
				if (screen->getLocalization() == NULL)
					continue;
				if (usedDisplayScreens.find(screen->getKey()) != usedDisplayScreens.end())
					continue;

				addRequest.getAction()->setObjectId(screen->getKey());

				stream << t1.row(Conversion::ToString(screen->getKey()));
				stream << t1.col() << screen->getFullName();
				stream << t1.col() << (screen->getType() ? screen->getType()->getName() : "inconnu");
				stream << t1.col();
				stream << t1.col();
				stream << t1.col() << HTMLModule::getLinkButton(addRequest.getURL(), "Ajouter");
			}

			stream << t1.close();
		}

		AlarmRecipientSearchFieldsMap DisplayScreenAlarmRecipient::getSearchFields(HTMLForm& form, const ParametersMap& parameters) const
		{

			ParametersMap::const_iterator it;

			shared_ptr<const ConnectionPlace> place;
			it = parameters.find(PARAMETER_SEARCH_PLACE);
			if (it != parameters.end() && EnvModule::getConnectionPlaces().contains(Conversion::ToLongLong(it->second)))
				place = EnvModule::getConnectionPlaces().get(Conversion::ToLongLong(it->second));

			shared_ptr<const Line> line;
			it = parameters.find(PARAMETER_SEARCH_LINE);
			if (it != parameters.end() && EnvModule::getLines().contains(Conversion::ToLongLong(it->second)))
				line  = EnvModule::getLines().get(Conversion::ToLongLong(it->second));

			AlarmRecipientSearchFieldsMap map;
			AlarmRecipientFilter arf;

			arf.label = "Arrêt";
			arf.htmlField = form.getSelectInput(PARAMETER_SEARCH_PLACE, DeparturesTableModule::getPlacesWithBroadcastPointsLabels(true), place ? place->getKey() : UNKNOWN_VALUE);
			arf.query = "";
			map.insert(make_pair(PARAMETER_SEARCH_PLACE, arf));

			arf.label = "Ligne";
			arf.htmlField = form.getSelectInput(PARAMETER_SEARCH_LINE, DeparturesTableModule::getCommercialLineWithBroadcastLabels(true), line ? line->getKey() : UNKNOWN_VALUE);
			arf.query = "";
			map.insert(make_pair(PARAMETER_SEARCH_LINE, arf));

			return map;
		}

		void DisplayScreenAlarmRecipient::addObject(const SentAlarm* alarm, uid objectId )
		{
			add(DeparturesTableModule::getDisplayScreens().get(objectId).get(), alarm);
		}

		void DisplayScreenAlarmRecipient::removeObject(const SentAlarm* alarm, uid objectId )
		{
			remove(DeparturesTableModule::getDisplayScreens().get(objectId).get(), alarm);
		}
	}
}
