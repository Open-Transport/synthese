
/** InterSYNTHESEDB class implementation.
	@file InterSYNTHESEDB.cpp

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

#include "DBInterSYNTHESE.hpp"

#include "DBException.hpp"
#include "DBDirectTableSync.hpp"
#include "DBModule.h"
#include "DBTransaction.hpp"
#include "Env.h"
#include "InterSYNTHESESlave.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace inter_synthese;
	using namespace util;

	template<>
	const string FactorableTemplate<InterSYNTHESESyncTypeFactory, db::DBInterSYNTHESE>::FACTORY_KEY = "db";

	namespace db
	{
		DBInterSYNTHESE::DBInterSYNTHESE():
			FactorableTemplate<InterSYNTHESESyncTypeFactory, DBInterSYNTHESE>()
		{
		}



		bool DBInterSYNTHESE::sync( const std::string& parameter ) const
		{
			try
			{
				DBModule::GetDB()->execQuery(parameter);
			}
			catch(...)
			{
				return false;
			}
			return true;
		}



		void DBInterSYNTHESE::initQueue(
			const InterSYNTHESESlave& slave,
			const std::string& perimeter
		) const	{

			try
			{
				// Detection of the table by id
				RegistryTableType tableId(
					lexical_cast<RegistryTableType>(perimeter)
				);
				shared_ptr<DBTableSync> tableSync(
					DBModule::GetTableSync(
						tableId
				)	);

				shared_ptr<DBDirectTableSync> directTableSync(
					dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
						tableSync
				)	);
				if(!directTableSync.get())
				{
					return;
				}

				// Getting all requests
				Env env;
				DBDirectTableSync::RegistrableSearchResult result(
					directTableSync->search(
						string(),
						env
				)	);

				// Build the dump
				DBTransaction transaction;
				BOOST_FOREACH(const DBDirectTableSync::RegistrableSearchResult::value_type& it, result)
				{
					directTableSync->saveRegistrable(*it, transaction);
				}

				// Enqueue
				slave.enqueue(
					DBInterSYNTHESE::FACTORY_KEY,
					transaction.getSQL()
				);
			}
			catch (bad_lexical_cast&)
			{
			}
			catch(DBException&)
			{
			}
		}
}	}

