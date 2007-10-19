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
		    
		    const std::string _serverHost;      //!< Server host.
		    const int _serverPort;              //!< Server port.
		    const int _timeOut;                 //!< TCP time out in milliseconds. 0 means no timeout.
		    
		public:
		    
		    BasicClient (const std::string& serverHost,
				 const int serverPort,
				 int timeOut = 0);

		    /**
		     * Sends a request to a server and writes received answer on an output stream.
		     * @param out : The output stream to write on.
		     * @param request : Request string to send to server (ex : fonction=rp&si=1&da=A)
		     * @param clientIp : Client IP (only relevant in a CGI context)
		     * @param clientURL : Client URL for link generation (only relevant in a CGI context)
		     */
		    void request (std::ostream& out, 
				  const std::string& request, 
				  const std::string& clientIp = "127.0.0.1", 
				  const std::string& clientUrl = "");

		    
		};
	}
}

#endif // SYNTHESE_SERVER_BASIC_CLIENT_h__
