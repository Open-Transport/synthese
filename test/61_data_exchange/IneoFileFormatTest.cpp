
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
	std::cout << "!!!!!!!!!!!!!!! BONJOUR THOMAS" << endl;
	std::cout << "TPU DEBUT DU TEST INEOFILEFORMAT" << endl;
	synthese::pt::moduleRegister();
	synthese::impex::moduleRegister();
	synthese::data_exchange::IneoFileFormat::integrate();

	boost::filesystem::path _dbPath = boost::filesystem::complete("test_db.db", boost::filesystem::initial_path());
	boost::filesystem::remove(_dbPath);
	string _connectionString = "sqlite://debug=1,path=" + _dbPath.string();	
	synthese::db::SQLiteDB::integrate();
	std::cout << "TPU SQLiteDB::integrate() done" << endl;

	synthese::db::DBModule::SetConnectionString(_connectionString );
	ModuleClassTemplate<synthese::db::DBModule>::PreInit();
	std::cout << "TPU PreInit() done" << endl;
	ModuleClassTemplate<synthese::db::DBModule>::Init();
	std::cout << "TPU Init() done" << endl;
	
	Env& env(Env::GetOfficialEnv());
	std::cout << "TPU Env set with OfficialEnv" << endl;

	shared_ptr<DataSource> ds(new DataSource(16607027920896001));
	env.getEditableRegistry<DataSource>().add(ds);

	shared_ptr<Import> import(new Import(1)); // TODO put real number
	import->set<DataSource>(*ds);
	std::cout << "TPU Datasource set to import" << endl;
	import->set<synthese::FileFormatKey>("Ineo");
	std::cout << "TPU Ineo set to import FileFormatKey" << endl;


	// STOP1
	shared_ptr<StopPoint> sp1(new StopPoint);
	std::cout << "TPU shared_ptr StopPoint 1 instanciated" << endl;
	
	// STOP2
	shared_ptr<StopPoint> sp2(new StopPoint);
	std::cout << "TPU shared_ptr StopPoint 2 instanciated" << endl;


	ParametersMap map;
	string ineoFilePattern(INEO_FILE_PATTERN);
	map.insert(IneoFileFormat::Importer_::FILE_PNT, ineoFilePattern + ".pnt");
	std::cout << "TPU FILE_PNT added to map" << endl;
	map.insert(IneoFileFormat::Importer_::FILE_DIS, ineoFilePattern + ".dis");
	std::cout << "TPU FILE_DIS added to map" << endl;
	map.insert(IneoFileFormat::Importer_::FILE_DST, ineoFilePattern + ".dst");
	std::cout << "TPU FILE_DST added to map" << endl;
	map.insert(IneoFileFormat::Importer_::FILE_LIG, ineoFilePattern + ".lig");
	std::cout << "TPU FILE_LIG added to map" << endl;
	map.insert(IneoFileFormat::Importer_::FILE_CJV, ineoFilePattern + ".cjv");
	std::cout << "TPU FILE_CJV added to map" << endl;
	map.insert(IneoFileFormat::Importer_::FILE_HOR, ineoFilePattern + ".hor");
	std::cout << "TPU FILE_HOR added to map" << endl;
	map.insert(IneoFileFormat::Importer_::FILE_CAL, ineoFilePattern + ".cal");
	std::cout << "TPU FILE_CAL added to map" << endl;

	stringstream logStream;
	ParametersMap pm;
	std::cout << "TPU Importer is gonna be instanciated" << endl;
	boost::shared_ptr<Importer> importer(
		import->getImporter(
			env,
			IMPORT_LOG_ALL,
			string(), 
			logStream,
			pm
	)	);
	importer->setFromParametersMap(map, true);

	std::cout << "TPU importer->setFromParametersMap done" << endl;

	bool doImport(
		importer->beforeParsing()
	);
	std::cout << "TPU importer->beforeParsing and doImport = " << doImport << endl;
	doImport &= importer->parseFiles();
	std::cout << "TPU parseFiles done and doImport = " << doImport << endl;
	doImport &= importer->afterParsing();
	std::cout << "TPU afterParsing done and doImport = " << doImport << endl;
	
	

}
#else
int main() {
	return 0;
}
#endif
