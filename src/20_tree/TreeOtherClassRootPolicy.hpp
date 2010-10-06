
/** TreeOtherClassRootPolicy class header.
	@file TreeOtherClassRootPolicy.h

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

#ifndef SYNTHESE_tree_TreeOtherClassRootPolicy_h__
#define SYNTHESE_tree_TreeOtherClassRootPolicy_h__

namespace synthese
{
	namespace tree
	{
		/** TreeOtherClassRootPolicy class.
			@ingroup m20
		*/
		template<class O>
		class TreeOtherClassRootPolicy
		{
		public:
			typedef O TreeRootType;

		private:
			TreeRootType* _root;

		public:
			TreeOtherClassRootPolicy() : _root(NULL) {}

			TreeRootType* getRoot() const { return _root; }

			void setRoot(TreeRootType* value) { _root=value; }

			void setSameRoot(const TreeOtherClassRootPolicy<O>& value){ _root = value._root; }

			void setNullRoot(){ _root = NULL; }
		};
	}
}

#endif // SYNTHESE_tree_TreeOtherClassRootPolicy_h__
