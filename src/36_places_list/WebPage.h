
/** WebPage class header.
	@file WebPage.h

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

#ifndef SYNTHESE_transportwebsite_WebPage_h__
#define SYNTHESE_transportwebsite_WebPage_h__

#include "Registrable.h"
#include "Registry.h"
#include "TreeNode.hpp"
#include "TreeRankOrderingPolicy.hpp"
#include "TreeOtherClassRootPolicy.hpp"
#include "Named.h"
#include "Site.h"

#include <ostream>
#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace transportwebsite
	{
		/** Web page class (CMS).
			@ingroup m56

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
		*/
		class WebPage:
			public util::Registrable,
			public tree::TreeNode<
				WebPage,
				tree::TreeRankOrderingPolicy,
				tree::TreeOtherClassRootPolicy<Site>
			>,
			public util::Named
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<WebPage>	Registry;

		private:
			std::string _content;
			boost::posix_time::ptime _startDate;
			boost::posix_time::ptime _endDate;



			//////////////////////////////////////////////////////////////////////////
			/// Parses a string and displays the output on a stream
			/// @param stream stream to write the result on
			/// @param it iterator on the beginning of the string to parse
			/// @param end iterator on the end of the string to parse
			/// @param request request which has launched the display of the webpage
			/// @return iterator on the end of the parsing
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			/// The parsing stops when the iterator has reached the end of the string, or if the ?> sequence has been found, indicating that the following text belongs to
			///	a lower level of recursion.
			std::string::const_iterator _parse(
				std::ostream& stream,
				std::string::const_iterator it,
				std::string::const_iterator end,
				const server::Request& request
			) const;

		public:
			WebPage(util::RegistryKeyType id = UNKNOWN_VALUE);

			//! @name Getters
			//@{
				const std::string& getContent() const { return _content; }
				const boost::posix_time::ptime& getStartDate() const { return _startDate; }
				const boost::posix_time::ptime& getEndDate() const { return _endDate; }
			//@}

			//! @name Setters
			//@{
				void setContent(const std::string& value) { _content = value; }
				void setStartDate(const boost::posix_time::ptime& value) { _startDate = value; }
				void setEndDate(const boost::posix_time::ptime& value) { _endDate = value; }
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Tests if the page must be displayed according to the current time.
				/// @param now the current time (default = now)
				/// @return true if the page must be displayed
				/// @author Hugues Romain
				/// @date 2010
				bool mustBeDisplayed(boost::posix_time::ptime now = boost::posix_time::second_clock::local_time()) const;



				//////////////////////////////////////////////////////////////////////////
				/// Parses the content and generates the result.
				/// @param stream stream to write on
				/// @param request current request
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				void display(
					std::ostream& stream,
					const server::Request& request
				) const;

			//@}
		};
	}
}

#endif // SYNTHESE_transportwebsite_WebPage_h__
