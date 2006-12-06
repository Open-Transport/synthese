
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <string>

#include "01_util/Exception.h"
#include "01_util/FactoryException.h"
#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include "11_interfaces/Interface.h"

#include "30_server/ActionException.h"
#include "30_server/ServerModule.h"
#include "30_server/Session.h"
#include "30_server/Site.h"
#include "30_server/SessionException.h"
#include "30_server/Action.h"
#include "30_server/RedirectInterfacePage.h"
#include "30_server/RequestException.h"
#include "30_server/Request.h"

using std::string;

namespace synthese
{
	using namespace util;

	namespace server
	{

		const std::string Request::PARAMETER_SEPARATOR ("&");
		const std::string Request::PARAMETER_STARTER ("?");
		const std::string Request::PARAMETER_ASSIGNMENT ("=");
		const int Request::MAX_REQUEST_SIZE (4096);
		const std::string Request::PARAMETER_FUNCTION = "fonction";
		const std::string Request::PARAMETER_SITE = "site";
		const std::string Request::PARAMETER_SESSION = "sid";
		const std::string Request::PARAMETER_IP = "ipaddr";
		const std::string Request::PARAMETER_CLIENT_URL = "clienturl";

		Request::Request(IsSessionNeeded isSessionNeeded)
			: _session(NULL)
			, _action(NULL)
			, _needsSession(isSessionNeeded)
		{

		}

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

		Request* Request::createFromString(const std::string& text )
		{
			std::string s (truncateStringIfNeeded (text));
			ParametersMap map = parseString(s);

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
				request->_site = ServerModule::getSites().get(Conversion::ToLongLong(it->second));
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
			map.erase(it);

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
						request->deleteSession();
						request->_sessionBroken = true;
					}
				}
			}

			// Client URL
			it = map.find(PARAMETER_CLIENT_URL);
			if (it != map.end())
			{
				request->_clientURL = it->second;
				map.erase(it);
			}

			try
			{
				// Action
				request->_action = Action::create(request, map);
			}
			catch (ActionException& e)
			{
				throw RequestException("Action error : "+ e.getMessage());
			}
			request->setFromParametersMap(map);

			return request;
		}

		std::string Request::getQueryString() const
		{
			// Adding function name
			ParametersMap map = getParametersMap();
			map.insert(make_pair(PARAMETER_FUNCTION, getFactoryKey()));
			map.insert(make_pair(PARAMETER_SITE, Conversion::ToString(_site->getKey())));
			if (_action != NULL)
			{
				map.insert(make_pair(Action::PARAMETER_ACTION, _action->getFactoryKey()));
				ParametersMap actionMap = _action->getParametersMap();
				for (ParametersMap::const_iterator it = actionMap.begin(); it != actionMap.end(); ++it)
				{
					map.insert(make_pair(Action::PARAMETER_PREFIX + it->first, it->second));
				}
			}
			if (_session != NULL)
			{
				map.insert(make_pair(PARAMETER_SESSION, _session->getKey()));
			}

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

		void Request::runActionAndFunction( std::ostream& stream )
		{
			if (_action != NULL)
			{
				try
				{
					_action->run();

					// Redirection to the same request without the action
					_action = NULL;
					const RedirectInterfacePage* page = _site->getInterface()->getPage<RedirectInterfacePage>();
					page->display(stream, this);
				}
				catch (ActionException e)
				{
					/** @todo Create a ActionException factory, a value interface element called 
					isactionerror actionname
					When an action returns an exception, it must be a pointer to a registered subclass of 
					ActionException. The isactionerror will compare the key of the exception and the actionname
					if the coparison is ok true is returned. This statement can be included in a print or a goto
					command as the if one.
					**/
					_actionException = true;
				}
			}
			else
			{
				if (_needsSession == NEEDS_SESSION && _session == NULL)
					return;
				run(stream);
			}
		}

		Request::~Request()
		{
			delete _action;
		}

		void Request::setAction( Action* action )
		{
			_action = action;
		}

		void Request::deleteSession()
		{
			delete _session;
			_session = NULL;
		}

		void Request::copy( const Request* request )
		{
			_site = request->_site;
			_clientURL = request->_clientURL;
			_session = request->_session;
		}

		std::string Request::getHTMLLink(const std::string& content) const
		{
			std::stringstream str;
			str << "<a href=\"" << _clientURL << PARAMETER_STARTER << getQueryString() << "\">"
				<< content << "</a>";
			return str.str();
		}

		const std::string& Request::getClientURL() const
		{
			return _clientURL;
		}

		const std::string& Request::getIP() const
		{
			return _ip;
		}

		void Request::setSession( Session* session )
		{
			_session = session;
		}

		Request::ParametersMap Request::parseString( const std::string& text )
		{
			ParametersMap map;
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep(PARAMETER_SEPARATOR.c_str ());

			// Parsing
			tokenizer parametersTokens (text, sep);
			for (tokenizer::iterator parameterToken = parametersTokens.begin();
				parameterToken != parametersTokens.end (); ++ parameterToken)
			{
				size_t pos = parameterToken->find (PARAMETER_ASSIGNMENT);
				if (pos == string::npos) continue;

				std::string parameterName (parameterToken->substr (0, pos));
				std::string parameterValue (parameterToken->substr (pos+1));

				map.insert (make_pair (parameterName, parameterValue));
			}
			return map;
		}

		std::string Request::getHTMLFormHeader(const std::string& name) const
		{
			std::stringstream str;
			str	<< "<form name=\"" << name << "\" action=\"" << _clientURL << "\" method=\"post\">"
				<< "<input type=\"hidden\" name=\"" << PARAMETER_FUNCTION << "\" value=\"" << getFactoryKey() << "\" />"
				<< "<input type=\"hidden\" name=\"" << PARAMETER_SITE << "\" value=\"" << _site->getKey() << "\" />";
			if (_session != NULL)
				str << "<input type=\"hidden\" name=\"" << PARAMETER_SESSION << "\" value=\"" << _session->getKey() << "\" />";
			if (_action != NULL)
			{
				str << "<input type=\"hidden\" name=\"" << Action::PARAMETER_ACTION << "\" value=\"" << _action->getFactoryKey() << "\" />";
			}
			return str.str();
		}

		const Site* Request::getSite() const
		{
			return _site;
		}

		const Action* Request::getAction() const
		{
			return _action;
		}
	}
}