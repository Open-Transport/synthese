
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESEObjectUpdateService class implementation.
///	@file InterSYNTHESEObjectUpdateService.cpp
///	@author Thomas Puigt
///	@date 2014
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

#include "InterSYNTHESEObjectUpdateService.hpp"

#include "InterSYNTHESEQueue.hpp"
#include "InterSYNTHESESyncTypeFactory.hpp"
#include "DBDirectTableSync.hpp"
#include "DBTransaction.hpp"
#include "ObjectBase.hpp"
#include "Request.h"

#include <boost/lexical_cast.hpp>


using namespace boost;
using namespace std;

namespace synthese
{
    using namespace db;
    using namespace util;
    using namespace server;

    template<>
    const string FactorableTemplate<server::Function,inter_synthese::InterSYNTHESEObjectUpdateService>::FACTORY_KEY = "inter_synthese_object_update";

    namespace inter_synthese
    {

        const string InterSYNTHESEObjectUpdateService::PARAMETER_OBJECT_ID = "object_id";
        const string InterSYNTHESEObjectUpdateService::PARAMETER_FIELD_PREFIX = "field_";

        const string InterSYNTHESEObjectUpdateService::QUEUE_IDS_SEPARATOR = ",";
        const string InterSYNTHESEObjectUpdateService::NO_ITEM_IN_QUEUE = "item_updated_not_found_in_queues";

        const string InterSYNTHESEObjectUpdateService::FORMAT_JSON("json");

        const string InterSYNTHESEObjectUpdateService::PARAMETER_OLDQUEUEIDS = "oldqueueids";

        const string InterSYNTHESEObjectUpdateService::ATTR_QUEUEIDS = "queue_ids";
        const string InterSYNTHESEObjectUpdateService::TAG_INTERSYNTHESE_KEYS = "intersynthese_keys";



        ParametersMap InterSYNTHESEObjectUpdateService::_getParametersMap() const
        {
            ParametersMap map(_values);
            if(_object)
            {
                map.insert(PARAMETER_OBJECT_ID, _object->getKey());
            }

            if(!_oldQueueIds.empty())
            {
                map.insert(PARAMETER_OLDQUEUEIDS, _oldQueueIds);
            }
            // Output format
            if(!_outputFormat.empty())
            {
                map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
            }
            return map;
        }



