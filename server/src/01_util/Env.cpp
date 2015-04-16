/** Env class implementation.
	@file Env.cpp
	@author Hugues Romain
	@date 2008

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

#include "Env.h"

#include "DBModule.h"
#include "DBDirectTableSync.hpp"
#include "Registrable.h"

using namespace boost;

namespace synthese
{
	using namespace db;
	
	namespace util
	{
		boost::shared_ptr<Env> Env::_officialRegistries(new Env);
		Env::RegistryCreatorMap Env::_registryCreators;



		void Env::addRegistrable( boost::shared_ptr<util::Registrable> object )
		{
			if(!object.get())
			{
				return;
			}
			RegistryTableType tableId(decodeTableId(object->getKey()));
			boost::shared_ptr<DBTableSync> tableSync(DBModule::GetTableSync(tableId));
			if(!dynamic_cast<DBDirectTableSync*>(tableSync.get()))
			{
				throw synthese::Exception("Incompatible registry");
			}
			dynamic_cast<DBDirectTableSync&>(*tableSync).getEditableRegistry(*this).addRegistrable(object);
		}
	}
}
