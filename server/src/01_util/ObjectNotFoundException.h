////////////////////////////////////////////////////////////////////////////////
/// ObjectNotFoundException class header.
///	@file ObjectNotFoundException.h
///	@author Hugues Romain
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

#ifndef SYNTHESE_util_ObjectNotFoundException_h__
#define SYNTHESE_util_ObjectNotFoundException_h__

#include "Exception.h"

#include <boost/lexical_cast.hpp>

namespace synthese
{
	namespace util
	{
		/** ObjectNotFoundException class.
			@ingroup m01Exceptions refExceptions
		*/
		template<class T>
		class ObjectNotFoundException : public synthese::Exception
		{
			const RegistryKeyType _key;

		public:
			ObjectNotFoundException(const RegistryKeyType key, const std::string& message)
				: Exception(message + " (key=" + boost::lexical_cast<std::string>(key) + ")")
				, _key(key)
			{}

			const RegistryKeyType& getKey() const
			{
				return _key;
			}

		};
	}
}

#endif // SYNTHESE_util_ObjectNotFoundException_h__
