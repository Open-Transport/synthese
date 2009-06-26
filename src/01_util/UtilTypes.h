////////////////////////////////////////////////////////////////////////////////
/// UtilTypes class header.
///	@file UtilTypes.h
///	@author Hugues Romain
///	@date 2008-12-26 17:49
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#ifndef SYNTHESE_util_UtilTypes_h__
#define SYNTHESE_util_UtilTypes_h__

namespace synthese
{
	namespace util
	{
		typedef signed long long int RegistryKeyType;

		typedef enum
		{
			UNKNOWN_LOAD_LEVEL = -1,
			FIELDS_ONLY_LOAD_LEVEL = 0,
			UP_LINKS_LOAD_LEVEL = 5,
			DOWN_LINKS_LOAD_LEVEL = 6,
			UP_DOWN_LINKS_LOAD_LEVEL = 10,
			RECURSIVE_LINKS_LOAD_LEVEL = 20,
			ALGORITHMS_OPTIMIZATION_LOAD_LEVEL = 30
		} LinkLevel;
	}
}

#endif // SYNTHESE_util_UtilTypes_h__
