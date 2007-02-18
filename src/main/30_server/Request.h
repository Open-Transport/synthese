
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
#include "01_util/Factorable.h"

#include <ostream>
#include <map>

namespace synthese
{
	namespace server
	{
		using util::Factory;

		class Site;
		class Session;
		class Action;

		/** Parsed request.
			@todo Remove the site pointer. Replace it by a Interface pointer.
		*/
		class Request : public util::Factorable
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
			static const std::string PARAMETER_ACTION_FAILED;
			static const std::string PARAMETER_ERROR_MESSAGE;
			static const std::string PARAMETER_ERROR_LEVEL;
			static const int MAX_REQUEST_SIZE;
			static const uid UID_WILL_BE_GENERATED_BY_THE_ACTION;
			
			typedef std::map<std::string, std::string> ParametersMap;
			typedef enum { REQUEST_ERROR_NONE, REQUEST_ERROR_INFO, REQUEST_ERROR_WARNING, REQUEST_ERROR_FATAL } ErrorLevel;

		protected:

			const Session*			_session;
			bool					_sessionBroken;
			std::string				_ip;
			Action*					_action;
			bool					_actionException;
			std::string				_errorMessage;
			ErrorLevel				_errorLevel;
			std::string				_clientURL;
			uid						_object_id;			//!< Object ID to display (generic parameter which can be used or not by the subclasses)

			/** Conversion from attributes to generic parameter maps.
			*/
			virtual ParametersMap getParametersMap() const = 0;

			/** Conversion from generic parameters map to attributes.
			*/
			virtual void setFromParametersMap(const ParametersMap& map) = 0;

			/** Method to run after action execution.
				@return true if the current execution must be stopped

				This method has to be overloaded.
				The default behaviour is to do nothing and continue the execution after the action run.
			*/
			virtual bool	runAfterAction(std::ostream& stream);

			/** Method to run before display when no session is detected.
				@return true if the current execution must be stopped

				This method has to be overloaded.
				The default behaviour is to do nothing and continue the execution without any session.
			*/
			virtual bool	runBeforeDisplayIfNoSession(std::ostream& stream);

			/** Normalize a query string.
				@param requestString request to normalize
			*/
			static std::string truncateStringIfNeeded (const std::string& requestString);

		public:
			//! \name Constructor and destructor
			//@{
				Request();
				virtual ~Request();
			//@}

			//! \name Getters
			//@{
				const Session*		getSession()		const;
				const std::string&	getClientURL()		const;
				const std::string&	getIP()				const;
				const Action*		getAction()			const;
				uid					getObjectId()		const;
				const std::string&	getErrorMessage()	const;
			//@}

			//! \name Setters
			//@{
				void setObjectId(uid id);
				void setAction(Action* action);
				void setSession(Session* session);
			//@}

			//! \name Modifiers
			//@{
				void deleteSession();
				virtual void copy(const Request* request);
			//@}

			//! \name Services
			//@{
				/** Query string getter for building links.
				*/
				virtual std::string getQueryString() const;

				/** Function to display, defined by each subclass.
				*/
				virtual void run(std::ostream& stream) const = 0;

				void runActionAndFunction(std::ostream& stream);

				std::string getHTMLLink(const std::string& content) const;
				virtual std::string getHTMLFormHeader(const std::string& name) const;
			//@}

			//! \name Static services
			//@{
				/** Instantiates a request from a text string, using the factory to choose the right subclass.
				@param text Text to parse.
				*/
				static Request* createFromString(const std::string& text);

				/** Parses a query string into a key => value map.
					@param text Text to parse
				*/
				static ParametersMap parseString(const std::string& text);
			//@}

		};
	}
}
#endif // SYNTHESE_Request_H__
