
/** CMSScript class header.
	@file CMSScript.hpp

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

#ifndef SYNTHESE_cms_CMSScript_hpp__
#define SYNTHESE_cms_CMSScript_hpp__

#include "shared_recursive_mutex.hpp"

#include <set>
#include <string>
#include <vector>

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
		class WebpageContentNode;



		/** CMSScript class.
			@ingroup m11
		*/
		class CMSScript
		{
		public:
			typedef CMSScript Type;

		private:
			std::string _code;
			bool _ignoreWhiteChars;
			bool _doNotEvaluate;

			typedef std::vector<boost::shared_ptr<WebpageContentNode> > Nodes;
			mutable Nodes _nodes;
			boost::shared_ptr<util::shared_recursive_mutex> _sharedMutex;

			void _updateNodes();

			void _parse(
				std::string::const_iterator& it,
				std::string::const_iterator end,
				std::set<std::string> termination
			);

		public:
			CMSScript();
			CMSScript(
				const std::string& code,
				bool ignoreWhiteChars = false,
				bool doNotEvaluate = false
			);

			CMSScript(
				std::string::const_iterator& it,
				std::string::const_iterator end,
				std::set<std::string> termination,
				bool ignoreWhiteChars = false
			);

			bool operator==(const CMSScript& other) const; 
			
			/// @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Checks if the objects has at least a node to display.
				bool empty() const { return _nodes.empty(); }



				void display(
					std::ostream& stream,
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap,
					const Webpage& page,
					util::ParametersMap& variables
				) const;



				void display(
					std::ostream& stream,
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap
				) const;



				void display(
					std::ostream& stream,
					const util::ParametersMap& additionalParametersMap
				) const;


				void display(
					std::ostream& stream
				) const;


				std::string eval(
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap,
					const Webpage& page,
					util::ParametersMap& variables
				) const;

				std::string eval(
					const server::Request& request,
					const util::ParametersMap& additionalParametersMap
				) const;

				std::string eval(
					const util::ParametersMap& additionalParametersMap
				) const;

				std::string eval() const;
			//@}

			void setCode(const std::string& value);
			
			bool update(
				const std::string& code,
				bool ignoreWhiteChars,
				bool doNotEvaluate
			);


			const std::string& getCode() const { return _code; }
			bool getIgnoreWhiteChars() const { return _ignoreWhiteChars; }
			bool getDoNotEvaluate() const { return _doNotEvaluate; }

		};
	}
}

#endif // SYNTHESE_cms_CMSScript_hpp__

