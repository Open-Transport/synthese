
/** GotoNode class implementation.
	@file GotoNode.cpp

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

#include "GotoNode.hpp"

using namespace std;

namespace synthese
{
	namespace cms
	{
		GotoNode::GotoNode(
			string::const_iterator& it,
			string::const_iterator end
		){
			set<string> terminations;
			terminations.insert("%>");
			_direction = CMSScript(it, end, terminations);
		}


		void GotoNode::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
			) const {
			_direction.display(stream, request, additionalParametersMap, page, variables);
		}
}	}
