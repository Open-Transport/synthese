
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
#include "InterSYNTHESESlaveUpdateService.hpp"
#include "InterSYNTHESESyncTypeFactory.hpp"
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

				if(contentStr == InterSYNTHESESlaveUpdateService::NO_CONTENT_TO_SYNC)
				{
					_logDebug(
						"Inter-SYNTHESE : "+ _address +":"+ _port + " has no content to sync for slave id #"+ lexical_cast<string>(_slaveId)
					);
					return true;
				}

				bool ok(true);
				typedef std::map<
					util::RegistryKeyType,	// id of the update
					std::pair<
						std::string,		// synchronizer
						std::string			// message
				>	> ContentMap;
				ContentMap content;
	
				size_t i(0);
				while(i < contentStr.size())
				{
					ContentMap::mapped_type item;

					// ID + Search for next :
					size_t l=i;
					for(; i < contentStr.size() && contentStr[i] != InterSYNTHESESlaveUpdateService::FIELDS_SEPARATOR[0]; ++i) ;
					if(i == contentStr.size())
					{
						ok = false;
						break;
					}
					RegistryKeyType id(lexical_cast<RegistryKeyType>(contentStr.substr(l, i-l)));
					++i;

					// Synchronizer + Search for next :
					l=i;
					for(; i < contentStr.size() && contentStr[i] != InterSYNTHESESlaveUpdateService::FIELDS_SEPARATOR[0]; ++i) ;
					if(i == contentStr.size())
					{
						ok = false;
						break;
					}
					item.first = contentStr.substr(l, i-l);
					++i;

					// Size + Search for next :
					l=i;
					for(; i < contentStr.size() && contentStr[i] != InterSYNTHESESlaveUpdateService::FIELDS_SEPARATOR[0]; ++i) ;
					if(i == contentStr.size())
					{
						ok = false;
						break;
					}
					size_t contentSize = lexical_cast<size_t>(contentStr.substr(l, i-l));
					++i;

					// Content
					if(i+contentSize > contentStr.size())
					{
						ok = false;
						break;
					}
					item.second = contentStr.substr(i, contentSize);
					i += contentSize + InterSYNTHESESlaveUpdateService::SYNCS_SEPARATOR.size();

					content.insert(
						make_pair(
							id,
							item
					)	);
				}

				if(!ok)
				{
					return false;
				}
				_logDebug(
					"Inter-SYNTHESE : "+ _address +":"+ _port + " has sent "+ lexical_cast<string>(content.size()) +" elements to sync in "+ lexical_cast<string>(contentStr.size()) +" bytes for slave id #"+ lexical_cast<string>(_slaveId)
				);

				StaticFunctionRequest<InterSYNTHESEUpdateAckService> ackRequest;
				ackRequest.getFunction()->setSlaveId(_slaveId);
				if(!content.empty())
				{
					ackRequest.getFunction()->setRangeBegin(
						content.begin()->first
					);
					ackRequest.getFunction()->setRangeEnd(
						content.rbegin()->first
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
					// Local variables
					auto_ptr<InterSYNTHESESyncTypeFactory> interSYNTHESE;
					string lastFactoryKey;

					// Reading the content
					BOOST_FOREACH(const ContentMap::value_type& item, content)
					{
						try
						{
							const string& factoryKey(item.second.first);
							if(factoryKey != lastFactoryKey)
							{
								if(interSYNTHESE.get())
								{
									interSYNTHESE->closeSync();
								}
								interSYNTHESE.reset(
									Factory<InterSYNTHESESyncTypeFactory>::create(factoryKey)
								);
								lastFactoryKey = factoryKey;
								interSYNTHESE->initSync();
							}

							interSYNTHESE->sync(
								item.second.second,
								_idFilter.get()
							);
						}
						catch(...)
						{
							// Log
						}
					}
					if(interSYNTHESE.get())
					{
						interSYNTHESE->closeSync();
						_logDebug(
							"Inter-SYNTHESE : "+ _address +":"+ _port + " has been synchronized with current instance as slave id #"+ lexical_cast<string>(_slaveId)
						);
					}
				}
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

