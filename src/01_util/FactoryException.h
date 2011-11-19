////////////////////////////////////////////////////////////////////////////////
/// FactoryException class header.
///	@file FactoryException.h
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

#ifndef SYNTHESE_FactoryException_H__
#define SYNTHESE_FactoryException_H__

#include "Exception.h"

#include <string>
#include <iostream>

namespace synthese
{
	namespace util
	{
		////////////////////////////////////////////////////////////////////
		/// Factory related exception class.
		/// @ingroup m01Factory m01Exceptions refExceptions
		template<class T>
		class FactoryException : public synthese::Exception
		{
		public:

			explicit FactoryException ( const std::string& message) : synthese::Exception(message) {}
			~FactoryException () throw () {}
		};
	}
}

#endif // SYNTHESE_FactoryException_H__
