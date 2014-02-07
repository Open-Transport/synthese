
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESEUpdatePushService class implementation.
///	@file InterSYNTHESEUpdatePushService.cpp
///	@author hromain
///	@date 2013
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "InterSYNTHESEUpdatePushService.hpp"

#include "Import.hpp"
#include "InterSYNTHESEFileFormat.hpp"
#include "InterSYNTHESEPacket.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace impex;
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,inter_synthese::InterSYNTHESEUpdatePushService>::FACTORY_KEY = "inter_synthese_push_update";
	
	namespace inter_synthese
	{
		const string InterSYNTHESEUpdatePushService::PARAMETER_IMPORT_ID = "import_id";
		const string InterSYNTHESEUpdatePushService::PARAMETER_SLAVE_ID = "slave_id";
		const string InterSYNTHESEUpdatePushService::PARAMETER_CONTENT = "content";
		const string InterSYNTHESEUpdatePushService::RANGE_SEPARATOR = ",";
		


		ParametersMap InterSYNTHESEUpdatePushService::_getParametersMap() const
		{
			ParametersMap map;

			if(_importId)
			{
				map.insert(PARAMETER_IMPORT_ID, _importId);
			}

			if(_slaveId)
			{
				map.insert(PARAMETER_SLAVE_ID, _slaveId);
			}

			if(!_content.empty())
			{
				map.insert(PARAMETER_CONTENT, _content);
			}

			return map;
		}



		void InterSYNTHESEUpdatePushService::_setFromParametersMap(const ParametersMap& map)
		{
			_importId = map.getDefault<RegistryKeyType>(PARAMETER_IMPORT_ID, 0);

			// Check of the import
			try
			{
				boost::shared_ptr<const Import> import(
					Env::GetOfficialEnv().get<Import>(_importId)
				);
				Env env;
				string noLog;
				ParametersMap pm;
				boost::shared_ptr<Importer> importer(
					import->getImporter(
						env,
						IMPORT_LOG_NOLOG,
						noLog,
						optional<ostream&>(),
						pm
				)	);

				InterSYNTHESEFileFormat::Importer_* interSYNTHESEImporter(
					dynamic_cast<InterSYNTHESEFileFormat::Importer_*>(importer.get())
				);
				if(!interSYNTHESEImporter)
				{
					throw RequestException("No such Inter-SYNTHESE import");
				}

				_slaveId = map.getDefault<RegistryKeyType>(PARAMETER_SLAVE_ID, 0);
				if( _slaveId !=	interSYNTHESEImporter->getSlaveId()
				){
					throw RequestException("Bad slave id");
				}
			}
			catch(ObjectNotFoundException<Import>&)
			{
				throw RequestException("No such Inter-SYNTHESE import");
			}

			_content = map.get<string>(PARAMETER_CONTENT);
		}



		ParametersMap InterSYNTHESEUpdatePushService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			
			try
			{
				InterSYNTHESEPacket packet(_content);

				// Load of the data contained in the packet
				packet.load(NULL);

				// Output the loaded range to clean the queue in the master
				stream << packet.getIdRange().first << RANGE_SEPARATOR << packet.getIdRange().second;
			}
			catch(InterSYNTHESEPacket::BadPacketException&)
			{
				throw RequestException("Malformed packet");
			}

			return map;
		}
		
		
		
		bool InterSYNTHESEUpdatePushService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string InterSYNTHESEUpdatePushService::getOutputMimeType() const
		{
			return "text/plain";
		}



		InterSYNTHESEUpdatePushService::InterSYNTHESEUpdatePushService():
			_importId(0),
			_slaveId(0)
		{}
}	}