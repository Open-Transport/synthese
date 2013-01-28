
/** DBResult class header.
	@file DBResult.hpp

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

#ifndef SYNTHESE_db_DBResult_hpp__
#define SYNTHESE_db_DBResult_hpp__

#include "Record.hpp"

#include "UtilTypes.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/optional.hpp>
#include <iostream>
#include <vector>

namespace geos
{
	namespace geom
	{
		class Geometry;
		class GeometryFactory;
	}
}

namespace synthese
{

	namespace db
	{
		class DBResult;

		typedef boost::shared_ptr<DBResult> DBResultSPtr;

		//////////////////////////////////////////////////////////////////////////
		/// Interface for access to a database query result.
		/// @ingroup m10
		/// @author Marc Jambert
		class DBResult:
			public Record
		{
		private:
			mutable int _pos;


			//////////////////////////////////////////////////////////////////////////
			/// Same as getColumnIndex(), but throws a DBException exception if the column doesn't exist.
			/// @author Sylvain Pasche
			unsigned int _getColumnIndexInternal(const std::string& columnName) const;

		protected:

			DBResult();
			virtual ~DBResult ();

			void resetPosition() const;
			void incrementPosition() const;
			void ensurePosition() const;

		public:

			//! @name Query methods.
			//@{

			virtual void reset () const = 0;
			virtual bool next () const = 0;

			virtual int getNbColumns () const = 0;

			virtual std::string getColumnName (int column) const = 0;
			int getColumnIndex (const std::string& columnName) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the list of the available fields
			virtual FieldNames getFieldNames() const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the value of a field
			/// @param fieldName name of the parameter to read
			/// @param exceptionIfMissing throws an exception if the parameter is undefined, else returns an empty string
			/// @return the value of the parameter
			virtual std::string getValue(
				const std::string& fieldName,
				bool exceptionIfMissing = true
			) const;

			virtual bool isDefined(const std::string& fieldName) const;

			virtual std::string getText (int column) const = 0;
			std::string getText (const std::string& name) const;

			virtual int getInt (int column) const = 0;
			int getInt (const std::string& name) const;

			boost::optional<int> getOptionalInt(const std::string& name) const;
			boost::optional<std::size_t> getOptionalUnsignedInt(const std::string& name) const;

			virtual long long getLongLong (int column) const = 0;
			long long getLongLong (const std::string& name) const;

			bool getBool (int column) const;
			bool getBool (const std::string& name) const;

			boost::logic::tribool getTribool (int column) const;
			boost::logic::tribool getTribool (const std::string& name) const;

			virtual double getDouble (int column) const = 0;
			double getDouble (const std::string& name) const;

			std::string getBlob(int column) const;
			std::string getBlob(const std::string& name) const;

			boost::posix_time::ptime getDateTime(const std::string& name) const;
			boost::gregorian::date getDate(const std::string& name) const;
			// TODO: call it getTime for consistency?
			boost::posix_time::time_duration getHour(const std::string& name) const;

			std::vector<int> computeMaxColWidths () const;

			util::RegistryKeyType getKey() const;
		};


		std::ostream& operator<< ( std::ostream& os, const DBResult& op );
	}
}

#endif // SYNTHESE_db_DBResult_hpp__
