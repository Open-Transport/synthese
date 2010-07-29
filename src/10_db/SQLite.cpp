
/** SQLite class implementation.
	@file SQLite.cpp

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

#include "SQLite.h"

#include "SQLiteException.h"

#include "Conversion.h"
#include "Log.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/tokenizer.hpp>

using namespace synthese::util;
using namespace std;



namespace synthese
{

    namespace db
    {
	
	
	
	
	SQLite::SQLite ()
	{
	}
	
	
	
	SQLite::~SQLite ()
	{
	}
	


	std::string 
	SQLite::GetLibVersion ()
	{
	    return std::string (sqlite3_libversion ());
	}



	SQLiteResultSPtr 
	SQLite::execQuery (const SQLData& sql, bool lazy)
	{
	    return execQuery (compileStatement (sql), lazy);
	}









	bool 
	SQLite::IsStatementComplete (const SQLData& sql)
	{
	    return sqlite3_complete (sql.c_str ());
	}


	bool 
	SQLite::IsUpdateStatement (const SQLData& sql)
	{
	    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	    boost::char_separator<char> sep (" ;\n\t");
	    tokenizer keyTokens (sql, sep);
	    
	    for (tokenizer::iterator tok_iter = keyTokens.begin(); 
		 tok_iter != keyTokens.end (); 
		 ++tok_iter) 
	    {
		std::string tok (boost::algorithm::to_upper_copy (*tok_iter));
		if (tok == "DROP") return true;
		if (tok == "DELETE") return true;
		if (tok == "UPDATE") return true;
		if (tok == "INSERT") return true;
		if (tok == "REPLACE") return true;
		if (tok == "CREATE") return true;
		if (tok == "ALTER") return true;
		std::cerr << "'" << tok << "'" << std::endl;
	    }
	    return false;
	    
	}


    }
}

