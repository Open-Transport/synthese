
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

#include "IneoFileFormat.hpp"
#include "PTModuleRegister.cpp"
#include "ImpExModuleRegister.cpp"
#include "Request.h"

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

	Env env;

	shared_ptr<DataSource> ds(new DataSource(16607027920896001));
	env.getEditableRegistry<DataSource>().add(ds);
	
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

	boost::shared_ptr<Importer> importer(ds->getImporter(env));
	importer->setFromParametersMap(map, true);

	stringstream output;
	bool doImport(
		importer->beforeParsing()
	);
	doImport &= importer->parseFiles(output, boost::optional<const Request&>());
	importer->getLogger().output(output);
	doImport &= importer->afterParsing();
	
	

}
#else
int main() {
	return 0;
}
#endif
