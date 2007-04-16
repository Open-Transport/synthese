
/** Request class header.
	@file Request.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_Request_H__
#define SYNTHESE_Request_H__

#include "01_util/UId.h"

#include "12_security/Right.h"
#include "12_security/User.h"
#include "12_security/Profile.h"
#include "12_security/Constants.h"

#include "30_server/Types.h"
#include "30_server/Session.h"

#include <boost/shared_ptr.hpp>

#include <ostream>
#include <map>

namespace synthese
{
	namespace html
	{
		class HTMLForm;
	}

	namespace server
	{
		class Action;
		class Function;

		/** Parsed request.
			@ingroup m30

			A request object determinates a couple of services to run, and can be used to launch it or to describe it (build of links, etc.)

			The two services are :
				- an action  (execution without display, see server::Action)
				- a fonction (build of a display)

			The request class can be used in several ways. The life cycle of the request object is divided in two steps :
				- the build
				- the output
			
			The build and the output follow much different ways. The request acts as a wrapper between the build and the output methods.

			The available build processes are :
				- build by a query string parse (static method createFromString)
				- build by classic object construction
				- build by copy followed by completion with the classic object construction methods

			The available output methods are  :
				- execution of the action and/or the display from the specified parameters
				- build of a query string (method getQueryString)
				- build of an URL (method getURL)
				- build of an HTML form (method getHTMLForm, build of a class html::HTMLForm object)

			When the request is used to run the services, a session control is handled.

			The full sequence of the control is as following :
				- generic parsing of the string by the _parseString method
				- build of an empty Request by the Factory from the key read by the parsing process
				- build of an empty Action by the Factory from the key read by the parsing process
				- interpretation of the fields by the Action::_setFromParametersMap method
				- if a field is intended to be filled by the action run, then it is marked by the Action::_setFromParametersMap method
				- interpretation of the fields by the _setFromParametersMap method
				- if the action has to run before the session control (Action::beforeSessionControl method), run of the Action::run method
				- control of the session
				- interruption of the process if the  action needs a valid session (Action::_needsSession method)
				- if the action has to run after the session control (Action::beforeSessionControl method), run of the Action::run method
				- run of the _runAfterAction method
				- run of the _runBeforeDisplayIfNoSession method if no session is validated
				- interruption of the process if the function needs a valid session (return of the preceding method)
				- display with the _run method
				
		*/
		class Request
		{
		public:
			static const std::string PARAMETER_SEPARATOR;
			static const std::string PARAMETER_STARTER;
			static const std::string PARAMETER_ASSIGNMENT;
			static const std::string PARAMETER_FUNCTION;
			static const std::string PARAMETER_SESSION;
			static const std::string PARAMETER_IP;
			static const std::string PARAMETER_CLIENT_URL;
			static const std::string PARAMETER_OBJECT_ID;
			static const std::string PARAMETER_ACTION;
			static const std::string PARAMETER_ACTION_FAILED;
			static const std::string PARAMETER_ERROR_MESSAGE;
			static const std::string PARAMETER_ERROR_LEVEL;
			static const int MAX_REQUEST_SIZE;
			static const uid UID_WILL_BE_GENERATED_BY_THE_ACTION;
			
			typedef enum { REQUEST_ERROR_NONE, REQUEST_ERROR_INFO, REQUEST_ERROR_WARNING, REQUEST_ERROR_FATAL } ErrorLevel;

		private:

			boost::shared_ptr<Action>	_action;
			boost::shared_ptr<Function>	_function;
			const Session*				_session;
			bool						_sessionBroken;
			std::string					_ip;
			std::string					_clientURL;
			bool						_actionException;
			std::string					_errorMessage;
			ErrorLevel					_errorLevel;
			uid							_object_id;			//!< Object ID to display (generic parameter which can be used or not by the subclasses)

			//! \name Static internal services
			//@{
				/** Parses a query string into a key => value map.
					@param text Text to parse
				*/
				static ParametersMap _parseString(const std::string& text);

				/** Normalize a query string.
				@param requestString request to normalize
				*/
				static std::string _normalizeQueryString(const std::string& requestString);

				ParametersMap _getParametersMap() const;
			//@}

		protected:

			//! \name Protected getters
			//@{

				boost::shared_ptr<Function> _getFunction();
				boost::shared_ptr<const Function> _getFunction() const;

				/** Action getter.
					@return const Action* The action of the request
					@author Hugues Romain
					@date 2007					
				*/
				boost::shared_ptr<const Action> _getAction() const;
				boost::shared_ptr<Action> _getAction();
			//@}

			//! \name Protected setters
			//@{
				/** Action setter.
					@param action Action to set
					@author Hugues Romain
					@date 2007
				*/
			void _setAction(boost::shared_ptr<Action> action);

				/** Function setter.
					@param function
					@return void
					@author Hugues Romain
					@date 2007					
				*/
			void _setFunction(boost::shared_ptr<Function> function);
			//@}

				/** Construction of an empty request from an other one.
					@param request Request to copy (default/NULL = no copy)
					@param function Function to link with
					@param action Action to link with
					@author Hugues Romain
					@date 2007
					Use the public setters to fill the request.					
				*/
			Request(
				const Request* request=NULL
				, boost::shared_ptr<Function> function=boost::shared_ptr<Function>()
				, boost::shared_ptr<Action> action=boost::shared_ptr<Action>()
				);
				
		public:
			void _setErrorMessage(const std::string& message);
			void _setActionException(bool value);
			void _setErrorLevel(const ErrorLevel& level);
			//! \name Constructor and destructor
			//@{
				
				/** Construction from a query string to parse, using the factory to choose the right subclass.
					@param querystring The query string to parse
					@throw RequestException if the string is incomplete or contains refused values according to the parameters validators
					@author Hugues Romain
					@date 2007					
				*/
				Request(const std::string& querystring);
			//@}

			//! \name Getters
			//@{
				/** Action exception getter for subclasses only.
				@return bool If an action exception has occured
				@author Hugues Romain
				@date 2007

				*/
				bool getActionException() const;

				const std::string&	getClientURL()		const;
				const std::string&	getIP()				const;
				uid					getObjectId()		const;
				const std::string&	getErrorMessage()	const;
			//@}

			//! \name Setters
			//@{
				virtual void setObjectId(uid id);
				void setSession(Session* session);

				/** Client URL setter.
					@param url URL to store.
					@author Hugues Romain
					@date 2007
				*/
				void setClientURL(const std::string& url);
			//@}

			//! \name Modifiers
			//@{
				void deleteSession();

				/** Deletes the action contained in the request.
					@author Hugues Romain
					@date 2007					
				*/
				void deleteAction();
			//@}

			//! \name Service
			//@{
				/** Run of the services.
					@param stream Stream to write the output on
					@author Hugues Romain
					@date 2007
				*/
				void run(std::ostream& stream);

				template<class R>
				bool isAuthorized(security::Right::Level publicr = security::Right::FORBIDDEN, security::Right::Level privater = security::Right::FORBIDDEN, std::string parameter = security::UNKNOWN_PERIMETER) const;
			//@}

			//! \name Output methods
			//@{
				/** URL generator.
					@return std::string The URL corresponding to the request (= client URL + query string)
					@author Hugues Romain
					@date 2007
				*/
				std::string getURL()	const;

				/** HTML Form generator.
					@param name Name of the form (default = no name, sufficient for link generation)
					@return boost::shared_ptr<html::HTMLForm> The created form.
					@author Hugues Romain
					@date 2007					
				*/
				html::HTMLForm getHTMLForm(std::string name=std::string()) const;

				/** Query string getter for building links.
					@return The query string corresponding to the request.
				*/
				std::string getQueryString() const;
			//@}
		};

		template<class R>
		bool Request::isAuthorized(security::Right::Level publicr, security::Right::Level privater, std::string parameter /*= security::GLOBAL_PERIMETER*/ ) const
		{
			if (_session == NULL)
				return false;

			boost::shared_ptr<const security::Profile> profile = _session->getUser()->getProfile();
			boost::shared_ptr<security::Right> neededRight(util::Factory<security::Right>::create<R>());
			neededRight->setPublicLevel(publicr);
			neededRight->setPrivateLevel(privater);
			neededRight->setParameter(parameter);
			return profile->isAuthorized(neededRight);

		}
	}
}
#endif // SYNTHESE_Request_H__
