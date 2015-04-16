
/** TreeFolderDownNodeInterface class implementation.
	@file TreeFolderDownNodeInterface.cpp

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

#include "TreeFolderDownNodeInterface.hpp"

#include "DBDirectTableSync.hpp"
#include "DBModule.h"
#include "TreeFolder.hpp"
#include "TreeFolderRoot.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace tree;
	using namespace util;


	FIELD_DEFINITION_OF_TYPE(Parent, "parent_id", SQL_INTEGER)

	namespace tree
	{
		void TreeFolderDownNodeInterface::GetLinkedObjectsIds(
			LinkedObjectsIds& list,
			const Record& record
		){
			RegistryKeyType parent_id(record.getDefault<RegistryKeyType>(FIELD.name, 0));
			if(	parent_id > 0 &&
				decodeTableId(parent_id) > 0
			){
				list.push_back(parent_id);
			}
		}



		bool TreeFolderDownNodeInterface::LoadFromRecord(
			Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const Env& env
		){
			assert(dynamic_cast<TreeFolderDownNodeInterface*>(&object));
			TreeFolderDownNodeInterface& node(dynamic_cast<TreeFolderDownNodeInterface&>(object));
			bool result(false);
			
			if(record.isDefined(FIELD.name))
			{
				// Name
				try
				{
					RegistryKeyType id(record.get<RegistryKeyType>(FIELD.name));
					RegistryTableType tableId(decodeTableId(id));

					if(tableId)
					{
						if(tableId != node.getTableId())
						{
							node.setTableId(tableId);
							result = true;
						}

						boost::shared_ptr<DBTableSync> ts(DBModule::GetTableSync(node.getTableId()));
						if(!dynamic_cast<DBDirectTableSync*>(ts.get()))
						{
							throw ObjectNotFoundException<TreeFolderUpNode>(id, "forbidden class");
						}

						const RegistryBase& registry(dynamic_cast<DBDirectTableSync&>(*ts).getRegistry(env));
						boost::shared_ptr<Registrable> parent(registry.getEditableObject(id));

						if(!dynamic_cast<TreeFolderUpNode*>(parent.get()))
						{
							throw ObjectNotFoundException<TreeFolderUpNode>(id, "forbidden class");
						}

						TreeFolderUpNode* parentValue(dynamic_cast<TreeFolderUpNode*>(parent.get()));
						if(parentValue != node._getParent())
						{
							node._setParent(*parentValue);
							result = true;
						}
						fieldObject = parentValue;
					}
					else
					{
						if(node.getTableId() != id)
						{
							node.setTableId(id);
							result = true;
						}
					}
				}
				catch(ObjectNotFoundException<Registrable>&)
				{
					Log::GetInstance().warn(
						"Data corrupted on object parent"
					);
				}
				catch(ObjectNotFoundException<TreeFolderUpNode>&)
				{
					Log::GetInstance().warn(
						"Data corrupted on object parent"
					);
				}
			}

			return result;
		}



		void TreeFolderDownNodeInterface::SaveToParametersMap(
			const Type& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			if(	boost::logic::indeterminate(withFiles) ||
				SimpleObjectFieldDefinition<Parent>::FIELD.exportOnFile == withFiles
			){
				assert(dynamic_cast<const TreeFolderDownNodeInterface*>(&object));
				const TreeFolderDownNodeInterface& node(dynamic_cast<const TreeFolderDownNodeInterface&>(object));

				if(node._getParent() && dynamic_cast<Registrable*>(node._getParent()))
				{
					map.insert(prefix + FIELD.name, dynamic_cast<Registrable*>(node._getParent())->getKey());
				}
				else if(fieldObject && dynamic_cast<const Registrable*>(fieldObject))
				{
					map.insert(prefix + FIELD.name, dynamic_cast<const Registrable*>(fieldObject)->getKey());
				}
				else
				{
					map.insert(prefix + FIELD.name, static_cast<util::RegistryKeyType>(node.getTableId()));
				}
			}
		}



		void TreeFolderDownNodeInterface::SaveToParametersMap(
			const Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			if(	boost::logic::indeterminate(withFiles) ||
				SimpleObjectFieldDefinition<Parent>::FIELD.exportOnFile == withFiles
			){
				map.insert(prefix + FIELD.name, fieldObject ? fieldObject->getKey() : RegistryKeyType(0));
			}
		}



		void TreeFolderDownNodeInterface::SaveToDBContent(
			const Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			RegistryKeyType i(0);
			assert(dynamic_cast<const TreeFolderDownNodeInterface*>(&object));
			const TreeFolderDownNodeInterface& node(dynamic_cast<const TreeFolderDownNodeInterface&>(object));

			if(node._getParent() && dynamic_cast<Registrable*>(node._getParent()))
			{
				i = dynamic_cast<Registrable*>(node._getParent())->getKey();
			}
			else if(fieldObject && dynamic_cast<const Registrable*>(fieldObject))
			{
				i = dynamic_cast<const Registrable*>(fieldObject)->getKey();
			}
			else
			{
				i = node.getTableId();
			}
			content.push_back(Cell(i));
		}



		void TreeFolderDownNodeInterface::SaveToFilesMap(
			const Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			if(SimpleObjectFieldDefinition<Parent>::FIELD.exportOnFile)
			{
				assert(dynamic_cast<const TreeFolderDownNodeInterface*>(&object));
				const TreeFolderDownNodeInterface& node(dynamic_cast<const TreeFolderDownNodeInterface&>(object));

				FilesMap::File item;
				item.mimeType = util::MimeTypes::TEXT;
				if(node._getParent() && dynamic_cast<Registrable*>(node._getParent()))
				{
					item.content = lexical_cast<string>(dynamic_cast<Registrable*>(node._getParent())->getKey());
				}
				else if(fieldObject && dynamic_cast<const Registrable*>(fieldObject))
				{
					item.content = lexical_cast<string>(dynamic_cast<const Registrable*>(fieldObject)->getKey());
				}
				else
				{
					item.content = node.getTableId();
				}
				map.insert(
					SimpleObjectFieldDefinition<Parent>::FIELD.name,
					item
				);
			}
		}



		TreeFolder* TreeFolderDownNodeInterface::getParentFolder() const
		{
			if(dynamic_cast<TreeFolder*>(_parent))
			{
				return dynamic_cast<TreeFolder*>(_parent);
			}
			return NULL;
		}



		void TreeFolderDownNodeInterface::_setParent(TreeFolderUpNode& value)
		{
			removeParentLink();
			_parent = &value;
			_parent->addChild(*this);
			if(dynamic_cast<TreeFolderDownNodeInterface*>(_parent))
			{
				_root = dynamic_cast<TreeFolderDownNodeInterface*>(_parent)->_getRoot();
			}
			else if(dynamic_cast<TreeFolderRoot*>(_parent))
			{
				_root = dynamic_cast<TreeFolderRoot*>(_parent);
			}
			else
			{
				assert(false);
			}
		}



		void TreeFolderDownNodeInterface::setNullParent()
		{
			removeParentLink();
			_parent = NULL;
			_root = NULL;
		}



		bool TreeFolderDownNodeInterface::isChildOf( const TreeFolderUpNode& upNode ) const
		{
			for(TreeFolderUpNode* parent(_parent);
				parent;
				parent = dynamic_cast<TreeFolderDownNodeInterface*>(parent) ? dynamic_cast<TreeFolderDownNodeInterface*>(parent)->_parent : NULL
			){
				if(parent == &upNode)
				{
					return true;
				}
			}
			return false;
		}



		void TreeFolderDownNodeInterface::removeParentLink()
		{
			if(_parent)
			{
				_parent->removeChild(*this);
			}
		}

		void TreeFolderDownNodeInterface::setParentLink()
		{
			if(_parent)
			{
				_parent->addChild(*this);
			}
		}
}	}
