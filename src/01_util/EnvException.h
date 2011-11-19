
/// EnvException class header.
///	@file EnvException.h
///	@author Hugues Romain
///	@date 2008
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

#ifndef SYNTHESE_util_EnvException_h__
#define SYNTHESE_util_EnvException_h__

#include "Exception.h"

namespace synthese
{
	namespace util
	{
		/** Registries environment exception class.
			@ingroup m01Registry
		*/
		class EnvException
			: public synthese::Exception
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param key of the desired registry
			/// Occurs when the key of the registry cannot be accepted :
			///		- when the source code attempt to integrate two registries with the same key
			///		- when a non existent registry is specified at the execution
			/// To avoid EnvException, check the possible values of the key and check that
			/// all registries are correctly integrated in the *.gen.cpp files.
			//////////////////////////////////////////////////////////////////////////
			EnvException(const std::string& key);

		};
	}
}

#endif // SYNTHESE_util_EnvException_h__
