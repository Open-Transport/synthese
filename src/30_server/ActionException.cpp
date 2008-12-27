////////////////////////////////////////////////////////////////////////////////
/// ActionException class implementation.
///	@file ActionException.cpp
///	@author Hugues Romain
///	@date 2008-12-26 18:36
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

#include "ActionException.h"
#include "Conversion.h"

namespace synthese
{
	using namespace util;

	namespace server
	{

		ActionException::ActionException(
			const std::string& message
		) throw() :
			Exception(message)
		{
		}



		ActionException::ActionException(
			const std::string& field,
			const std::string& source,
			const Exception& e
		) throw():
			Exception("Specified "+ field +" not found in "+ source + ":" + e.getMessage())
		{

		}



		ActionException::ActionException(
			const std::string& field, util::RegistryKeyType id, const std::string& source, const Exception& e
		) throw() :
			Exception("Specified "+ field + Conversion::ToString(id) +" not found in "+ source + ":" + e.getMessage())
		{

		}
		ActionException::~ActionException() throw ()
		{

		}
	}
}
