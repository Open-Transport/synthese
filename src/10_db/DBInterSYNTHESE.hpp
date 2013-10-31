
/** InterSYNTHESEDB class header.
	@file InterSYNTHESEDB.hpp

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

#ifndef SYNTHESE_db_InterSYNTHESEDB_hpp__
#define SYNTHESE_db_InterSYNTHESEDB_hpp__

#include "FactorableTemplate.h"
#include "InterSYNTHESESyncTypeFactory.hpp"

#include "FrameworkTypes.hpp"

#include <boost/variant.hpp>

namespace synthese
{
	namespace db
	{
		class DBRecord;
		class DBTransaction;

		//////////////////////////////////////////////////////////////////////////
		/// InterSYNTHESE DB class.
		///	@ingroup m10
		/// Messages schema :
		///  - sql / rstmt = SQL query or replace prepared statement
		///  If SQL :
		///    - SQL query
		///  If RSTMT : 
		///    - table name
		///    - each field :
		///      - field name
		///      - is null
		///      - size of the content
		///      - content
		/// the data fields are separated by : 
		class DBInterSYNTHESE:
			public util::FactorableTemplate<inter_synthese::InterSYNTHESESyncTypeFactory, DBInterSYNTHESE>
		{
		public:
			static const std::string TYPE_SQL;
			static const std::string TYPE_REPLACE_STATEMENT;
			static const std::string TYPE_DELETE_STATEMENT;
			static const std::string FIELD_SEPARATOR;

			DBInterSYNTHESE();

			mutable std::auto_ptr<DBTransaction> _transaction;
			
			virtual bool mustBeEnqueued(
				const std::string& configPerimeter,
				const std::string& messagePerimeter
			) const;

			virtual void initSync(
			) const;

			virtual bool sync(
				const std::string& parameter,
				const inter_synthese::InterSYNTHESEIdFilter* idFilter
			) const;

			virtual void closeSync(
			) const;

			virtual void initQueue(
				const inter_synthese::InterSYNTHESESlave& slave,
				const std::string& perimeter
			) const;

			class RequestEnqueue:
				public boost::static_visitor<bool>
			{
				std::stringstream& _result;

			public:
				RequestEnqueue(
					std::stringstream& result
				);

				// SQL query
				bool operator()(const std::string& sql);

				// Replace statement
				bool operator()(const DBRecord& r);

				// Delete statement
				bool operator()(util::RegistryKeyType id);
			};

			class ContentGetter:
				public boost::static_visitor<>
			{
				std::stringstream& _result;

			public:
				ContentGetter(
					std::stringstream& result
				);

				void operator()(const int& i) const;
				void operator()(const double& d) const;
#ifndef _WINDOWS
				void operator()(const size_t& s) const;
#endif
				void operator()(const util::RegistryKeyType& id) const;
				void operator()(const boost::optional<std::string>& str) const;
				void operator()(const boost::optional<Blob>& blob) const;
				void operator()(const boost::shared_ptr<geos::geom::Geometry>& geom) const;
			};
		};
}	}

#endif // SYNTHESE_db_InterSYNTHESEDB_hpp__

