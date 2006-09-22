#include "EnvironmentLSTest.h"

#include "16_env_ls_xml/EnvironmentLS.h"

#include "15_env/Environment.h"
#include "15_env/Axis.h"
#include "15_env/City.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/Calendar.h"
#include "15_env/Line.h"
#include "15_env/TransportNetwork.h"

#include "04_time/Hour.h"
#include "04_time/Schedule.h"


#include "01_util/XmlToolkit.h"

#include <iostream>


using synthese::time::Date;
using synthese::time::Hour;
using synthese::time::Schedule;

using namespace synthese::env;
using namespace synthese::time;


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
    
    // cities
    CPPUNIT_ASSERT_EQUAL (2, (int) env->getCities ().size ());
    CPPUNIT_ASSERT (env->getCities ().contains (6000));
    CPPUNIT_ASSERT (env->getCities ().contains (6001));
    
    const City* city6000 = env->getCities ().get (6000);
    CPPUNIT_ASSERT_EQUAL (std::string ("Toulouse"), city6000->getName ());
    CPPUNIT_ASSERT_EQUAL (10, (int) city6000->getConnectionPlacesMatcher ().size ());
    CPPUNIT_ASSERT_EQUAL (1, (int) city6000->getPublicPlacesMatcher ().size ());
    CPPUNIT_ASSERT_EQUAL (1, (int) city6000->getPlaceAliasesMatcher ().size ());
    CPPUNIT_ASSERT_EQUAL (2, (int) city6000->getRoadsMatcher ().size ());
    CPPUNIT_ASSERT_EQUAL (2, (int) city6000->getIncludedPlaces ().size ());



    // transport networks
    CPPUNIT_ASSERT_EQUAL (1, (int) env->getTransportNetworks ().size ());
    CPPUNIT_ASSERT (env->getTransportNetworks ().contains (2200));

    const TransportNetwork* transportNetwork2200 = env->getTransportNetworks ().get (2200);
    CPPUNIT_ASSERT_EQUAL (std::string ("tisseo"), transportNetwork2200->getName ());



    // axes
    CPPUNIT_ASSERT_EQUAL (2, (int) env->getAxes ().size ());
    CPPUNIT_ASSERT (env->getAxes ().contains (4000));
    CPPUNIT_ASSERT (env->getAxes ().contains (4001));
    
    const Axis* axis4000 = env->getAxes ().get (4000);
    CPPUNIT_ASSERT_EQUAL (std::string ("A1"), axis4000->getName ());
    CPPUNIT_ASSERT_EQUAL (true, axis4000->isFree ());
    CPPUNIT_ASSERT_EQUAL (false, axis4000->isAllowed ());
    


    // connectionPlaces
    CPPUNIT_ASSERT_EQUAL (11, (int) env->getConnectionPlaces ().size ());
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7000));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7001));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7002));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7003));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7004));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7005));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7006));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7007));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7008));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7009));
    CPPUNIT_ASSERT (env->getConnectionPlaces ().contains (7030));
    
    const ConnectionPlace* connectionPlace7004 = env->getConnectionPlaces ().get (7004);
    CPPUNIT_ASSERT_EQUAL (std::string ("CP5"), connectionPlace7004->getName ());
    CPPUNIT_ASSERT_EQUAL (city6000, connectionPlace7004->getCity ());
    CPPUNIT_ASSERT_EQUAL (ConnectionPlace::CONNECTION_TYPE_LINELINE, 
			  connectionPlace7004->getConnectionType ());
    CPPUNIT_ASSERT_EQUAL (7, connectionPlace7004->getTransferDelay (
			      env->fetchVertex (12004), 
			      env->fetchVertex (12008)) );    



    // publicPlaces
    



    delete env;


}



}
}

