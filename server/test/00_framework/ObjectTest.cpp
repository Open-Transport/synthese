
/** Object Test implementation.
	@file ObjectTest.cpp

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

#include "Object.hpp"

#include "SchemaMacros.hpp"
#include "StringField.hpp"

#include <boost/test/auto_unit_test.hpp>

using namespace boost;
using namespace synthese;
using namespace synthese::util;

FIELD_DOUBLE(DoubleField)
template<> const Field SimpleObjectFieldDefinition<DoubleField>::FIELD = Field("double_field", SQL_DOUBLE, true);

typedef boost::fusion::map<
	FIELD(Key),
	FIELD(DoubleField)
> TestSchema;

class TestObject:
	public Object<TestObject, TestSchema>
{
public:
	typedef util::Registry<TestObject> Registry;

	TestObject(
		RegistryKeyType id = 0
	):	Registrable(id),
		Object<TestObject, TestSchema>(
			TestSchema(
				FIELD_VALUE_CONSTRUCTOR(Key, id),
				FIELD_VALUE_CONSTRUCTOR(DoubleField, 2.2)
		)	)
	{}
};

namespace synthese
{
	CLASS_DEFINITION(TestObject, "t980_test_object", 980)
}

BOOST_AUTO_TEST_CASE(SimpleObjectTest)
{
	TestObject obj;

	BOOST_CHECK_EQUAL(obj.getClassName(), "TestObject");
	BOOST_CHECK_EQUAL(obj.getClassNumber(), 980);
	BOOST_CHECK_EQUAL(obj.getTableName(), "t980_test_object");
	
	BOOST_CHECK_EQUAL(obj.get<Key>(), 0);
	BOOST_CHECK_EQUAL(obj.getKey(), 0);
	BOOST_CHECK_EQUAL(obj.get<DoubleField>(), 2.2);

	obj.set<Key>(4);
	BOOST_CHECK_EQUAL(obj.get<Key>(), 4);
	BOOST_CHECK_EQUAL(obj.getKey(), 4);
	BOOST_CHECK_EQUAL(obj.get<DoubleField>(), 2.2);

	obj.set<DoubleField>(5.5);
	BOOST_CHECK_EQUAL(obj.get<Key>(), 4);
	BOOST_CHECK_EQUAL(obj.getKey(), 4);
	BOOST_CHECK_EQUAL(obj.get<DoubleField>(), 5.5);

	ObjectBase& objb(static_cast<ObjectBase&>(obj));
	BOOST_CHECK_EQUAL(objb.getClassName(), "TestObject");
	BOOST_CHECK_EQUAL(objb.getClassNumber(), 980);
	BOOST_CHECK_EQUAL(objb.getTableName(), "t980_test_object");
	BOOST_CHECK_EQUAL(objb.getKey(), 4);

	BOOST_CHECK_EQUAL(objb.hasField<Key>(), true);
	BOOST_CHECK_EQUAL(objb.hasField<Name>(), false);
	BOOST_CHECK_EQUAL(objb.hasField<DoubleField>(), true);

	BOOST_CHECK_EQUAL(objb.dynamic_get<Key>(), 4);
	BOOST_CHECK_EQUAL(objb.dynamic_get<DoubleField>(), 5.5);

	objb.dynamic_set<Key>(8);
	
	BOOST_CHECK_EQUAL(obj.get<Key>(), 8);
	BOOST_CHECK_EQUAL(obj.get<DoubleField>(), 5.5);

	objb.dynamic_set<DoubleField>(9.2);

	BOOST_CHECK_EQUAL(obj.get<Key>(), 8);
	BOOST_CHECK_EQUAL(obj.get<DoubleField>(), 9.2);

	{
		ParametersMap pm;
		obj.toParametersMap(pm);

		BOOST_CHECK_EQUAL(pm.getMap().size(), 2);
		BOOST_CHECK_EQUAL(pm.get<RegistryKeyType>("id"), 8);
		BOOST_CHECK_EQUAL(pm.get<double>("double_field"), 9.2);
	}

	{
		ParametersMap pm;
		obj.toParametersMap(pm, false, true);

		BOOST_CHECK_EQUAL(pm.getMap().size(), 1);
		BOOST_CHECK_EQUAL(pm.get<double>("double_field"), 9.2);
	}

	{
		ParametersMap pm;
		obj.toParametersMap(pm, false, false);

		BOOST_CHECK_EQUAL(pm.getMap().size(), 1);
		BOOST_CHECK_EQUAL(pm.get<RegistryKeyType>("id"), 8);
	}

	{
		FilesMap fm;
		obj.toFilesMap(fm);

		BOOST_CHECK_EQUAL(fm.getMap().size(), 1);
		const FilesMap::File& file(fm.get("double_field"));
		BOOST_CHECK_EQUAL(lexical_cast<double>(file.content), 9.2);
		BOOST_CHECK_EQUAL(std::string(file.mimeType), "text/plain");
	}

	{
		ParametersMap pm;
		Env env;
		pm.insert("id", 87);
		obj.loadFromRecord(pm, env);
		BOOST_CHECK_EQUAL(obj.get<Key>(), 87);
		BOOST_CHECK_EQUAL(obj.get<DoubleField>(), 9.2);
	}
}

