
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

#include "SimpleObjectField.hpp"
#include "SchemaMacros.hpp"

namespace synthese
{
	struct Parent;

	namespace tree
	{
		class TreeFolder;
		class TreeFolderRoot;
		class TreeFolderUpNode;

		/** TreeFolderDownNodeInterface class.
			@ingroup m20
		*/
		class TreeFolderDownNodeInterface
		{
		private:
			TreeFolderRoot* _root;
			TreeFolderUpNode*	_parent;

		protected:
			TreeFolderDownNodeInterface():
				_root(NULL),
				_parent(NULL)
			{}
			virtual ~TreeFolderDownNodeInterface() {}

		

		public:
			void setNullParent();
			void _setParent(TreeFolderUpNode& value);
			TreeFolderRoot* _getRoot() const {	return _root; }
			TreeFolderUpNode* _getParent() const { return _parent; }


			template<class T>
			T* getCastRoot() const
			{
				return dynamic_cast<T*>(_root);
			}



			TreeFolder* getParentFolder() const;


			friend class ObjectField<Parent, tree::TreeFolderUpNode*>;
		};
	}



	//////////////////////////////////////////////////////////////////////////
	/// Pointers vector specialization
	template<>
	class ObjectField<Parent, tree::TreeFolderUpNode*>:
		public SimpleObjectFieldDefinition<Parent>
	{
	public:
		typedef tree::TreeFolderUpNode* Type;

		static void LoadFromRecord(tree::TreeFolderUpNode* fieldObject, ObjectBase& object, const Record& record, const util::Env& env);
		static void SaveToParametersMap(tree::TreeFolderUpNode* fieldObject, const ObjectBase& object, util::ParametersMap& map, const std::string& prefix);
		static void SaveToParametersMap(tree::TreeFolderUpNode* fieldObject, util::ParametersMap& map, const std::string& prefix);
		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record);
	};



	struct Parent : public ObjectField<Parent, tree::TreeFolderUpNode*> {};
}

#endif // SYNTHESE_tree_TreeFolderDownNodeInterface_hpp__
