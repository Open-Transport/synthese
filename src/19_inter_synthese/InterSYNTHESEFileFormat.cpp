
/** InterSYNTHESEFileFormat class implementation.
	@file InterSYNTHESEFileFormat.cpp

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

#include "InterSYNTHESEFileFormat.hpp"

#include "BasicClient.h"
#include "Import.hpp"
#include "InterSYNTHESEIdFilter.hpp"
#include "InterSYNTHESEPacket.hpp"
#include "InterSYNTHESESlaveUpdateService.hpp"
#include "InterSYNTHESEUpdateAckService.hpp"
#include "StaticFunctionRequest.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace inter_synthese;
	using namespace server;
	using namespace util;

	template<>
	const string FactorableTemplate<FileFormat, InterSYNTHESEFileFormat>::FACTORY_KEY = "inter_synthese";

	namespace inter_synthese
	{
		const string InterSYNTHESEFileFormat::Importer_::PARAMETER_SLAVE_ID = "slave_id";
		const string InterSYNTHESEFileFormat::Importer_::PARAMETER_ID_FILTER = "id_filter";



		bool InterSYNTHESEFileFormat::Importer_::_read(
		) const	{
			try
			{
				_logDebug(
					"Inter-SYNTHESE : Attempt to sync with "+ _address +":"+ _port + " as slave id #"+ lexical_cast<string>(_slaveId)
				);
				
				StaticFunctionRequest<InterSYNTHESESlaveUpdateService> r;
				r.getFunction()->setSlaveId(_slaveId);
				BasicClient c(
					_address,
					_port
				);
				string contentStr(
					c.get(r.getURL())
				);

				// Case no content to sync
				if(contentStr == InterSYNTHESESlaveUpdateService::NO_CONTENT_TO_SYNC)
				{
					_logDebug(
						"Inter-SYNTHESE : "+ _address +":"+ _port + " has no content to sync for slave id #"+ lexical_cast<string>(_slaveId)
					);
					return true;
				}

				// Case content to sync
				InterSYNTHESEPacket packet(contentStr);

				_logDebug(
					"Inter-SYNTHESE : "+ _address +":"+ _port + " has sent "+ lexical_cast<string>(packet.size()) +" elements to sync in "+ lexical_cast<string>(contentStr.size()) +" bytes for slave id #"+ lexical_cast<string>(_slaveId)
				);

				StaticFunctionRequest<InterSYNTHESEUpdateAckService> ackRequest;
				ackRequest.getFunction()->setSlaveId(_slaveId);
				if(!packet.empty())
				{
					ackRequest.getFunction()->setRangeBegin(
						packet.getIdRange().first
					);
					ackRequest.getFunction()->setRangeEnd(
						packet.getIdRange().second
					);
				}
				BasicClient c2(
					_address,
					_port
				);
				string result2(
					c2.get(ackRequest.getURL())
				);
				if(result2 == InterSYNTHESEUpdateAckService::VALUE_OK)
				{
					packet.load(_idFilter.get());
					_logDebug(
						"Inter-SYNTHESE : "+ _address +":"+ _port + " has been synchronized with current instance as slave id #"+ lexical_cast<string>(_slaveId)
					);
				}
			}
			catch(InterSYNTHESEPacket::BadPacketException&)
			{
				_logError(
					"Inter-SYNTHESE : Synchronization with "+ _address +":"+ _port + " as slave id #"+ lexical_cast<string>(_slaveId) +" has failed : malformed packet"
				);
				return false;
			}
			catch(std::exception& e)
			{
				_logError(
					"Inter-SYNTHESE : Synchronization with "+ _address +":"+ _port + " as slave id #"+ lexical_cast<string>(_slaveId) +" has failed "+ string(e.what())
				);
				return false;
			}
			return true;
		}



		void InterSYNTHESEFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			// Slave ID
			_slaveId = map.get<RegistryKeyType>(PARAMETER_SLAVE_ID);

			// ID Filter
			string idFilter(map.getDefault<string>(PARAMETER_ID_FILTER));
			trim(idFilter);
			if(!idFilter.empty())
			{
				_idFilter.reset(Factory<InterSYNTHESEIdFilter>::create(idFilter));
				if(_import.get<DataSource>())
				{
					_idFilter->setDataSource(&*_import.get<DataSource>());
				}
			}
		}



		util::ParametersMap InterSYNTHESEFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap pm;
			pm.insert(PARAMETER_SLAVE_ID, _slaveId);
			if(_idFilter.get())
			{
				pm.insert(PARAMETER_ID_FILTER, _idFilter->getFactoryKey());
			}
			return pm;
		}



		db::DBTransaction InterSYNTHESEFileFormat::Importer_::_save() const
		{
			return DBTransaction();
		}



		InterSYNTHESEFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			ConnectionImporter<InterSYNTHESEFileFormat>(env, import, minLogLevel, logPath, outputStream, pm)
		{}
}	}

