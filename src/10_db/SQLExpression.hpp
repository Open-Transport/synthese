
/** SQLExpression class header.
	@file SQLExpression.hpp

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

#ifndef SYNTHESE_db_SQLExpression_hpp__
#define SYNTHESE_db_SQLExpression_hpp__

#include "DBModule.h"

#include <set>
#include <vector>
#include <string>
#include <geos/io/WKBWriter.h>
#include <geos/geom/Geometry.h>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/logic/tribool.hpp>

namespace synthese
{
	namespace db
	{
		/** SQLExpression class.
			@ingroup m10
		*/
		class SQLExpression
		{
		public:
			virtual std::string toString() const = 0;
			virtual ~SQLExpression() {}
		};

		class FieldExpression:
			public SQLExpression
		{
			const std::string _table;
			const std::string _field;

		public:
			FieldExpression(): _table(), _field() {}
			FieldExpression(const std::string& table, const std::string& field) : _table(table), _field(field) {}

			virtual std::string toString() const;
			static boost::shared_ptr<SQLExpression> Get(const std::string& table, const std::string& field);
		};

		class SubQueryExpression:
			public SQLExpression
		{
			const std::string _subquery;

		public:
			SubQueryExpression(const std::string& subQuery) : _subquery(subQuery) {}

			virtual std::string toString() const;
			static boost::shared_ptr<SQLExpression> Get(const std::string& subQuery);
		};


		class ComposedExpression:
			public SQLExpression
		{
		public:
			static const std::string OP_AND;
			static const std::string OP_OR;
			static const std::string OP_EQ;
			static const std::string OP_LIKE;
			static const std::string OP_SUP;
			static const std::string OP_SUPEQ;
			static const std::string OP_INF;
			static const std::string OP_INFEQ;
			static const std::string OP_BITAND;
			static const std::string OP_BITOR;
			static const std::string OP_IN;
			static const std::string OP_NOTIN;
			static const std::string OP_EXISTS;
			static const std::string OP_DIFF;

		private:
			boost::shared_ptr<SQLExpression> _expr1;
			std::string _op;
			boost::shared_ptr<SQLExpression> _expr2;

		public:
			ComposedExpression(boost::shared_ptr<SQLExpression> expr1, std::string op, boost::shared_ptr<SQLExpression> expr2) : _expr1(expr1), _op(op), _expr2(expr2) {}
			virtual std::string toString() const;
			static boost::shared_ptr<SQLExpression> Get(boost::shared_ptr<SQLExpression> expr1, std::string op, boost::shared_ptr<SQLExpression> expr2);
		};

		template<class T>
		class ValueExpression:
			public SQLExpression
		{
			const std::string _value;
		public:
			ValueExpression(const T& value) : _value(boost::lexical_cast<std::string>(value)) {}
			virtual std::string toString() const { return _value; }
			static boost::shared_ptr<SQLExpression> Get(const T& value);
		};

		class NotExpression:
			public SQLExpression
		{
			boost::shared_ptr<SQLExpression> _expression;

		public:
			NotExpression(boost::shared_ptr<SQLExpression> expression) : _expression(expression) {}

			virtual std::string toString() const;

			static boost::shared_ptr<SQLExpression> Get(boost::shared_ptr<SQLExpression> expression);
		};

		class IsNullExpression:
			public SQLExpression
		{
			boost::shared_ptr<SQLExpression> _expression;

		public:
			IsNullExpression(boost::shared_ptr<SQLExpression> expression) : _expression(expression) {}

			virtual std::string toString() const;

			static boost::shared_ptr<SQLExpression> Get(boost::shared_ptr<SQLExpression> expression);
		};



		template<class T>
		class ValueExpression<std::set<T> >:
			public SQLExpression
		{
			std::string _value;
		public:
			ValueExpression(const std::set<T>& value) {
				std::stringstream s;
				bool first(true);
				BOOST_FOREACH(const typename std::set<T>::value_type& o, value)
				{
					if(!first) s << ",";
					s << o;
					first = false;
				}
				_value = s.str();
			}
			virtual std::string toString() const { return _value; }
			static boost::shared_ptr<SQLExpression> Get(const std::set<T>& value);
		};


		template<class T>
		class ValueExpression<std::vector<T> >:
			public SQLExpression
		{
			std::string _value;
		public:
			ValueExpression(const std::vector<T>& value) {
				std::stringstream s;
				bool first(true);
				BOOST_FOREACH(const typename std::vector<T>::value_type& o, value)
				{
					if(!first) s << ",";
					s << o;
					first = false;
				}
				_value = s.str();
			}
			virtual std::string toString() const { return _value; }
			static boost::shared_ptr<SQLExpression> Get(const std::vector<T>& value);
		};


		template<>
		class ValueExpression<std::string>:
			public SQLExpression
		{
			std::string _value;
		public:
			ValueExpression(const std::string& value) {
				_value.push_back('\'');
				BOOST_FOREACH(char s, value)
				{
					// Escape several characters
					if (s == '\'')
						_value.push_back('\'');
					_value.push_back(s);
				}
				_value.push_back('\'');
			}
			virtual std::string toString() const { return _value; }
			static boost::shared_ptr<SQLExpression> Get(const std::string& value);
		};


		template<>
		class ValueExpression<boost::posix_time::ptime>:
			public SQLExpression
		{
			std::string _value;
		public:
			ValueExpression(const boost::posix_time::ptime& value) {
				std::stringstream s;
				if(value.is_not_a_date_time())
				{
					s << "NULL";
				}
				else
				{
					s << "'" <<
						boost::gregorian::to_iso_extended_string(value.date()) << " " << boost::posix_time::to_simple_string(value.time_of_day()) <<
						"'"
					;
				}
				_value = s.str();
			}
			virtual std::string toString() const { return _value; }
			static boost::shared_ptr<SQLExpression> Get(const boost::posix_time::ptime& value);
		};



		template<>
		class ValueExpression<boost::gregorian::date>:
			public SQLExpression
		{
			std::string _value;
		public:
			ValueExpression(const boost::gregorian::date& value) {
				std::stringstream s;
				if(value.is_not_a_date())
				{
					s << "NULL";
				}
				else
				{
					s << "'"
						<< boost::gregorian::to_iso_extended_string(value)
						<< "'";
				}
				_value = s.str();
			}
			virtual std::string toString() const { return _value; }
			static boost::shared_ptr<SQLExpression> Get(const boost::gregorian::date& value);
		};



		template<>
		class ValueExpression<boost::logic::tribool>:
			public SQLExpression
		{
			std::string _value;
		public:
			ValueExpression(const boost::logic::tribool& value) {
				_value = boost::lexical_cast<std::string>(value == true ? 1 : value == false ? 0 : -1);
			}
			virtual std::string toString() const { return _value; }
			static boost::shared_ptr<SQLExpression> Get(const boost::logic::tribool& value);
		};



		template <>
		class ValueExpression<boost::shared_ptr<geos::geom::Geometry> >:
			public SQLExpression
		{
			std::string _value;
		public:
			ValueExpression(const boost::shared_ptr<geos::geom::Geometry>& value)
			{
				if(value.get() && !value->isEmpty())
				{
					std::stringstream str;
					if(DBModule::GetStorageCoordinatesSystem().getSRID() != value->getSRID())
					{
						str << "Transform(";
					}
					str << "GeomFromWKB(X'";
					geos::io::WKBWriter writer;
					writer.writeHEX(*value, str);
					str << "'," << value->getSRID() << ")";
					if(DBModule::GetStorageCoordinatesSystem().getSRID() != value->getSRID())
					{
						str << "," << DBModule::GetStorageCoordinatesSystem().getSRID() << ")";
					}
					_value = str.str();
				}
				else
				{
					_value ="NULL";
				}
			}
			virtual std::string toString() const { return _value; }
			static boost::shared_ptr<SQLExpression> Get(const boost::shared_ptr<geos::geom::Geometry>& value)
			{
				return boost::shared_ptr<SQLExpression>(
					static_cast<SQLExpression*>(new ValueExpression<boost::shared_ptr<geos::geom::Geometry> >(value))
				);
			}
		};





		template<class T>
		boost::shared_ptr<SQLExpression> ValueExpression<T>::Get( const T& value )
		{
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new ValueExpression<T>(value))
			);
		}



		template<class T>
		boost::shared_ptr<SQLExpression> ValueExpression<std::set<T> >::Get( const std::set<T>& value )
		{
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new ValueExpression<std::set<T> >(value))
			);
		}



		template<class T>
		boost::shared_ptr<SQLExpression> ValueExpression<std::vector<T> >::Get( const std::vector<T>& value )
		{
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new ValueExpression<std::vector<T> >(value))
			);
		}
	}
}

#endif // SYNTHESE_db_SQLExpression_hpp__
