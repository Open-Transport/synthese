
/** SQLiteLazyResult class header.
	@file SQLiteLazyResult.h

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

#ifndef SYNTHESE_DB_SQLITELAZYRESULT_H
#define SYNTHESE_DB_SQLITELAZYRESULT_H

#include "SQLiteResult.h"
#include "SQLiteStatement.h"


namespace synthese
{

	namespace db
	{
		//////////////////////////////////////////////////////////////////////////
		/// Implementation class for access to a SQLite query result.
		/// @author Marc Jambert
		/// @ingroup m10
		/// @date 2007
		class SQLiteLazyResult:
			public SQLiteResult
		{
		 public:

		 private:
			SQLiteStatementSPtr _statement;
			mutable int _pos;

			SQLiteLazyResult (SQLiteStatementSPtr statement);

		 public:

			~SQLiteLazyResult ();
		    
			//! @name Query methods.
			//@{
				virtual void reset () const;
				virtual bool next () const;

				virtual int getNbColumns () const;

				virtual std::string getColumnName (int column) const;

				virtual SQLiteValue getValue (int column) const;
				virtual boost::shared_ptr<SQLiteValue> getValueSPtr(int column) const;
			//@}

		 private:

			friend class SQLiteHandle;
		};
	}
}

#endif
