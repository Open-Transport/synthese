#ifndef GPS_H
#define GPS_H

#include <boost/asio.hpp>

class gps
{
public:
  gps();
  ~gps();
  
  bool tests();
  
  void initSocket(std::string hostname, int port);
  bool enableTalk();
  bool updateFromGps();
  void getLatLong(double &longitude, double &latitude);
  void releaseSocket();
  
private:
  
  bool ExtractLongitudeAndLatitudeJson(std::string &str, double &longitude, double &latitude);
  bool readGSPJsonAnswers(boost::asio::streambuf &ss, double &longitude, double &latitude);

  double m_longitude;
  double m_latitude;
  bool m_bEnabled;
  boost::asio::io_service m_ios;  
  boost::asio::ip::tcp::socket m_socket;
};

#endif // GPS_H
