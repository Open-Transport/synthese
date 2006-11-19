
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

		/** Parsed request.
		*/
		class Request : public util::Factorable
		{
		public:
			static const std::string PARAMETER_SEPARATOR;
			static const std::string PARAMETER_ASSIGNMENT;
			static const std::string PARAMETER_FUNCTION;
			static const std::string PARAMETER_SITE;
			static const std::string PARAMETER_SESSION;
			static const std::string PARAMETER_IP;
			static const int MAX_REQUEST_SIZE;

		protected:
			typedef std::map<std::string, std::string> ParametersMap;

			const Site* _site;
			const Session* _session;
			bool _sessionBroken;
			std::string _ip;

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
			/** Instantiates a request from a text string, using the factory to choose the right subclass.
			@param text Text to parse.
			*/
			static Request* createFromString(const Site::Registry& siteRegistry, const std::string& text);

			/** Action to run, defined by each subclass.
			*/
			virtual void run(std::ostream& stream) const = 0;

			/** Query string getter for building links.
			*/
			std::string getQueryString() const;

			const Site* getSite() const;
			const Session* getSession() const;
		};
	}
}
#endif // SYNTHESE_Request_H__