#ifndef SYNTHESE_SERVER_BASIC_CLIENT_h__
#define SYNTHESE_SERVER_BASIC_CLIENT_h__

#include <string>
#include <map>


namespace synthese
{

	namespace server
	{


		class BasicClient
		{
		private:
		    
		    const std::string _serverHost;
		    const int _serverPort;
		    const int _timeOut;  //!< TCP time out in milliseconds. 0 means no timeout.
		    
		public:
		    
		    BasicClient (const std::string& serverHost,
				 const int serverPort,
				 int timeOut = 0);

		    void request (std::ostream& out, std::string ip, std::string request, std::string clientUrl = "");

		    
		};
	}
}

#endif // SYNTHESE_SERVER_BASIC_CLIENT_h__
