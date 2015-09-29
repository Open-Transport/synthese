
/** Webpage class header.
	@file Webpage.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_cms_WebPage_h__
#define SYNTHESE_cms_WebPage_h__

#include "Object.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "TreeNode.hpp"
#include "TreeNodeField.hpp"
#include "TreeRankOrderingPolicy.hpp"
#include "TreeOtherClassRootPolicy.hpp"

#include "MinutesField.hpp"
#include "NumericField.hpp"
#include "PointerField.hpp"
#include "PtimeField.hpp"
#include "SchemaMacros.hpp"
#include "StringField.hpp"
#include "PointersVectorField.hpp"
#include "WebpageContent.hpp"
#include "Website.hpp"
#include "Function.h"
#include "Factory.h"
#include "shared_recursive_mutex.hpp"

#include <ostream>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace server
	{
		class Request;
	}

	namespace cms
	{
		class Webpage;

		FIELD_TREE_NODE(WebpageTreeNode, Webpage)
		FIELD_STRING(Abstract)
		FIELD_STRING(ImageURL)
		FIELD_POINTERS_VECTOR(WebpageLinks, Webpage)
		FIELD_BOOL(DoNotUseTemplate)
		FIELD_BOOL(HasForum)
		FIELD_STRING(SmartURLPath)
		FIELD_STRING(SmartURLDefaultParameterName)
		FIELD_BOOL(RawEditor)
		FIELD_POINTER(SpecificTemplate, Webpage)
		FIELD_MINUTES(MaxAge)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(WebpageTreeNode),
			FIELD(Title),
			FIELD(WebpageContent),
			FIELD(StartTime),
			FIELD(EndTime),
			FIELD(MaxAge),
			FIELD(Abstract),
			FIELD(ImageURL),
			FIELD(WebpageLinks),
			FIELD(DoNotUseTemplate),
			FIELD(HasForum),
			FIELD(SmartURLPath),
			FIELD(SmartURLDefaultParameterName),
			FIELD(RawEditor),
			FIELD(SpecificTemplate)
		> WebpageRecord;


		/** Web page class (CMS).
			@ingroup m36

			A web page is defined by a content, which can include recursively calls to public functions.

			A call to a public function is done by using a special markup around a synthese query string :

			@code <?fonction=...&param=...?> @endcode

			The query string can contain spaces and other special characters according to the function loader.

			In order to get the code cleaner, it is possible to insert carriage return inside a query string : all of then will be ignored, as defined in the HTML specifications. To generate a carriage return, insert \n in the content.

			The query string can include recursive calls to other query strings : a parameter of a function can be the result of an other function, without depth limit.

			The carriage return, the \ character, and the <? and ?> sequence are reserved words. It is possible to display it by using special commands :
			<ul>
				<li>\n = carriage return</li>
				<li>\\ = \</li>
				<li>\<? = <?</li>
				<li>\?> ? ?></li>
			</ul>

			<h3>Smart URLS</h3>

			A webpage can be found by a real URL instead of its id, in order to raise up the score of the page
			in the search engines.

			The full smart URL for displaying a page follows this pattern :
			<pre>http://<host><page smart path>[/<main parameter value>][&<other parameters>]</pre>

			To transfert the smart URL into a real one, a Apache Rewrite Rule is necessary. After the rule, the
			URL is transformed into :
			<pre>http://<host>/<site client url>?fonction=page&smart_url=<page smart path>[/<main parameter value>][&<other parameters>]</pre>

			The corresponding normal URL is :
			<pre>http://<host>/<site client url>?<site client url>?fonction=page&p=<page id>[&<main parameter name>=<main parameter value>][&<other parameters>]</pre>

			The Rewrite Rule to insert in the Apache configuration file :
			<pre>
			<Directory /var/www/example.com>
				RewriteEngine on
				RewriteBase /
				RewriteCond %{REQUEST_FILENAME} !-f
				RewriteCond %{REQUEST_FILENAME} !-d
				RewriteRule ^(.*)$ <site client url>?fonction=page&pu=$1 [L,QSA]
			</Directory>
			</pre>

			If the page smart path does not begin with a / character, it is automatically added. The smart path can be /. It must be unique in a website.
			Pages without smart path are accessible only by their id.
		*/
		class Webpage:
			public tree::TreeNode<
				Webpage,
				tree::TreeRankOrderingPolicy,
				tree::TreeOtherClassRootPolicy<Website>
			>,
			public Object<Webpage, WebpageRecord>
		{
		private:
			static const std::string DATA_PUBLICATION_DATE;
			static const std::string DATA_FORUM;
			static const std::string DATA_DEPTH;

		public:
			/// Chosen registry class.
			typedef util::Registry<Webpage>	Registry;
			typedef Website TreeRoot;

			Webpage(util::RegistryKeyType id = 0);

			~Webpage();

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Gets the MIME type generated by the web page.
				/// @return the MIME type recorded in the object if non empty, text/html else.
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				//////////////////////////////////////////////////////////////////////////
				/// For more informations about MIME Types :
				/// http://en.wikipedia.org/wiki/Internet_media_type
				//////////////////////////////////////////////////////////////////////////
				std::string getMimeType() const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the template applicable to the current page.
				/// @return in decreasing priority order :
				///		- NULL if the template must not be used according to the attribute
				///		- the template specified in the attribute
				///		- NULL if the page is the default template of the site
				///		- the default template of the site
				/// @author Hugues Romain
				/// @date 2010
				Webpage* getTemplate() const;



				//////////////////////////////////////////////////////////////////////////
				/// Tests if the page must be displayed according to the current time.
				/// @param now the current time (default = now)
				/// @return true if the page must be displayed
				/// @author Hugues Romain
				/// @date 2010
				bool mustBeDisplayed(
					boost::posix_time::ptime now = boost::posix_time::second_clock::local_time()
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Parses the content and generates the result.
				/// @param stream stream to write on
				/// @author Hugues Romain
				/// @date 2013
				void display(
					std::ostream& stream,
					util::ParametersMap& additionalParametersMap
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Parses the content and generates the result.
				/// @param stream stream to write on
				/// @param request current request
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				void display(
					std::ostream& stream,
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Parses the content and generates the result.
				/// @param stream stream to write on
				/// @param request current request
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.4.0
				void display(
					std::ostream& stream,
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap,
					util::ParametersMap& variables
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Computes the depth level of the page position in the tree.
				/// @return the depth of the page
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.18
				std::size_t getDepth() const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the name of each page from the top of the tree
				/// @return the name of each page from the top of the tree
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.2.0
				std::string getFullName() const;


				boost::posix_time::time_duration getMaxAge() const;
				
				//////////////////////////////////////////////////////////////////////////
				/// CMS exporter.
				//////////////////////////////////////////////////////////////////////////
				/// @param pm parameters map to populate
				/// @param prefix prefix to add to the field names
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				virtual void addAdditionalParameters(
					util::ParametersMap& pm,
					std::string prefix = std::string()
				) const;


				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();


				virtual std::string getName() const { return get<Title>(); }
				virtual SubObjects getSubObjects() const;

				void getPagesList(
					Website::WebpagesList& result,
					const std::string prefix
				) const;
			//@}
		};
}	}

#endif // SYNTHESE_cms_WebPage_h__
