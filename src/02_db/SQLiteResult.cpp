#include "SQLiteResult.h"

#include "Conversion.h"

#include "SQLiteException.h"
#include "sqlite3.h"

#include "02_db/Constants.h"

#include <iomanip>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	namespace db
	{


		SQLiteResult::SQLiteResult ()
		{
		}



		SQLiteResult::~SQLiteResult ()
		{
		}



		int
		SQLiteResult::getColumnIndex (const std::string& columnName) const
		{
			for (int i=0; i<getNbColumns (); ++i)
			{
			if (getColumnName (i) == columnName) return i;
			}
			return -1;
		}



		SQLiteValue*
		SQLiteResult::getValue (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getValue (index);
		}



		std::string 
		SQLiteResult::getText (int column) const
		{
			return getValue (column)->getText ();
		}


		std::string
		SQLiteResult::getText (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getText (index);
		}




		int 
		SQLiteResult::getInt (int column) const
		{
			return getValue (column)->getInt ();
		}

		int 
		SQLiteResult::getInt (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getInt (index);
		}



		long 
		SQLiteResult::getLong (int column) const
		{
			return getValue (column)->getLong ();
		}

		long
		SQLiteResult::getLong (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getLong (index);
		}




		bool
		SQLiteResult::getBool (int column) const
		{
			return getValue (column)->getBool ();
		}

		bool 
		SQLiteResult::getBool (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getBool (index);
		}



		boost::logic::tribool
		SQLiteResult::getTribool (int column) const
		{
			return getValue (column)->getTribool ();
		}



		boost::logic::tribool 
		SQLiteResult::getTribool (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getTribool (index);
		}




		double 
		SQLiteResult::getDouble (int column) const
		{
			return getValue (column)->getDouble ();
		}

		double 
		SQLiteResult::getDouble (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getDouble (index);
		}



		std::string 
		SQLiteResult::getBlob (int column) const
		{
			return getValue (column)->getBlob ();
		}

		std::string 
		SQLiteResult::getBlob (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getBlob (index);
		}





		long long 
		SQLiteResult::getLongLong (int column) const
		{
			return getValue (column)->getLongLong ();
		}


		long long 
		SQLiteResult::getLongLong (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getLongLong (index);

		}



		boost::posix_time::ptime 
		SQLiteResult::getTimestamp (int column) const
		{
			return getValue (column)->getTimestamp ();
		}


		boost::posix_time::ptime
		SQLiteResult::getTimestamp (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getTimestamp (index);
		}


		    
		SQLiteResultRow 
		SQLiteResult::getRow () const
		{
			SQLiteResultRow row;
			for (int i=0; i<getNbColumns (); ++i)
			{
			row.push_back (getValue (i));
			}
			return row;
		}





		std::vector<int>
		SQLiteResult::computeMaxColWidths () const
		{
			reset ();

			std::vector<int> widths;
			for (int c=0; c<getNbColumns (); ++c) 
			{
				int max = 0;
				std::string name (getColumnName (c));
				if (name.length () > max) max = name.length ();

				while (next ())
				{
					std::string value (getText (c));
					if (value.length () > max) max = value.length ();
				}
				widths.push_back (max);
			}
			return widths;
		}



		RegistryKeyType SQLiteResult::getKey() const
		{
			return getLongLong(TABLE_COL_ID);
		}

		boost::optional<int> SQLiteResult::getOptionalInt( const std::string& name ) const
		{
			string text(getText(name));
			if(text.empty()) return optional<int>();
			return lexical_cast<int>(text);
		}

		boost::optional<unsigned int> SQLiteResult::getOptionalUnsignedInt( const std::string& name ) const
		{
			string text(getText(name));
			if(text.empty()) return optional<unsigned int>();
			return lexical_cast<unsigned int>(text);
		}



		boost::posix_time::ptime SQLiteResult::getDateTime( const std::string& name ) const
		{
			try
			{
				string text(getText(name));
				return time_from_string(text);
			}
			catch(...)
			{
				return ptime(not_a_date_time);
			}
		}



		boost::gregorian::date SQLiteResult::getDate( const std::string& name ) const
		{
			try
			{
				string text(getText(name));
				return from_string(text);
			}
			catch(...)
			{
				return date(not_a_date_time);
			}
		}



		boost::posix_time::time_duration SQLiteResult::getHour( const std::string& name ) const
		{
			try
			{
				string text(getText(name));
				return duration_from_string(text);
			}
			catch(...)
			{
				return time_duration(not_a_date_time);
			}
		}



		std::ostream& 
		operator<< ( std::ostream& os, const SQLiteResult& op )
		{
			std::vector<int> widths (op.computeMaxColWidths ());

			for (int c=0; c<op.getNbColumns (); ++c) 
			{
				os << std::setw (widths.at(c)) << std::setfill (' ') << op.getColumnName (c);
				if (c != op.getNbColumns ()-1) os << " | ";
			}
			os << std::endl;

			op.reset ();
			while (op.next ())
			{
				for (int c=0; c<op.getNbColumns (); ++c) 
				{
					os << std::setw (widths.at(c)) << std::setfill (' ') << op.getText (c);
					if (c != op.getNbColumns ()-1) os << " | ";
				}
				os << std::endl;
			}
			return os;
		}


	}
}
