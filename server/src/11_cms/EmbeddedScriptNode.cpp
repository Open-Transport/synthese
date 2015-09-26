/** EmbeddedScriptNode class implementation.
	@file EmbeddedScriptNode.cpp

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

#include "EmbeddedScriptNode.hpp"
#ifdef WITH_PYTHON
#include "PythonInterpreter.hpp"
#endif
#include <sstream>


namespace synthese {

	namespace cms {

		EmbeddedScriptNode::EmbeddedScriptNode(
			const std::string& scriptInterpreter,
			const std::string& scriptCode
		): 	_scriptInterpreter(scriptInterpreter), _scriptCode(scriptCode)
		{

		}

		void EmbeddedScriptNode::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const
		{			
			// Note : the attributes 'client_url', 'host_name', 'site' and 'p' are not contained in additionalParamatersMap so they are not passed to PythonInterpreter
			// See VariableExpression::eval() for details
#ifdef WITH_PYTHON
			PythonInterpreter::Execute(_scriptCode, stream, additionalParametersMap, variables);
#endif
		}


		std::string EmbeddedScriptNode::eval(
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const
		{
			std::stringstream s;
			display(s, request, additionalParametersMap, page, variables);
			return s.str();
		}

	}

}