        void InterSYNTHESEObjectUpdateService::_setFromParametersMap(const ParametersMap& map)
        {
            // Object id
            RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_OBJECT_ID));
            try
            {
                RegistryTableType tableId(decodeTableId(id));
                _tableSync = dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
                    DBModule::GetTableSync(tableId)
                );
                if(!_tableSync.get())
                {
                    throw Exception("Incompatible table");
                }
                _object = dynamic_cast<ObjectBase*>(
                    _tableSync->getEditableRegistrable(id, *_env).get()
                );
                if(!_object)
                {
                    throw Exception("Incompatible table");
                }
                _setObject(*_object);
            }
            catch(ObjectNotFoundException<void*>&)
            {
                throw Exception("No such object");
            }

            // Record extraction
            ParametersMap record;
            BOOST_FOREACH(const ParametersMap::Map::value_type& item, map.getMap())
            {
                if(	item.first.size() <= PARAMETER_FIELD_PREFIX.size() ||
                    item.first.substr(0, PARAMETER_FIELD_PREFIX.size()) != PARAMETER_FIELD_PREFIX
                ){
                    continue;
                }
                record.insert(
                    item.first.substr(PARAMETER_FIELD_PREFIX.size()),
                    map.getValue(item.first)
                );
            }
            ParametersMap existingMap;
            _object->toParametersMap(existingMap, false);
            record.merge(existingMap);

            // Record read
            LinkedObjectsIds linkedObjects(_value->getLinkedObjectsIds(record));
            BOOST_FOREACH(RegistryKeyType linkedId, linkedObjects)
            {
                DBModule::GetObject(linkedId, *_env);
            }
            _value->loadFromRecord(record, *_env);

            // Value check
            try
            {
                _value->checkIntegrity();
            }
            catch(ObjectBase::IntegrityException& e)
            {
                throw Exception(e.getMessage());
            }

            _oldQueueIds = map.getDefault<string>(PARAMETER_OLDQUEUEIDS);

            // Output format
            _outputFormat = map.getDefault<string>(PARAMETER_OUTPUT_FORMAT);
        }



        ParametersMap InterSYNTHESEObjectUpdateService::run(
            std::ostream& stream,
            const Request& request
        ) const {
            ParametersMap map;

            DBTransaction transaction;
            _value->beforeUpdate(*_object, transaction);
            _tableSync->saveRegistrable(*_value, transaction);
            _value->afterUpdate(*_object, transaction);
            transaction.run();

            // Using the id of the created object, find the matching queue id
            // of this new entry in the queue of each slave
            string content;

            QueueIds queueIds;
            BOOST_FOREACH(
                        const InterSYNTHESEQueue::Registry::value_type& it,
                        Env::GetOfficialEnv().getRegistry<InterSYNTHESEQueue>()
            ){
                // Creating stringstream containing 'rstmt:table_id', id of the table of the new object
                stringstream tableIdStream;
                tableIdStream << "rstmt:";
                RegistryTableType tableId(decodeTableId(_value->getKey()));
                tableIdStream << tableId;
                // Filling the content string with the content field of the item in the queue
                content = it.second->get<SyncContent>();
                // Getting the id of the new object in its table
                stringstream keystream;
                keystream << _value->getKey();
                // Look for the string 'rstmt:table_id' in the content of the queue item
                // and Look for the id of the new object in the content of the queue item
                if((string::npos != content.find(tableIdStream.str()))
                        && (string::npos != content.find(keystream.str())))
                {
                    if(string::npos == _oldQueueIds.find(lexical_cast<string>(it.second->getKey())))
                    {
                        // New object has been found in the queue, let's store its queue id
                        queueIds.insert(it.second->get<Key>());
                    }
                }
            }

            // If the new object has not been found in the queue
            if(queueIds.empty())
            {
                stream << NO_ITEM_IN_QUEUE;
            }
            else
            {
                // Serialize the _queueIds set and insert it in the map
                stringstream queueIdsStr;
                unsigned int i = 1;
                BOOST_FOREACH(
                            const QueueIds::value_type& qId,
                            queueIds
                            ){
                    if(i == queueIds.size())
                    {
                        queueIdsStr << qId;
                    }
                    else
                    {
                        queueIdsStr << qId << QUEUE_IDS_SEPARATOR;
                    }
                    i++;
                }
                map.insert(ATTR_QUEUEIDS, queueIdsStr.str());
            }
            if (_outputFormat == FORMAT_JSON)
            {
                map.outputJSON(stream, TAG_INTERSYNTHESE_KEYS);
            }
            return map;
        }



        bool InterSYNTHESEObjectUpdateService::isAuthorized(
            const Session* session
        ) const {
            return true;
        }



        InterSYNTHESEObjectUpdateService::InterSYNTHESEObjectUpdateService(){}



        void InterSYNTHESEObjectUpdateService::_setObject( const ObjectBase& value )
        {
            // Object to update
            _object = &value;

            // Table sync
            RegistryTableType tableId(decodeTableId(_object->getKey()));
            _tableSync = dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
                DBModule::GetTableSync(tableId)
            );
            if(!_tableSync.get())
            {
                throw Exception("Incompatible table");
            }

            // Copied value
            _value = _object->copy();
        }



        std::string InterSYNTHESEObjectUpdateService::getOutputMimeType() const
        {
            std::string mimeType;
            if(_outputFormat == FORMAT_JSON)
            {
                mimeType = "application/json";
            }
            else // For empty result
            {
                mimeType = "text/html";
            }
            return mimeType;
        }



}	}
