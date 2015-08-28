/** EmbeddedScriptNode class header.
		@file EmbeddedScriptNode.hpp

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

#ifndef SYNTHESE_cms_EmbeddedScriptNode_hpp__
#define SYNTHESE_cms_EmbeddedScriptNode_hpp__

#include "WebpageContentNode.hpp"


namespace synthese
{
	namespace cms
	{

		/** EmbeddedScriptNode class.
		@ingroup m11
		*/
		class EmbeddedScriptNode :
			public WebpageContentNode
		{

		public:

			EmbeddedScriptNode(
				const std::string& scriptInterpreter,
				const std::string& scriptCode
			);

			virtual void display(
				std::ostream& stream,
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			) const;

			virtual std::string eval(
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			) const;

		private:

			std::string _scriptInterpreter;

			std::string _scriptCode;

		};

	}
}

#endif // SYNTHESE_cms_EmbeddedScriptNode_hpp__
