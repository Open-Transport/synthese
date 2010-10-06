
/** TreeMultiClassRootPolicy class header.
	@file TreeMultiClassRootPolicy.hpp

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

#ifndef SYNTHESE_tree_TreeMultiClassRootPolicy_hpp__
#define SYNTHESE_tree_TreeMultiClassRootPolicy_hpp__

#include <boost/static_assert.hpp>

namespace synthese
{
	namespace tree
	{
		/** TreeMultiClassRootPolicy class.
			@ingroup m20
		*/
		template<class C1, class C2>
		class TreeMultiClassRootPolicy
		{
		public:
			typedef C1 TreeRootType1;
			typedef C2 TreeRootType2;

		private:
			C1* _root1;
			C2* _root2;

			C1* _getRoot(std::auto_ptr<C1>) const { return _root1; }
			C2* _getRoot(std::auto_ptr<C2>) const { return _root2; }

		public:
			TreeMultiClassRootPolicy() : _root1(NULL), _root2(NULL) {}

			template<class C>
			C* getRoot() const { return _getRoot(std::auto_ptr<C>()); }

			void setRoot(C1* value){ _root1 = value; _root2 = NULL; }
			void setRoot(C2* value){ _root2 = value; _root1 = NULL; }

			void setSameRoot(const TreeMultiClassRootPolicy<C1, C2>& value){ _root1 = value._root1; _root2 = value._root2; }

			void setNullRoot(){ _root1 = NULL; _root2 = NULL; }
		};
	}
}

#endif // SYNTHESE_tree_TreeMultiClassRootPolicy_hpp__
