#ifndef SYNTHESE_SERVER_HTTPClient_hpp__
#define SYNTHESE_SERVER_HTTPClient_hpp__

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/optional.hpp>

#include <map>

namespace synthese
{
	namespace server
	{

		class HTTPClient
		{
		private:
			bool stopped_;
			boost::asio::ip::tcp::socket socket_;
			boost::asio::streambuf input_buffer_;
			boost::asio::deadline_timer deadline_;
			boost::system::error_code _errorCode;
			const std::string _serverHost;      //!< Server host.
			const std::string _serverPort;      //!< Server port.
			const std::string& _url;
			const std::string& _postData;
			const std::string _contentType;
			const bool _acceptGzip;

			bool _gotHeader;
			// Map of each http header received
			typedef std::map<std::string, std::string> Headers;
			Headers _headers;
			// The payload of the http response
			std::string &_payload;
			size_t _announcedContentLength;

			const int _connectionTimeout;
			const int _readTimeout;

			boost::asio::streambuf _request;

		public:
			HTTPClient(boost::asio::io_service& io_service,
				std::string serverHost,
				std::string serverPort,
				const std::string& url,
				const std::string& postData,
				const std::string& contentType,
				const bool acceptGzip,
				std::string &payload, /* Will contain the body or the http response */
				const boost::optional<int> connectionTimeout = boost::none,
				const boost::optional<int> readTimeout = boost::none
			);

			// Called by the user of the client class to initiate the connection process.
			// The endpoint iterator will have been obtained using a tcp::resolver.
			void start(boost::asio::ip::tcp::resolver::iterator endpoint_iter);

			// This function terminates all the actors to shut down the connection. It
			// may be called by the user of the client class, or by the class itself in
			// response to graceful termination or an unrecoverable error.
			void stop();

			// Returns the last error code
			boost::system::error_code getErrorCode();

		private:
			void startConnect(boost::asio::ip::tcp::resolver::iterator endpoint_iter);
			void handleConnect(const boost::system::error_code& ec,
							   boost::asio::ip::tcp::resolver::iterator endpoint_iter);
			void startRead();
			void handleRead(const boost::system::error_code& ec);
			void initRequest(boost::asio::streambuf &request);
			void startWrite();
			void handleWrite(const boost::system::error_code& ec);
			void checkDeadline();
			void payloadAppend();

		};
}	}

#endif // SYNTHESE_SERVER_HTTPClient_h__
