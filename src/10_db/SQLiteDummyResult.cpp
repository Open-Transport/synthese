
/** SQLiteDummyResult class implementation.
	@file SQLiteDummyResult.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "SQLiteDummyResult.h"
#include "SQLiteException.h"

using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	
	namespace db
	{
		SQLiteDummyResult::SQLiteDummyResult ()
		{
		}



		SQLiteDummyResult::~SQLiteDummyResult ()
		{

		}



		void 
		SQLiteDummyResult::reset () const
		{
		}



		bool 
		SQLiteDummyResult::next () const
		{
			return false;
		}




		int 
		SQLiteDummyResult::getNbColumns () const
		{
			return 0;
		}



		std::string
		SQLiteDummyResult::getColumnName (int column) const
		{
			throw SQLiteException ("Dummy result..."); 
		}




		int
		SQLiteDummyResult::getColumnIndex (const std::string& columnName) const
		{
			throw SQLiteException ("Dummy result..."); 
		}



		SQLiteValue	SQLiteDummyResult::getValue (int column) const
		{
			throw SQLiteException ("Dummy result..."); 
		}



		std::string
		SQLiteDummyResult::getText (int column) const
		{
			throw SQLiteException ("Dummy result..."); 
		}



		int 
		SQLiteDummyResult::getInt (int column) const
		{
			throw SQLiteException ("Dummy result..."); 
		}




		long
		SQLiteDummyResult::getLong (int column) const
		{
			throw SQLiteException ("Dummy result..."); 
		}






		bool 
		SQLiteDummyResult::getBool (int column) const
		{
			throw SQLiteException ("Dummy result..."); 
		}



		double 
		SQLiteDummyResult::getDouble (int column) const
		{
			throw SQLiteException ("Dummy result..."); 
		}





		std::string 
		SQLiteDummyResult::getBlob (int column) const
		{
			throw SQLiteException ("Dummy result..."); 
		}





		long long 
		SQLiteDummyResult::getLongLong (int column) const
		{
			throw SQLiteException ("Dummy result..."); 
		}




		boost::posix_time::ptime
		SQLiteDummyResult::getTimestamp (int column) const
		{
			throw SQLiteException ("Dummy result..."); 
		}



		boost::shared_ptr<SQLiteValue> SQLiteDummyResult::getValueSPtr( int column ) const
		{
			throw SQLiteException ("Dummy result..."); 
		}
	}
}
