
/** PTOperationFileFormat class implementation.
	@file PTOperationFileFormat.cpp

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

#include "PTOperationFileFormat.hpp"

#include "ImportLogger.hpp"
#include "VehicleServiceTableSync.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace impex;

	namespace pt_operation
	{
		VehicleService* PTOperationFileFormat::CreateOrUpdateVehicleService(
			impex::ImportableTableSync::ObjectBySource<VehicleServiceTableSync>& vehicleServices,
			const std::string& id,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& logger
		){
			set<VehicleService*> loadedVehicleServices(vehicleServices.get(id));
			if(!loadedVehicleServices.empty())
			{
				stringstream logStream;
				logStream << "Link between vehicle services " << id << " and ";
				BOOST_FOREACH(VehicleService* vs, loadedVehicleServices)
				{
					logStream << vs->getKey();
				}
				logger.log(ImportLogger::LOAD, logStream.str());
			}
			else
			{
				boost::shared_ptr<VehicleService> vs(new VehicleService(VehicleServiceTableSync::getId()));

				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				vs->setDataSourceLinksWithoutRegistration(links);
				env.getEditableRegistry<VehicleService>().add(vs);
				vehicleServices.add(*vs);
				loadedVehicleServices.insert(vs.get());

				logger.log(
					ImportLogger::CREA,
					"Creation of the vehicle service with key "+ id
				);
			}
			return *loadedVehicleServices.begin();
		}
}	}
