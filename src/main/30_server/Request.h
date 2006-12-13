
#ifndef SYNTHESE_Request_H__
#define SYNTHESE_Request_H__


#include "01_util/Factorable.h"
#include "Site.h"
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
		*/
		class Request : public util::Factorable
		{
		public:
			static const std::string PARAMETER_SEPARATOR;
			static const std::string PARAMETER_STARTER;
			static const std::string PARAMETER_ASSIGNMENT;
			static const std::string PARAMETER_FUNCTION;
			static const std::string PARAMETER_SITE;
			static const std::string PARAMETER_SESSION;
			static const std::string PARAMETER_IP;
			static const std::string PARAMETER_CLIENT_URL;
			static const int MAX_REQUEST_SIZE;
			
			typedef std::map<std::string, std::string> ParametersMap;
			typedef enum { NEEDS_SESSION, DONT_NEED_SESSION } IsSessionNeeded;

		protected:

			const Site*				_site;
			const Session*			_session;
			bool					_sessionBroken;
			std::string				_ip;
			Action*					_action;
			bool					_actionException;
			std::string				_clientURL;
			const IsSessionNeeded	_needsSession;
			uid						_object_id;			//!< Object ID to display (generic parameter which can be used or not by the subclasses)

			/** Conversion from attributes to generic parameter maps.
			*/
			virtual ParametersMap getParametersMap() const = 0;

			/** Conversion from generic parameters map to attributes.
			*/
			virtual void setFromParametersMap(const ParametersMap& map) = 0;

			/** Normalize a query string.
				@param requestString request to normalize
			*/
			static std::string truncateStringIfNeeded (const std::string& requestString);

		public:
			//! \name Constructor and destructor
			//@{
				Request(IsSessionNeeded needsSession);
				virtual ~Request();
			//@}

			//! \name Getters
			//@{
				const Site*			getSite()		const;
				const Session*		getSession()	const;
				const std::string&	getClientURL()	const;
				const std::string&	getIP()			const;
				const Action*		getAction()		const;
				uid					getObjectId()	const;
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
				void copy(const Request* request);
			//@}

			//! \name Services
			//@{
				/** Query string getter for building links.
				*/
				std::string getQueryString() const;

				/** Function to display, defined by each subclass.
				*/
				virtual void run(std::ostream& stream) const = 0;

				void runActionAndFunction(std::ostream& stream);

				std::string getHTMLLink(const std::string& content) const;
				std::string getHTMLFormHeader(const std::string& name) const;
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
