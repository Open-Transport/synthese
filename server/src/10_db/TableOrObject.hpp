
/** TableOrObject class header.
	@file TableOrObject.hpp

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

#ifndef SYNTHESE_db_TableOrObject_hpp__
#define SYNTHESE_db_TableOrObject_hpp__

#include "UtilTypes.h"

#include "Registrable.h"

#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
		class ParametersMap;
	}

	namespace db
	{
		class DBTableSync;

		/** TableOrObject class.
			@ingroup m10
		*/
		class TableOrObject
		{
		public:
			static const std::string TAG_OBJECT;
			static const std::string ATTR_ID;
			static const std::string ATTR_NAME;
			
			typedef boost::variant<
				boost::shared_ptr<DBTableSync>,
				boost::shared_ptr<util::Registrable>,
				util::Registrable*
			> Value;

		private:
			Value _value;

			class KeyGetter:
				public boost::static_visitor<util::RegistryKeyType>
			{
			public:
				util::RegistryKeyType operator()(boost::shared_ptr<DBTableSync> value) const;
				util::RegistryKeyType operator()(boost::shared_ptr<util::Registrable> value) const;
				util::RegistryKeyType operator()(util::Registrable* value) const;
			};

			class ParametersMapExporter:
				public boost::static_visitor<>
			{
			private:
				util::ParametersMap& _pm;

			public:
				ParametersMapExporter(util::ParametersMap& pm);

				void operator()(boost::shared_ptr<DBTableSync> value) const;
				void operator()(boost::shared_ptr<util::Registrable> value) const;
				void operator()(util::Registrable* value) const;
			};

			class SubObjectsGetter:
				public boost::static_visitor<synthese::SubObjects>
			{
				util::Env& _env;
			public:
				SubObjectsGetter(util::Env& env);

				synthese::SubObjects operator()(boost::shared_ptr<DBTableSync> value) const;
				synthese::SubObjects operator()(boost::shared_ptr<util::Registrable> value) const;
				synthese::SubObjects operator()(util::Registrable* value) const;
			};

			class ObjectGetter:
				public boost::static_visitor<util::Registrable*>
			{
			public:
				util::Registrable* operator()(boost::shared_ptr<DBTableSync> value) const;
				util::Registrable* operator()(boost::shared_ptr<util::Registrable> value) const;
				util::Registrable* operator()(util::Registrable* value) const;
			};

		public:
			TableOrObject(boost::shared_ptr<DBTableSync> value);
			TableOrObject(boost::shared_ptr<util::Registrable> value);
			TableOrObject(util::Registrable* value);
			TableOrObject(util::RegistryKeyType id, util::Env& env);
			TableOrObject(util::RegistryKeyType id, const util::Env& env);

			util::RegistryKeyType getKey() const;
			Value getValue() const { return _value; }

			void toParametersMap(
				util::ParametersMap& pm
			) const;

			bool operator==(const TableOrObject& op) const;

			synthese::SubObjects getSubObjects(
				util::Env& env
			) const;

			util::Registrable* getObject() const;
		};
	}
}

#endif // SYNTHESE_db_TableOrObject_hpp__
