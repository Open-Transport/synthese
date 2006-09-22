#include "EnvironmentLSTest.h"

#include "16_env_ls_xml/EnvironmentLS.h"

#include "15_env/Environment.h"
#include "15_env/Axis.h"
#include "15_env/Calendar.h"
#include "15_env/Line.h"

#include "04_time/Hour.h"
#include "04_time/Schedule.h"


#include "01_util/XmlToolkit.h"

#include <iostream>


using synthese::time::Date;
using synthese::time::Hour;
using synthese::time::Schedule;

using synthese::env::Environment;


namespace synthese
{
namespace envlsxml
{


void 
EnvironmentLSTest::setUp () 
{

}


void 
EnvironmentLSTest::tearDown () 
{

} 





void 
EnvironmentLSTest::testLoadingEnvironment0 ()
{
    // Parse the environment xml file
    boost::filesystem::path xmlFile ("resources/environment0.xml", boost::filesystem::native);

    XMLNode environmentNode = 
	synthese::util::XmlToolkit::ParseFile (xmlFile, EnvironmentLS::ENVIRONMENT_TAG);

    Environment* env = EnvironmentLS::Load (environmentNode);

    delete env;


}



}
}

