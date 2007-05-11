
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

#include "01_util/Conversion.h"

#include "05_html/HTMLForm.h"
#include "05_html/HTMLTable.h"

#include "15_env/EnvModule.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/City.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminParametersException.h"

#include "34_departures_table/AdvancedSelectTableSync.h"
#include "34_departures_table/BroadcastPointAdmin.h"
#include "34_departures_table/BroadcastPointsAdmin.h"
#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/CreateBroadcastPointAction.h"
#include "34_departures_table/RenameBroadcastPointAction.h"
#include "34_departures_table/DeleteBroadcastPointAction.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace env;
	using namespace html;
	using namespace departurestable;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement,BroadcastPointAdmin>::FACTORY_KEY("broadcastpoint");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<BroadcastPointAdmin>::ICON("building.png");
		template<> const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<BroadcastPointAdmin>::DISPLAY_MODE(AdminInterfaceElement::DISPLAYED_IF_CURRENT);
		template<> string AdminInterfaceElementTemplate<BroadcastPointAdmin>::getSuperior()
		{
			return BroadcastPointsAdmin::FACTORY_KEY;
		}
	}

	namespace departurestable
	{
		void BroadcastPointAdmin::setFromParametersMap(const ParametersMap& map)
		{
			// Place ID
			ParametersMap::const_iterator it = map.find(Request::PARAMETER_OBJECT_ID);
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

		void BroadcastPointAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			ActionFunctionRequest<CreateBroadcastPointAction,AdminRequest> createRequest(request);
			createRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<BroadcastPointAdmin>());
			createRequest.setObjectId(request->getObjectId());

			ActionFunctionRequest<DeleteBroadcastPointAction,AdminRequest> deleteRequest(request);
			deleteRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<BroadcastPointAdmin>());
			deleteRequest.setObjectId(request->getObjectId());

			ActionFunctionRequest<RenameBroadcastPointAction,AdminRequest> renameRequest(request);
			renameRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<BroadcastPointAdmin>());
			renameRequest.setObjectId(request->getObjectId());

			stream << "<h1>Emplacements d'affichage de la zone d'arrêt</h1>";

			HTMLTable t;
			stream << t.open();

			// Physical stop loop
			vector<PhysicalStopAndBroadcastPoint> m = getConnectionPlacePhysicalStopsAndBroadcastPoints(_place->getKey());
			for (vector<PhysicalStopAndBroadcastPoint>::iterator it = m.begin(); it != m.end(); ++it)
			{

				stream << t.row();
				stream << t.col() << "Arrêt physique " << it->stop->getName();
				stream << t.col();
				if (it->bp == NULL)
				{
					HTMLForm f(createRequest.getHTMLForm("create" + Conversion::ToString(it->stop->getKey())));
					f.addHiddenField(CreateBroadcastPointAction::PARAMETER_PHYSICAL_ID, Conversion::ToString(it->stop->getKey()));
					f.addHiddenField(CreateBroadcastPointAction::PARAMETER_PLACE_ID, Conversion::ToString(_place->getKey()));
					stream << f.getLinkButton("Activer");
				}
				else
				{
					HTMLForm f(deleteRequest.getHTMLForm("delete" + Conversion::ToString(it->bp->getKey())));
					f.addHiddenField(DeleteBroadcastPointAction::PARAMETER_BROADCAST_ID, Conversion::ToString(it->bp->getKey()));
					stream
						<< "Arrêt physique actif en tant que point de diffusion "
						<< f.getLinkButton("Désactiver", "Etes-vous sûr(e) de vouloir supprimer le point de diffusion ?");
				}
			}

			vector<PhysicalStopAndBroadcastPoint> b = getConnectionPlaceBroadcastPointsAndPhysicalStops(_place->getKey(), false);
			for (vector<PhysicalStopAndBroadcastPoint>::iterator bit = b.begin(); bit != b.end(); ++bit)
			{
				stream << t.row();
				
				HTMLForm rf(renameRequest.getHTMLForm("rename" + Conversion::ToString(bit->bp->getKey())));
				rf.addHiddenField(RenameBroadcastPointAction::PARAMETER_BROADCAST_ID, Conversion::ToString(bit->bp->getKey()));

				stream << t.col() << rf.open();
				stream << rf.getTextInput(RenameBroadcastPointAction::PARAMETER_NAME, bit->bp->getName());
				stream << rf.getSubmitButton("Renommer");
				stream << rf.close();

				HTMLForm df(deleteRequest.getHTMLForm("delete" + Conversion::ToString(bit->bp->getKey())));
				df.addHiddenField(DeleteBroadcastPointAction::PARAMETER_BROADCAST_ID, Conversion::ToString(bit->bp->getKey()));

				stream << t.col() << df.getLinkButton("Supprimer","Etes-vous sûr(e) de vouloir supprimer le point de diffusion ?");
			}

			HTMLForm cf(createRequest.getHTMLForm("createbp"));
			cf.addHiddenField(CreateBroadcastPointAction::PARAMETER_PLACE_ID, Conversion::ToString(_place->getKey()));
			stream << t.row();
			stream << t.col(2) << cf.open();
			stream << cf.getTextInput(CreateBroadcastPointAction::PARAMETER_NAME, "", "(entrez le nom ici)");
			stream << cf.getSubmitButton("Ajouter");
			stream << cf.close();

			stream << t.close();
		}

		bool BroadcastPointAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return true;
		}

		BroadcastPointAdmin::BroadcastPointAdmin()
			: AdminInterfaceElementTemplate<BroadcastPointAdmin>()
		{
		
		}
	}
}
