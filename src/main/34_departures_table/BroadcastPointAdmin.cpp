
/** BroadcastPointAdmin class implementation.
	@file BroadcastPointAdmin.cpp

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
#include "01_util/Conversion.h"

#include "15_env/EnvModule.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/City.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"

#include "34_departures_table/AdvancedSelectTableSync.h"
#include "34_departures_table/BroadcastPointAdmin.h"
#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/CreateBroadcastPointAction.h"
#include "34_departures_table/RenameBroadcastPointAction.h"
#include "34_departures_table/DeleteBroadcastPointAction.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace env;

	namespace departurestable
	{
		BroadcastPointAdmin::BroadcastPointAdmin()
			: AdminInterfaceElement("broadcastpoints", AdminInterfaceElement::DISPLAYED_IF_CURRENT)
			, _place(NULL)
		{}


		void BroadcastPointAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			// Place ID
			Request::ParametersMap::const_iterator it = map.find(Request::PARAMETER_OBJECT_ID);
			uid placeId = Conversion::ToLongLong(it->second);
			if (it == map.end())
				throw AdminParametersException("Connection place not specified");

			if (!EnvModule::getConnectionPlaces().contains(placeId))
				throw AdminParametersException("Connection place not found");

			_place = EnvModule::getConnectionPlaces().get(placeId);
		}

		string BroadcastPointAdmin::getTitle() const
		{
			return _place->getCity()->getName() + " " + _place->getName();
		}

		void BroadcastPointAdmin::display(ostream& stream, const Request* request) const
		{
			AdminRequest* createRequest = Factory<Request>::create<AdminRequest>();
			createRequest->copy(request);
			createRequest->setPage(Factory<AdminInterfaceElement>::create<BroadcastPointAdmin>());
			createRequest->setAction(Factory<Action>::create<CreateBroadcastPointAction>());

			AdminRequest* deleteRequest = Factory<Request>::create<AdminRequest>();
			deleteRequest->copy(request);
			deleteRequest->setPage(Factory<AdminInterfaceElement>::create<BroadcastPointAdmin>());
			deleteRequest->setAction(Factory<Action>::create<DeleteBroadcastPointAction>());

			AdminRequest* renameRequest = Factory<Request>::create<AdminRequest>();
			renameRequest->copy(request);
			renameRequest->setPage(Factory<AdminInterfaceElement>::create<BroadcastPointAdmin>());
			renameRequest->setAction(Factory<Action>::create<RenameBroadcastPointAction>());

			stream
				<< "<h1>Emplacements d'affichage de la zone d'arrêt</h1>"
				<< "<table>";

			// Physical stop loop
			vector<PhysicalStopAndBroadcastPoint> m = getConnectionPlacePhysicalStopsAndBroadcastPoints(_place->getKey());
			for (vector<PhysicalStopAndBroadcastPoint>::iterator it = m.begin(); it != m.end(); ++it)
			{

				stream
					<< "<tr><td>Arrêt physique " << it->stop->getName() << "</td>"
					<< "<td>";
				if (it->bp == NULL)
				{
					stream
						<< createRequest->getHTMLFormHeader("create" + Conversion::ToString(it->stop->getKey()))
						<< Html::getHiddenInput(CreateBroadcastPointAction::PARAMETER_PHYSICAL_ID, Conversion::ToString(it->stop->getKey()))
						<< Html::getHiddenInput(CreateBroadcastPointAction::PARAMETER_PLACE_ID, Conversion::ToString(_place->getKey()))
						<< Html::getSubmitButton("Activer")
						<< "</form>";
				}
				else
				{
					stream
						<< deleteRequest->getHTMLFormHeader("delete" + Conversion::ToString(it->bp->getKey()))
						<< Html::getHiddenInput(DeleteBroadcastPointAction::PARAMETER_BROADCAST_ID, Conversion::ToString(it->bp->getKey()))
						<< "Arrêt physique actif en tant que point de diffusion" 
						<< Html::getSubmitButton("Supprimer")
						<< "</form>";
				}
				stream	
					<< "</td>"
					<< "</tr>";
				delete it->bp;
			}

			vector<PhysicalStopAndBroadcastPoint> b = getConnectionPlaceBroadcastPointsAndPhysicalStops(_place->getKey(), false);
			for (vector<PhysicalStopAndBroadcastPoint>::iterator bit = b.begin(); bit != b.end(); ++bit)
			{
				stream
					<< "<tr><td>" << renameRequest->getHTMLFormHeader("rename" + Conversion::ToString(bit->bp->getKey()))
					<< Html::getHiddenInput(RenameBroadcastPointAction::PARAMETER_BROADCAST_ID, Conversion::ToString(bit->bp->getKey()))
					<< Html::getTextInput(RenameBroadcastPointAction::PARAMETER_NAME, bit->bp->getName())
					<< Html::getSubmitButton("Renommer")
					<< "</form></td>"
					<< "<td>" << deleteRequest->getHTMLFormHeader("delete" + Conversion::ToString(bit->bp->getKey()))
					<< Html::getHiddenInput(DeleteBroadcastPointAction::PARAMETER_BROADCAST_ID, Conversion::ToString(bit->bp->getKey()))
					<< Html::getSubmitButton("Supprimer")
					<< "</form></td>"
					<< "</tr>";
				delete bit->bp;
			}

			stream
				<< createRequest->getHTMLFormHeader("createbp")
				<< Html::getHiddenInput(CreateBroadcastPointAction::PARAMETER_PLACE_ID, Conversion::ToString(_place->getKey()))
				<< "<tr><td>" << Html::getTextInput(CreateBroadcastPointAction::PARAMETER_NAME, "", "(entrez le nom ici)") << "</td>"
				<< "<td>" << Html::getSubmitButton("Ajouter") << "</td>"
				<< "</tr></form>";

			delete createRequest;
			delete deleteRequest;
			delete renameRequest;
		}
	}
}
