#include "gps.h"

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

using boost::asio::ip::tcp;
using namespace std;

//#define RAW_DATA_SIMULATION_TEST 1

gps::gps():
  m_socket(m_ios),
  m_longitude(0.0),
  m_latitude(0.0),
  m_bEnabled(false)
{
}

gps::~gps()
{
	releaseSocket();
}

void gps::initSocket(std::string hostname/*"127.0.0.1"*/, int port/*2947*/) 
{
#ifdef RAW_DATA_SIMULATION_TEST
	return ;
#endif // RAWTEST

  	try
	{	
		releaseSocket();
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(hostname), port);
		m_socket.connect(endpoint);
	}
	catch (std::exception const& e)
	{
	    std::cerr << e.what() << std::endl;
	}
}

void gps::releaseSocket()
{
	m_socket.close();
}

// return true if the enabling as been send correctly to the GPS lib.
// false if we got any other error.
bool gps::enableTalk()
{
#ifdef RAW_DATA_SIMULATION_TEST
	m_bEnabled = true;
	return true;
#endif // RAWTEST

	m_bEnabled = false;
	try{
		// Set the request format
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << "?WATCH={\"enable\":true,\"json\":true}";
		if(boost::asio::write(m_socket, request)>0)
		{
			m_bEnabled = true;
		}
	}
	catch (std::exception const& e)
	{
	    m_bEnabled = false;
	    std::cerr << e.what() << std::endl;
	}	
	
	return m_bEnabled;
}

bool gps::updateFromGps()
{
	if(!m_bEnabled)
		return false;

	bool bSuccedded = false;
	try{
		//TODO: make it loops until we got new datas.
		//TODO: should we have a clean timeout and reconnect to the socket when we fail too much?
		//	But this should never fail as today we use a local port on local host.
		for(int i=0;i<100;i++){	// Theoriticaly should successed after two maximum 3 loops
			boost::asio::streambuf response;
#ifdef RAW_DATA_SIMULATION_TEST
			std::ostream os(&response);
			os << "{\"class\":\"TPV\",\"tag\":\"GLL\",\"device\":\"/dev/ttyS4\",\"mode\":3,\"time\":\"2013-08-22T12:27:03.000Z\",\"ept\":0.005,\"lat\":47.622858500,\"lon\":6.857120333,\"alt\":452.100,\"epv\":2299.770,\"track\":293.7200,\"speed\":0.000,\"climb\":0.000}" << std::endl;
#else
			boost::asio::read_until(m_socket, response, "\r\n");
#endif // RAWTEST
			if(readGSPJsonAnswers(response, m_longitude, m_latitude)){
			  bSuccedded = true;
				break;
			}
		}
	}
	catch (std::exception const& e)
	{
	    bSuccedded = false;
	    std::cerr << e.what() << std::endl;
	}	
	
	return bSuccedded;	
}

void gps::getLatLong(double &longitude, double &latitude)
{
	longitude = m_longitude;
	latitude = m_latitude;
}
 
// return true if the currently given JSON has valid GPS coordinates
// return false if the JSON was not formated correctly OR don't have the coordinates
bool gps::ExtractLongitudeAndLatitudeJson(std::string &str, double &longitude, double &latitude)
{
	bool readsucceded=false;
	//std::cout << "read JSON buffer" << std::endl;
  
	try
	{	
		boost::property_tree::ptree pt;
		istringstream iss(str);
		boost::property_tree::json_parser::read_json(iss,pt);
		
		string response(pt.get<string>("class"));
		if(response == "TPV")
		{
			try{
				longitude = boost::lexical_cast<double>(pt.get<string>("lon"));
				latitude = boost::lexical_cast<double>(pt.get<string>("lat"));
				readsucceded=true;
			} catch(const boost::bad_lexical_cast & ) {
				// cannot read the data. Com error? 
				readsucceded=false;
			}
		}
	}
	catch (std::exception const& e)
	{
	    std::cerr << e.what() << std::endl;
	    readsucceded=false;
	}
	
	return readsucceded;
	
}

// read a serie of JSON.
// we could get more than one JSON string out of the GPS. So we need to loop though all of them
bool gps::readGSPJsonAnswers(boost::asio::streambuf &ss, double &longitude, double &latitude)
{
	// warning, we could have mode than one JSON at the time.
	// but all separated by a new line.
	bool succedded=false;
	std::string token;
	std::istream str(&ss); 

	while(std::getline(str, token)) {
		succedded = ExtractLongitudeAndLatitudeJson(token, longitude, latitude);
	} 
	
	return succedded;
}

