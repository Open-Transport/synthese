
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <string>

#include "01_util/FactoryException.h"
#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include "30_server/ServerModule.h"
#include "30_server/RequestException.h"
#include "30_server/Session.h"
#include "30_server/Request.h"
#include "30_server/SessionException.h"

using std::string;

namespace synthese
{
	using namespace util;

	namespace server
	{

		const std::string Request::PARAMETER_SEPARATOR ("&");
		const std::string Request::PARAMETER_ASSIGNMENT ("=");
		const int Request::MAX_REQUEST_SIZE (4096);
		const std::string Request::PARAMETER_FUNCTION = "fonction";
		const std::string Request::PARAMETER_SITE = "site";
		const std::string Request::PARAMETER_SESSION = "sid";
		const std::string Request::PARAMETER_IP = "ipaddr";

		std::string Request::truncateStringIfNeeded (const std::string& requestString)
		{
			std::string s (requestString);

			// The + characters are added by the web browsers instead of spaces
			boost::algorithm::replace_all (s, "+", " ");

			// Deletes the end of line code
			size_t pos = s.find ("\r");
			if (pos != string::npos)
				s = s.substr(0, pos);
			pos = s.find ("\n");
			if (pos != string::npos)
				s = s.substr(0, pos);


			/* ?? what do we do with this code ?
			if (s.size () > MAX_REQUEST_SIZE) {
			bool parameterTruncated = (s.substr (MAX_REQUEST_SIZE, 1) != PARAMETER_SEPARATOR);
			s = s.substr (0, MAX_REQUEST_SIZE);

			// Filter last parameter which if it has been truncated
			if (parameterTruncated) 
			{
			s = s.substr (0, s.rfind (PARAMETER_SEPARATOR));
			}
			}
			*/
			return s;
		}

		Request* Request::createFromString( const Site::Registry& siteRegistry, const std::string& text )
		{
			std::string s (truncateStringIfNeeded (text));
			ParametersMap map;

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep(PARAMETER_SEPARATOR.c_str ());

			// Parsing
			tokenizer parametersTokens (s, sep);
			for (tokenizer::iterator parameterToken = parametersTokens.begin();
				parameterToken != parametersTokens.end (); ++ parameterToken)
			{
				size_t pos = parameterToken->find (PARAMETER_ASSIGNMENT);
				if (pos == string::npos) continue;

				std::string parameterName (parameterToken->substr (0, pos));
				std::string parameterValue (parameterToken->substr (pos+1));

				map.insert (make_pair (parameterName, parameterValue));
			}

			// Function name
			ParametersMap::iterator it = map.find(PARAMETER_FUNCTION);
			if (it == map.end())
				throw RequestException("Function not specified");
			if (!Factory<Request>::contains(it->second))
				throw RequestException("Function not found");

			// Request instantiation
			Request* request;
			try
			{
				request = Factory<Request>::create(it->second);
			}
			catch (FactoryException<Request> e)
			{
				throw RequestException(e.getMessage());
			}
			map.erase(it);
			
			// Used site
			it = map.find(PARAMETER_SITE);
			if (it == map.end())
				throw RequestException("Site not specified");
			try
			{
				request->_site = siteRegistry.get(Conversion::ToLongLong(it->second));
			}
			catch (Site::RegistryKeyException e)
			{
				throw RequestException("Site not found");
			}
			// Site validity control
			if (!request->_site->dateControl())
				throw RequestException("Site is deactivated");
			map.erase(it);

			// IP
			it = map.find(PARAMETER_IP);
			if (it == map.end())
			{
				util::Log::GetInstance().warn("Query without IP : a bad client is attempting to connect, or there was an attack.");
				throw RequestException("Client IP not found in parameters.");
			}
			request->_ip = it->second;

			// Session
			it = map.find(PARAMETER_SESSION);
			if (it == map.end())
			{
				request->_session = NULL;
				request->_sessionBroken = false;
			}
			else
			{
				ServerModule::SessionMap::iterator sit = ServerModule::getSessions().find(it->second);
				if (sit == ServerModule::getSessions().end())
				{
					request->_session = NULL;
					request->_sessionBroken = true;
				}
				else
				{
					try
					{
						sit->second->controlAndRefresh(request->_ip);
						request->_session = sit->second;
						request->_sessionBroken = false;
					}
					catch (SessionException e)
					{
						delete sit->second;
						ServerModule::getSessions().erase(sit);
						request->_session = NULL;
						request->_sessionBroken = true;
					}
				}
				map.erase(it);
			}

			request->setFromParametersMap(map);

			return request;
		}

		std::string Request::getQueryString() const
		{
			// Adding function name
			ParametersMap map = getParametersMap();
			map.insert(make_pair(PARAMETER_FUNCTION, getFactoryKey()));
			map.insert(make_pair(PARAMETER_SITE, Conversion::ToString(_site->getId())));

			// Serialize the parameter lists in a synthese querystring
			std::stringstream ss;

			for (ParametersMap::const_iterator iter = map.begin(); 
				iter != map.end(); 
				++iter )
			{
				if (iter != map.begin ()) ss << PARAMETER_SEPARATOR;
				ss << iter->first << PARAMETER_ASSIGNMENT << iter->second;
			}

			return truncateStringIfNeeded(ss.str());

		}
	}
}