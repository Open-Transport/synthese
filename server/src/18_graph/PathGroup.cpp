////////////////////////////////////////////////////////////////////////////////
///	PathGroup class implementation.
///	@file PathGroup.cpp
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

#include "PathGroup.h"

#include "Path.h"

namespace synthese
{
	using namespace util;

	namespace graph
	{
		PathGroup::PathGroup(
			RegistryKeyType id
		):	Registrable(id)
		{}



		const RuleUser* PathGroup::_getParentRuleUser() const
		{
			return NULL;
		}



		void PathGroup::addPath( Path* path )
		{
			if(_paths.find(path) != _paths.end()) return;

			_paths.insert(path);
		}



		void PathGroup::removePath( Path* path )
		{
			Paths::iterator it(_paths.find(path));

			if(it == _paths.end()) return;

			_paths.erase(it);
		}



		PathGroup::~PathGroup()
		{
			BOOST_FOREACH(Path* path, _paths)
			{
				path->setPathGroup(NULL);
			}
		}
}	}
