#ifndef SYNTHESE_SERVER_BASIC_CLIENT_h__
#define SYNTHESE_SERVER_BASIC_CLIENT_h__

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/optional.hpp>

namespace synthese
{

	namespace server
	{

		//////////////////////////////////////////////////////////////////////////
		/// Simple HTTP client.
		/// @param m15
		class BasicClient:
			private boost::noncopyable
		{
		private:

		    const std::string _serverHost;      //!< Server host.
			const std::string _serverPort;		//!< Server port.
		    const boost::optional<int> _connectionTimeout; //!< TCP connection timeout in milliseconds
		    const boost::optional<int> _readTimeout;	//!< TCP read timeout in milliseconds
			const bool _outputHTTPHeaders;
			const bool _acceptGzip;

			std::string _send(
				const std::string& url,
				const std::string& postData,
				const std::string& contentType
			) const;

		public:

		    BasicClient (
				const std::string& serverHost,
				const std::string serverPort = "8080",
				int timeOut = 0,
				bool outputHTTPHeaders = false,
				bool acceptGzip = true
			);

		    BasicClient (
				const std::string& serverHost,
				const std::string serverPort,
				boost::optional<int> connectionTimeout,
				boost::optional<int> readTimeout,
				bool outputHTTPHeaders = false,
				bool acceptGzip = true
			);

		    /**
		     * Sends a request to a server and writes received answer on an output stream.
		     * @param out : The output stream to write on.
		     * @param request : Request string to send to server (ex : fonction=rp&si=1&da=A)
		     * @param clientIp : Client IP (only relevant in a CGI context)
		     * @param clientURL : Client URL for link generation (only relevant in a CGI context)
		     */
			std::string get(
				const std::string& url
			) const;


			std::string post(
				const std::string& url,
				const std::string& data,
				std::string contentType = std::string()
			) const;


			//////////////////////////////////////////////////////////////////////////
			/// Simple HTTP GET request helper.
			/// @warning Only HTTP is supported, not HTTPS
			/// @param url the url to get (format http://host:port/path)
			static std::string Get(
				const std::string url
			);
		};
}	}

#endif // SYNTHESE_SERVER_BASIC_CLIENT_h__
