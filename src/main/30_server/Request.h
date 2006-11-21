
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

		protected:

			const Site*		_site;
			const Session*	_session;
			bool			_sessionBroken;
			std::string		_ip;
			Action*			_action;
			std::string		_clientURL;

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
			//! \name Constructors and destructors
			//@{
				virtual ~Request();
			//@}

			//! \name Getters
			//@{
				const Site* getSite() const;
				const Session* getSession() const;
				const std::string& getClientURL() const;
				const std::string& getIP() const;
			//@}

			//! \name Setters
			//@{
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
			//@}

			//! \name Static services
			//@{
				/** Instantiates a request from a text string, using the factory to choose the right subclass.
				@param text Text to parse.
				*/
				static Request* createFromString(const Site::Registry& siteRegistry, const std::string& text);
			//@}

		};
	}
}
#endif // SYNTHESE_Request_H__