////////////////////////////////////////////////////////////////////////////////
/// SQLite class header.
///	@file SQLite.h
///	@author Marc Jambert
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

#ifndef SYNTHESE_DB_SQLITEDB_H
#define SYNTHESE_DB_SQLITEDB_H

#include "02_db/SQLiteStatement.h"
#include "02_db/SQLiteResult.h"



namespace synthese
{
	namespace db
	{

			  

		/** SQLite interface class.
		    @ingroup m10
		*/
		class SQLite
		{

		private:

		protected:

			SQLite ();
			virtual ~SQLite ();

		public:


		    
			//! @name SQLite db access methods.
			//@{
			static std::string GetLibVersion ();


			virtual SQLiteStatementSPtr compileStatement (const SQLData& sql) = 0;

			virtual SQLiteResultSPtr execQuery (const SQLiteStatementSPtr& statement, bool lazy = false) = 0;
			virtual SQLiteResultSPtr execQuery (const SQLData& sql, bool lazy = false) ;

			virtual void execUpdate (const SQLiteStatementSPtr& statement) = 0;
			virtual void execUpdate (const SQLData& sql) = 0;

			/** Returns true if a transaction is already opened.
			    SQLite does not support nested transaction.
			*/
			// virtual bool isTransactionOpened () ;

			static bool IsStatementComplete (const SQLData& sql);

			static bool IsUpdateStatement (const SQLData& sql);

			//@}

		};


	}
}

#endif

