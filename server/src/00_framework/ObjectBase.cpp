
/** ObjectBase class implementation.
	@file ObjectBase.cpp

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

#include "ObjectBase.hpp"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{

	std::map<util::RegistryTableType, std::string>
	ObjectBase::REGISTRY_NAMES_BY_TABLE_ID;
	
	ObjectBase::IntegrityException::IntegrityException(
		const ObjectBase& object,
		const std::string& field,
		const std::string& value,
		const std::string& problem
	):	Exception(
			"Object integrity check failed : object "+ object.getName() +
			" ("+ lexical_cast<string>(object.getKey()) + ") has bad value for field " +
			field + " (" + value +") : " + problem
		)
	{}



	ObjectBase::IntegrityException::IntegrityException(
		const ObjectBase& object,
		const std::string& problem
	):	Exception(
			"Object integrity check failed : object " + object.getName() +
			" ("+ lexical_cast<string>(object.getKey()) + ") : " + problem
		)
	{}
}

