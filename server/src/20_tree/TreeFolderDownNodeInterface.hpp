
/** TreeFolderDownNodeInterface class header.
	@file TreeFolderDownNodeInterface.hpp

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

#ifndef SYNTHESE_tree_TreeFolderDownNodeInterface_hpp__
#define SYNTHESE_tree_TreeFolderDownNodeInterface_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"

#include <boost/logic/tribool.hpp>

namespace synthese
{
	class ObjectBase;

	namespace util
	{
		class Env;
	}

	namespace tree
	{
		class TreeFolder;
		class TreeFolderRoot;
		class TreeFolderUpNode;

		/** TreeFolderDownNodeInterface class.
			@ingroup m20
		*/
		class TreeFolderDownNodeInterface:
			public SimpleObjectFieldDefinition<TreeFolderDownNodeInterface>
		{
		public:
			typedef tree::TreeFolderUpNode* Type;

		private:
			util::RegistryTableType _tableId;
			TreeFolderRoot* _root;
			TreeFolderUpNode*	_parent;

		protected:
			TreeFolderDownNodeInterface():
				_tableId(0),
				_root(NULL),
				_parent(NULL)
			{}
			virtual ~TreeFolderDownNodeInterface() {}

		

		public:
			void setNullParent();
			void removeParentLink();
			void setParentLink();
			void _setParent(TreeFolderUpNode& value);

			/// @name Getters
			//@{
				TreeFolderRoot* _getRoot() const {	return _root; }
				TreeFolderUpNode* _getParent() const { return _parent; }
				util::RegistryTableType getTableId() const { return _tableId; }
			//@}

			/// @name Services
			//@{
				void setTableId(util::RegistryTableType value){ _tableId = value; }

				template<class T>
				T* getCastRoot() const
				{
					return dynamic_cast<T*>(_root);
				}



				//////////////////////////////////////////////////////////////////////////
				/// Check if the object is a child of a specified node.
				/// @param upNode the node which should be a parent
				/// @return true if the upNode is a parent of the current object
				bool isChildOf(const TreeFolderUpNode& upNode) const;
			//@}



			TreeFolder* getParentFolder() const;

			static bool LoadFromRecord(
				Type& fieldObject,
				ObjectBase& object,
				const Record& record,
				const util::Env& env
			);

			static void SaveToParametersMap(
				const Type& fieldObject,
				const ObjectBase& object,
				util::ParametersMap& map,
				const std::string& prefix,
				boost::logic::tribool withFiles
			);

			static void SaveToParametersMap(
				const Type& fieldObject,
				util::ParametersMap& map,
				const std::string& prefix,
				boost::logic::tribool withFiles
			);

			static void SaveToFilesMap(
				const Type& fieldObject,
				const ObjectBase& object,
				FilesMap& map
			);

			static void SaveToDBContent(
				const Type& fieldObject,
				const ObjectBase& object,
				DBContent& content
			);

			static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record);
		};

		// Field class
		typedef TreeFolderDownNodeInterface Parent;
}	}

#endif // SYNTHESE_tree_TreeFolderDownNodeInterface_hpp__
