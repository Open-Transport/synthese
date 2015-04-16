
/** TableOrObject class implementation.
	@file TableOrObject.cpp

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

#include "TableOrObject.hpp"

#include "DBDirectTableSync.hpp"
#include "DBModule.h"
#include "DBTableSync.hpp"
#include "Registrable.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;

	namespace db
	{
		const string TableOrObject::TAG_OBJECT = "object";
		const string TableOrObject::ATTR_ID = "id";
		const string TableOrObject::ATTR_NAME = "name";



		TableOrObject::TableOrObject(
			boost::shared_ptr<DBTableSync> value
		):	_value(value)
		{}



		TableOrObject::TableOrObject(
			boost::shared_ptr<util::Registrable> value
		):	_value(value)
		{}



		TableOrObject::TableOrObject(
			util::RegistryKeyType id,
			util::Env& env
		):	_value(
				decodeTableId(id) ?
				Value(DBModule::GetEditableObject(id, env)) :
				Value(DBModule::GetTableSync(static_cast<RegistryTableType>(id)))
			)
		{}



		TableOrObject::TableOrObject(
			util::RegistryKeyType id,
			const util::Env& env 
		):	_value(
				decodeTableId(id) ?
				Value(DBModule::GetEditableObject(id, const_cast<util::Env&>(env))) :
				Value(DBModule::GetTableSync(static_cast<RegistryTableType>(id)))
			)
		{}



		TableOrObject::TableOrObject( util::Registrable* value ):
			_value(value)
		{

		}



		RegistryKeyType TableOrObject::KeyGetter::operator()(
			boost::shared_ptr<DBTableSync> value
		) const {
			return value ? static_cast<RegistryKeyType>(value->getFormat().ID) : 0;
		}



		RegistryKeyType TableOrObject::KeyGetter::operator()(
			boost::shared_ptr<util::Registrable> value
		) const {
			return operator()(value.get());
		}



		RegistryKeyType TableOrObject::KeyGetter::operator()(
			Registrable* value
		) const	{
			return value ? value->getKey() : 0;
		}
	


		RegistryKeyType TableOrObject::getKey() const
		{
			return apply_visitor(KeyGetter(), _value);
		}



		TableOrObject::ParametersMapExporter::ParametersMapExporter(
			ParametersMap& pm
		):	_pm(pm)
		{}



		void TableOrObject::ParametersMapExporter::operator()( boost::shared_ptr<DBTableSync> value ) const
		{
			boost::shared_ptr<ParametersMap> tablePM(new ParametersMap);
			tablePM->insert(ATTR_ID, lexical_cast<string>(value->getFormat().ID));
			tablePM->insert(ATTR_NAME, value->getFormat().NAME);
			_pm.insert(TAG_OBJECT, tablePM);
		}



		void TableOrObject::ParametersMapExporter::operator()( boost::shared_ptr<util::Registrable> value ) const
		{
			operator()(value.get());
		}



		void TableOrObject::ParametersMapExporter::operator()( util::Registrable* value ) const
		{
			boost::shared_ptr<ParametersMap> objectPM(new ParametersMap);
			value->toParametersMap(*objectPM, false);
			_pm.insert(TAG_OBJECT, objectPM);
		}



		void TableOrObject::toParametersMap(
			util::ParametersMap& pm
		) const	{
			apply_visitor(ParametersMapExporter(pm), _value);
		}



		bool TableOrObject::operator==( const TableOrObject& op ) const
		{
			return _value == op._value;
		}



		synthese::SubObjects TableOrObject::SubObjectsGetter::operator()(
			boost::shared_ptr<DBTableSync> value
		) const	{
			synthese::SubObjects result;

			// Export of all objects of the table
			boost::shared_ptr<DBDirectTableSync> directTableSync(
				boost::dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
					value
			)	);
			if(directTableSync.get())
			{
				DBDirectTableSync::RegistrableSearchResult objects(
					directTableSync->search(
						string(),
						_env
				)	);
				BOOST_FOREACH(const DBDirectTableSync::RegistrableSearchResult::value_type& item, objects)
				{
					result.push_back(item.get());
				}
			}

			return result;
		}



		synthese::SubObjects TableOrObject::SubObjectsGetter::operator()( boost::shared_ptr<util::Registrable> value ) const
		{
			return operator()(value.get());
		}



		synthese::SubObjects TableOrObject::SubObjectsGetter::operator()( util::Registrable* value ) const
		{
			return value->getSubObjects();
		}



		TableOrObject::SubObjectsGetter::SubObjectsGetter( util::Env& env ):
			_env(env)
		{}



		synthese::SubObjects TableOrObject::getSubObjects(
			Env& env
		) const	{
			return apply_visitor(SubObjectsGetter(env), _value);
		}



		util::Registrable* TableOrObject::ObjectGetter::operator()( boost::shared_ptr<DBTableSync> value ) const
		{
			return NULL;
		}



		util::Registrable* TableOrObject::ObjectGetter::operator()( boost::shared_ptr<util::Registrable> value ) const
		{
			return value.get();
		}



		util::Registrable* TableOrObject::ObjectGetter::operator()( util::Registrable* value ) const
		{
			return value;
		}



		util::Registrable* TableOrObject::getObject() const
		{
			return apply_visitor(ObjectGetter(), _value);
		}
}	}
