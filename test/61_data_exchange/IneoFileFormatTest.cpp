
/** IneoFileFormatTest class implementation.
	@file PegaseFileFormatTest.cpp
	@author Sylvain Pasche

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

// scons workaround
#ifdef INEO_FILE_PATTERN

#include "Import.hpp"
#include "IneoFileFormat.hpp"
#include "PTModuleRegister.cpp"
#include "ImpExModuleRegister.cpp"
#include "Request.h"
#include "DBModule.h"
#include "101_sqlite/SQLiteDB.h"
#include "UtilTypes.h"

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::admin;
using namespace synthese::data_exchange;
using namespace synthese::impex;
using namespace synthese::pt;
using namespace synthese::server;
using namespace synthese::util;

using namespace boost;
using namespace std;

using synthese::Exception;

BOOST_AUTO_TEST_CASE (testIneoFileFormat)
{
	synthese::pt::moduleRegister();
	synthese::impex::moduleRegister();
	synthese::data_exchange::IneoFileFormat::integrate();

	boost::filesystem::path _dbPath = boost::filesystem::complete("test_db.db", boost::filesystem::initial_path());
	boost::filesystem::remove(_dbPath);
	string _connectionString = "sqlite://debug=1,path=" + _dbPath.string();
	synthese::db::SQLiteDB::integrate();
	synthese::db::DBModule::SetConnectionString(_connectionString );
	ModuleClassTemplate<synthese::db::DBModule>::PreInit();
	ModuleClassTemplate<synthese::db::DBModule>::Init();
	
	Env& env(Env::GetOfficialEnv());

	shared_ptr<DataSource> ds(new DataSource(16607027920896001));
	env.getEditableRegistry<DataSource>().add(ds);

	shared_ptr<Import> import(new Import(1)); // TODO put real number
	import->set<DataSource>(*ds);
	import->set<synthese::FileFormatKey>("Ineo");


	// STOP1
	shared_ptr<StopPoint> sp1(new StopPoint);
	
	// STOP2
	shared_ptr<StopPoint> sp2(new StopPoint);


	ParametersMap map;
	string ineoFilePattern(INEO_FILE_PATTERN);
	map.insert(IneoFileFormat::Importer_::FILE_PNT, ineoFilePattern + ".pnt");
	map.insert(IneoFileFormat::Importer_::FILE_DIS, ineoFilePattern + ".dis");
	map.insert(IneoFileFormat::Importer_::FILE_DST, ineoFilePattern + ".dst");
	map.insert(IneoFileFormat::Importer_::FILE_LIG, ineoFilePattern + ".lig");
	map.insert(IneoFileFormat::Importer_::FILE_CJV, ineoFilePattern + ".cjv");
	map.insert(IneoFileFormat::Importer_::FILE_HOR, ineoFilePattern + ".hor");
	map.insert(IneoFileFormat::Importer_::FILE_CAL, ineoFilePattern + ".cal");

	stringstream logStream;
	ParametersMap pm;
	boost::shared_ptr<Importer> importer(
		import->getImporter(
			env,
			IMPORT_LOG_ALL,
			string(), 
			logStream,
			pm
	)	);
	importer->setFromParametersMap(map, true);

	bool doImport(
		importer->beforeParsing()
	);
	doImport &= importer->parseFiles();
	doImport &= importer->afterParsing();
	
	

}
#else
int main() {
	return 0;
}
#endif
