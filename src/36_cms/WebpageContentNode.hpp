
/** WebpageContentNode class header.
	@file WebpageContentNode.hpp

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

#ifndef SYNTHESE_cms_WebpageContentNode_hpp__
#define SYNTHESE_cms_WebpageContentNode_hpp__

#include <ostream>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace util
	{
		class ParametersMap;
	}

	namespace cms
	{
		class Webpage;


		/** WebpageContentNode class.
			@ingroup m36
		*/
		class WebpageContentNode
		{
		protected:
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
			static void ParseText(
				std::ostream& stream,
				std::string::const_iterator& it,
				std::string::const_iterator end,
				std::string termination
			);


			//////////////////////////////////////////////////////////////////////////
			/// Parses the content and put it in a stream.
			/// @param it iterator on the beginning of the string to parse
			/// @param end iterator on the end of the string to parse
			/// @param termination termination string to detect to interrupt the parsing
			/// @return iterator on the end of the parsing
			/// @author Hugues Romain
			/// @date 2012
			/// @since 3.4.0
			/// The parsing stops when the iterator has reached the end of the string, or if the ?> sequence has been found, indicating that the following text belongs to
			///	a lower level of recursion.
			/// If the level of recursion is superior than 0, then the output is encoded
			/// as an url to avoid mistake when the result of parsing is considered as
			/// a single parameter of a function call.
			static std::string ParseText(
				std::string::const_iterator& it,
				std::string::const_iterator end,
				std::string termination
			);



		public:
			virtual void display(
				std::ostream& stream,
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			) const = 0;


			virtual std::string eval(
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			) const;
		};
}	}

#endif // SYNTHESE_cms_WebpageContentNode_hpp__

