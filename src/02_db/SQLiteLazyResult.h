#ifndef SYNTHESE_DB_SQLITELAZYRESULT_H
#define SYNTHESE_DB_SQLITELAZYRESULT_H

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteStatement.h"


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
