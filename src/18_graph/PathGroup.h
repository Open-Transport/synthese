////////////////////////////////////////////////////////////////////////////////
/// PathGroup class header.
///	@file PathGroup.h
///	@author Hugues Romain (RCS)
///	@date dim jan 25 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_PathGroup_h__
#define SYNTHESE_PathGroup_h__

#include "Registrable.h"
#include "Registry.h"
#include "RuleUser.h"

#include <set>

namespace synthese
{
	namespace graph
	{
		class Path;

		/////////////////////////////////////////////////////////////////////////
		/// Path group representing a human readable object.
		/// @ingroup m18
		class PathGroup
		:	public RuleUser,
			public virtual util::Registrable
		{
		public:
			typedef std::set<Path*> Paths;

		protected:
			Paths _paths;


			PathGroup(
				util::RegistryKeyType id = 0
			);
			virtual ~PathGroup();

		public:
			//! @name Getters
			//@{
				const Paths& getPaths() const { return _paths; }
			//@}


			//! @name Modifiers
			//@{
				void addPath(Path* path);
				void removePath(Path* path);
			//@}

			//! @name Services
			//@{
				virtual const RuleUser* _getParentRuleUser() const;
			//@}
		};
	}
}

#endif
