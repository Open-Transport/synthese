
/** Webpage class header.
	@file Webpage.h

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

#ifndef SYNTHESE_cms_WebPage_h__
#define SYNTHESE_cms_WebPage_h__

#include "Registrable.h"
#include "Registry.h"
#include "TreeNode.hpp"
#include "TreeRankOrderingPolicy.hpp"
#include "TreeOtherClassRootPolicy.hpp"
#include "Named.h"
#include "Website.hpp"
#include "Function.h"
#include "Factory.h"
#include "shared_recursive_mutex.hpp"

#include <ostream>
#include <boost/date_time/posix_time/posix_time.hpp>

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
			public util::Registrable,
			public tree::TreeNode<
				Webpage,
				tree::TreeRankOrderingPolicy,
				tree::TreeOtherClassRootPolicy<Website>
			>,
			public util::Named
		{
		private:

			// Precompiled webpage

			class Node
			{
			public:
				static const std::string PARAMETER_TRANSMIT_PARAMETERS;

				virtual void display(
					std::ostream& stream,
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap,
					const Webpage& page
				) const = 0;
			};

			typedef std::vector<boost::shared_ptr<Node> > Nodes;

			class TextNode : public Node
			{
			public:
				std::string text;

				virtual void display(
					std::ostream& stream,
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap,
					const Webpage& page
				) const;
			};

			class FunctionNode : public Node
			{
			public:
				const util::Factory<server::Function>::CreatorInterface* functionCreator;
				typedef std::vector<std::pair<std::string, Nodes> > Parameters;
				Parameters parameters;

				virtual void display(
					std::ostream& stream,
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap,
					const Webpage& page
				) const;
			};

			class LabelNode : public Node
			{
			public:
				std::string label;

				virtual void display(
					std::ostream& stream,
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap,
					const Webpage& page
				) const;
			};

			class GotoNode : public Node
			{
			public:
				Nodes direction;

				virtual void display(
					std::ostream& stream,
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap,
					const Webpage& page
				) const;
			};

			class ValueNode : public Node
			{
			public:
				std::string name;

				virtual void display(
					std::ostream& stream,
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap,
					const Webpage& page
				) const;
			};

			class IncludeNode : public Node
			{
			public:
				std::string pageName;
				typedef std::vector<std::pair<std::string, Nodes> > Parameters;
				Parameters parameters;

				virtual void display(
					std::ostream& stream,
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap,
					const Webpage& page
				) const;
			};

		public:
			/// Chosen registry class.
			typedef util::Registry<Webpage>	Registry;

			typedef std::vector<Webpage*> Links;

		private:
			mutable Nodes _nodes;

			std::string _smartURLPath;
			std::string _smartURLDefaultParameterName;

			Webpage* _template;
			std::string _content;
			boost::posix_time::ptime _startDate;
			boost::posix_time::ptime _endDate;
			std::string _mimeType;
			std::string _abstract;
			std::string _image;
			Links _links;
			bool _doNotUseTemplate;
			bool _hasForum;
			bool _ignoreWhiteChars;
			bool _rawEditor; //!< if false the editor is WYSIWYG editor
			static synthese::util::shared_recursive_mutex _SharedMutex;


			//////////////////////////////////////////////////////////////////////////
			/// Parses the content and put it in the nodes cache.
			/// @retval nodes object to write the result on
			/// @param it iterator on the beginning of the string to parse
			/// @param end iterator on the end of the string to parse
			/// @param termination termination string to detect to interrupt the parsing
			/// @return iterator on the end of the parsing
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			/// The parsing stops when the iterator has reached the end of the string, or if the ?> sequence has been found, indicating that the following text belongs to
			///	a lower level of recursion.
			/// If the level of recursion is superior than 0, then the output is encoded
			/// as an url to avoid mistake when the result of parsing is considered as
			/// a single parameter of a function call.
			std::string::const_iterator _parse(
				Nodes& nodes,
				std::string::const_iterator it,
				std::string::const_iterator end,
				std::set<std::string> termination
			) const;

			//////////////////////////////////////////////////////////////////////////
			/// Parses the content and put it in a stream.
			/// @retval stream stream to write the result on
			/// @param it iterator on the beginning of the string to parse
			/// @param end iterator on the end of the string to parse
			/// @param termination termination string to detect to interrupt the parsing
			/// @return iterator on the end of the parsing
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			/// The parsing stops when the iterator has reached the end of the string, or if the ?> sequence has been found, indicating that the following text belongs to
			///	a lower level of recursion.
			/// If the level of recursion is superior than 0, then the output is encoded
			/// as an url to avoid mistake when the result of parsing is considered as
			/// a single parameter of a function call.
			std::string::const_iterator _parseText(
				std::ostream& stream,
				std::string::const_iterator it,
				std::string::const_iterator end,
				std::string termination
			) const;

		public:
			Webpage(util::RegistryKeyType id = 0);

			//! @name Getters
			//@{
				const std::string& getContent() const { return _content; }
				const boost::posix_time::ptime& getStartDate() const { return _startDate; }
				const boost::posix_time::ptime& getEndDate() const { return _endDate; }
				const std::string& _getMimeType() const { return _mimeType; }
				Webpage* _getTemplate() const { return _template;}
				const std::string& getAbstract() const { return _abstract; }
				const std::string& getImage() const { return _image; }
				const Links& getLinks() const { return _links; }
				bool getDoNotUseTemplate() const { return _doNotUseTemplate; }
				bool getHasForum() const { return _hasForum; }
				const std::string& getSmartURLPath() const { return _smartURLPath; }
				const std::string& getSmartURLDefaultParameterName() const { return _smartURLDefaultParameterName; }
				bool getIgnoreWhiteChars() const { return _ignoreWhiteChars; }
				bool getRawEditor() const { return _rawEditor; }
			//@}

			//! @name Setters
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Content setter with nodes cache update.
				/// The parsing is done only if the content has changed.
				/// @param value new value for the content
				/// @param noUpdate avoid the nodes cache to be updated (optional, default is not to avoid)
				/// @author Hugues Romain
				void setContent(const std::string& value);

				void setStartDate(const boost::posix_time::ptime& value) { _startDate = value; }
				void setEndDate(const boost::posix_time::ptime& value) { _endDate = value; }
				void setMimeType(const std::string& value){ _mimeType = value; }
				void setTemplate(Webpage* value){ _template = value; }
				void setAbstract(const std::string& value){ _abstract = value; }
				void setImage(const std::string& value){ _image = value; }
				void setLinks(const Links& value){ _links = value; }
				void setDoNotUseTemplate(bool value){ _doNotUseTemplate = value; }
				void setHasForum(bool value){ _hasForum = value; }
				void setSmartURLPath(const std::string& value){ _smartURLPath = value; }
				void setSmartURLDefaultParameterName(const std::string& value){ _smartURLDefaultParameterName = value; }
				void setIgnoreWhiteChars(bool value){ _ignoreWhiteChars = value; }
				void setRawEditor(bool value){ _rawEditor = value; }
			//@}

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
			//@}
		};
	}
}

#endif // SYNTHESE_cms_WebPage_h__
