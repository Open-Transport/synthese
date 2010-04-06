
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

#include <set>
#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

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

		private:
			boost::shared_ptr<SQLExpression> _expr1;
			std::string _op;
			boost::shared_ptr<SQLExpression> _expr2;

		public:
			ComposedExpression(boost::shared_ptr<SQLExpression> expr1, std::string op, boost::shared_ptr<SQLExpression> expr2) : _expr1(expr1), _op(op), _expr2(expr2) {}
			virtual std::string toString() const { return _expr1->toString() + " " + _op + " " + _expr2->toString(); }
		};

		template<class T>
		class ValueExpression:
			public SQLExpression
		{
			const std::string _value;
		public:
			ValueExpression(const T& value) : _value(boost::lexical_cast<std::string>(value)) {}
			virtual std::string toString() const { return _value; }
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
		};
	}
}

#endif // SYNTHESE_db_SQLExpression_hpp__
