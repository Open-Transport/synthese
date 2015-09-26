
/** LineAlarmRecipient class implementation.
	@file LineAlarmRecipient.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "LineAlarmRecipient.hpp"

#include "ResultHTMLTable.h"
#include "SecurityConstants.hpp"
#include "TreeFolder.hpp"
#include "User.h"
#include "Profile.h"
#include "HTMLList.h"
#include "StopArea.hpp"
#include "JourneyPattern.hpp"
#include "JourneyPatternTableSync.hpp"
#include "PTModule.h"
#include "Alarm.h"
#include "AlarmObjectLink.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmRemoveLinkAction.h"
#include "AlarmAddLinkAction.h"
#include "AlarmObjectLinkException.h"
#include "AdminModule.h"
#include "TransportNetworkAdmin.h"
#include "TransportNetwork.h"
#include "ImportableTableSync.hpp"
#include "TransportNetworkRight.h"

#include <boost/foreach.hpp>
#include <vector>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace messages;
	using namespace tree;
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace admin;
	using namespace html;
	using namespace security;
	using namespace impex;

	namespace util
	{
		template<> const std::string FactorableTemplate<AlarmRecipient, LineAlarmRecipient>::FACTORY_KEY("line");
	}

	namespace messages
	{
		template<>
		const string AlarmRecipientTemplate<LineAlarmRecipient>::TITLE("Lignes");

		template<>
		void AlarmRecipientTemplate<LineAlarmRecipient>::GetParametersLabels(
			ParameterLabelsVector& m
		){
			m.push_back(make_pair(FACTORY_KEY +"/"+ GLOBAL_PERIMETER,"(toutes les lignes)"));
			PTModule::getNetworkLinePlaceRightParameterList(m, FACTORY_KEY +"/", "Ligne ");
		}



		template<>
		util::RegistryKeyType AlarmRecipientTemplate<LineAlarmRecipient>::GetObjectIdBySource(
			const impex::DataSource& source,
			const std::string& key,
			util::Env& env
		){
			ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(source, env);
			ImportableTableSync::ObjectBySource<CommercialLineTableSync>::Set linesSet(lines.get(key));
			if(linesSet.empty())
			{
				throw Exception("No such line");
			}
			return (*linesSet.begin())->getKey();
		}

	}


	namespace pt
	{
		const string LineAlarmRecipient::PARAMETER_SEARCH_LINE("sl");

		void LineAlarmRecipient::displayBroadcastListEditor(
			std::ostream& stream,
			const messages::Alarm* alarm,
			const util::ParametersMap& parameters,
			Request& searchRequest,
			StaticActionRequest<messages::AlarmAddLinkAction>& addRequest,
			StaticActionRequest<messages::AlarmRemoveLinkAction>& removeRequest
		){
			Env env;
			CommercialLineTableSync::SearchResult dsv(
				AlarmObjectLinkTableSync::search<CommercialLineTableSync, CommercialLine>(
					env,
					alarm->getKey(),
					this->getFactoryKey()
			)	);
			std::sort(dsv.begin(), dsv.end(), CommercialLine::PointerComparator());

			set<util::RegistryKeyType> usedLines;

			stream << "<h1>Lignes destinataires</h1>";

			if (dsv.empty())
			{
				stream << "<p>Aucune ligne destinataire</p>";
			}
			else
			{
				HTMLList l;
				stream << l.open();

				// Loop on lines
				for (CommercialLineTableSync::SearchResult::iterator dsit = dsv.begin(); dsit != dsv.end(); ++dsit)
				{
					boost::shared_ptr<CommercialLine> ds = *dsit;

					// Right check
//					if(!getRight()->perimeterIncludes(lexical_cast<string>(ds->getKey()))
//					{
//						continue;
//					}

					usedLines.insert(ds->getKey());
					removeRequest.getAction()->setObjectId(ds->getKey());

					stream << l.element("chart_line");
					stream << HTMLModule::getHTMLLink(removeRequest.getURL(), HTMLModule::getHTMLImage("/admin/img/delete.png","Supprimer"));

					stream << ds->getShortName();
				}

				stream << l.close();
			}

			stream << "<h1>Ajout de ligne</h1>";

			string searchName(parameters.getDefault<string>(TransportNetworkAdmin::PARAMETER_SEARCH_NAME));
			optional<RegistryKeyType> searchNetworkId(parameters.getOptional<RegistryKeyType>(TransportNetworkAdmin::PARAMETER_SEARCH_NETWORK_ID));

			TransportNetworkAdmin::getHTMLLineSearchForm(
				stream,
				searchRequest.getHTMLForm(),
				searchNetworkId,
				searchName
			);

			CommercialLineTableSync::SearchResult lines(
				CommercialLineTableSync::Search(
					Env::GetOfficialEnv(),
					searchNetworkId,
					string("%"+searchName+"%")
			)	);
			std::sort(lines.begin(), lines.end(), CommercialLine::PointerComparator());

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(string(), "N°"));
			h.push_back(make_pair(TransportNetworkAdmin::PARAMETER_SEARCH_NETWORK_ID, "Réseau"));
			h.push_back(make_pair(TransportNetworkAdmin::PARAMETER_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t(
				h,
				searchRequest.getHTMLForm(),
				ResultHTMLTable::RequestParameters(),
				lines
			);

			stream << t.open();
			BOOST_FOREACH(const boost::shared_ptr<CommercialLine>& line, lines)
			{
				if (usedLines.find(line->getKey()) != usedLines.end())
				{
					continue;
				}
				addRequest.getAction()->setObjectId(line->getKey());

				stream << t.row();
				stream << t.col(1, line->getStyle(), true);
				stream << line->getNetwork()->getName();
				stream << "/" << line->getShortName();
				stream << t.col();
				stream << line->getName();
				stream << t.col();
				stream << t.col() << HTMLModule::getLinkButton(addRequest.getURL(), "Ajouter");
			}

			stream << t.close();
		}



		AlarmRecipientSearchFieldsMap LineAlarmRecipient::getSearchFields(HTMLForm& form, const ParametersMap& parameters) const
		{
			AlarmRecipientSearchFieldsMap map;
/*			boost::shared_ptr<const JourneyPattern> line;
			Env env;
			optional<RegistryKeyType> id(parameters.getOptional<RegistryKeyType>(PARAMETER_SEARCH_LINE));
			if (id)
			{
				line  = JourneyPatternTableSync::Get(*id, env, FIELDS_ONLY_LOAD_LEVEL);
			}

			AlarmRecipientFilter arf;

			arf.label = "Ligne";
			arf.htmlField = form.getSelectInput(
				PARAMETER_SEARCH_LINE,
				DeparturesTableModule::getCommercialLineWithBroadcastLabels(true),
				line ? line->getKey() : optional<RegistryKeyType>()
			);
			arf.query = "";
			map.insert(make_pair(PARAMETER_SEARCH_LINE, arf));
*/
			return map;
		}



		boost::shared_ptr<security::Right> LineAlarmRecipient::getRight( const std::string& perimeter ) const
		{
			TransportNetworkRight* result(new TransportNetworkRight);
			result->setParameter(perimeter);
			return boost::shared_ptr<Right>(result);
		}
}	}