bool gps::tests()
{
	try
	{
		double longitude=0.0;
		double latitude=0.0;

		boost::asio::streambuf ss;
		std::ostream os(&ss);
		// for debug and test purpose.
		os << "{\"class\":\"VERSION\",\"release\":\"3.9\",\"rev\":\"3.9\",\"proto_major\":3,\"proto_minor\":8}" << std::endl;
		os << "{\"class\":\"DEVICES\",\"devices\":[{\"class\":\"DEVICE\",\"path\":\"/dev/ttyS4\",\"activated\":\"2013-08-22T14:25:18.627Z\",\"flags\":1,\"driver\":\"Generic NMEA\",\"native\":0,\"bps\":9600,\"parity\":\"N\",\"stopbits\":1,\"cycle\":1.00}]}" << std::endl;
		os << "{\"class\":\"WATCH\",\"enable\":true,\"json\":true,\"nmea\":false,\"raw\":0,\"scaled\":false,\"timing\":false}" << std::endl;
		os << "{\"class\":\"SKY\",\"tag\":\"GSV\",\"device\":\"/dev/ttyS4\",\"vdop\":99.99,\"hdop\":99.99,\"pdop\":99.99,\"satellites\":[{\"PRN\":5,\"el\":15,\"az\":57,\"ss\":41,\"used\":false},{\"PRN\":16,\"el\":20,\"az\":301,\"ss\":36,\"used\":false},{\"PRN\":23,\"el\":6,\"az\":319,\"ss\":0,\"used\":false},{\"PRN\":13,\"el\":2,\"az\":345,\"ss\":0,\"used\":false},{\"PRN\":29,\"el\":59,\"az\":59,\"ss\":0,\"used\":false},{\"PRN\":6,\"el\":9,\"az\":258,\"ss\":0,\"used\":false},{\"PRN\":31,\"el\":51,\"az\":228,\"ss\":0,\"used\":false},{\"PRN\":25,\"el\":34,\"az\":120,\"ss\":0,\"used\":false},{\"PRN\":21,\"el\":48,\"az\":175,\"ss\":0,\"used\":false},{\"PRN\":21,\"el\":48,\"az\":175,\"ss\":0,\"used\":false}]}" << std::endl;
		os << "{\"class\":\"TPV\",\"tag\":\"GLL\",\"device\":\"/dev/ttyS4\",\"mode\":3,\"time\":\"2013-08-22T12:27:03.000Z\",\"ept\":0.005,\"lat\":48.123123214,\"lon\":5.535245425,\"alt\":452.100,\"epv\":2299.770,\"track\":293.7200,\"speed\":0.000,\"climb\":0.000}" << std::endl;
		os << "{\"class\":\"SKY\",\"tag\":\"GSV\",\"device\":\"/dev/ttyS4\",\"vdop\":99.99,\"hdop\":99.99,\"pdop\":99.99,\"satellites\":[{\"PRN\":5,\"el\":15,\"az\":57,\"ss\":41,\"used\":false},{\"PRN\":16,\"el\":20,\"az\":301,\"ss\":36,\"used\":false},{\"PRN\":23,\"el\":6,\"az\":319,\"ss\":0,\"used\":false},{\"PRN\":13,\"el\":2,\"az\":345,\"ss\":0,\"used\":false},{\"PRN\":29,\"el\":59,\"az\":59,\"ss\":0,\"used\":false},{\"PRN\":6,\"el\":9,\"az\":258,\"ss\":0,\"used\":false},{\"PRN\":31,\"el\":51,\"az\":228,\"ss\":0,\"used\":false},{\"PRN\":25,\"el\":34,\"az\":120,\"ss\":0,\"used\":false},{\"PRN\":21,\"el\":48,\"az\":175,\"ss\":0,\"used\":false},{\"PRN\":21,\"el\":48,\"az\":175,\"ss\":0,\"used\":false}]}" << std::endl;	
		os << "{\"class\":\"TPV\",\"tag\":\"GLL\",\"device\":\"/dev/ttyS4\",\"mode\":3,\"time\":\"2013-08-22T12:27:03.000Z\",\"ept\":0.005,\"lat\":47.622858500,\"lon\":6.857120333,\"alt\":452.100,\"epv\":2299.770,\"track\":293.7200,\"speed\":0.000,\"climb\":0.000}" << std::endl;

		if(readGSPJsonAnswers(ss, m_longitude, m_latitude)){
			std::cout << "longitude=" << m_longitude <<std::endl;
			std::cout << "latidue=" << m_latitude <<std::endl;
		}
		
		if(m_longitude!=6.857120333){
			return EXIT_FAILURE;
		}
		
		if(m_latitude!=47.622858500){
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}
	catch (std::exception const& e)
	{
	    std::cerr << e.what() << std::endl;
	}
	
	return EXIT_FAILURE;
}
