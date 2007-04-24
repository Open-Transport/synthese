
/** AdminPagesTreeInterfaceElement class header.
	@file AdminPagesTreeInterfaceElement.h

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

#ifndef SYNTHESE_AdminPagesTreeInterfaceElement_H__
#define SYNTHESE_AdminPagesTreeInterfaceElement_H__

#include <string>
#include "11_interfaces/ValueInterfaceElement.h"

namespace synthese
{
	namespace admin
	{
		class AdminRequest;

		/** Tree of admin pages links.
			@ingroup m32Values refValues
		*/
		class AdminPagesTreeInterfaceElement : public interfaces::ValueInterfaceElement
		{
			std::string getSubPages(
				const std::string& page
				, boost::shared_ptr<const AdminInterfaceElement> currentPage
				, const server::FunctionRequest<admin::AdminRequest>* request
				, int level = -1
				, std::string prefix = std::string()
			) const;

			boost::shared_ptr<ValueInterfaceElement> _subpageIntroducerVIE;
			boost::shared_ptr<ValueInterfaceElement> _lastSubpageIntroducerVIE;
			boost::shared_ptr<ValueInterfaceElement> _levelIndenterVIE;
			boost::shared_ptr<ValueInterfaceElement> _lastLevelIndenterVIE;
			boost::shared_ptr<ValueInterfaceElement> _endingVIE;
			mutable std::string _subpageIntroducer;
			mutable std::string _lastSubpageIntroducer;
			mutable std::string _levelIndenter;
			mutable std::string _lastLevelIndenter;
			mutable std::string _ending;

		public:
			/** Controls and store the internals parameters.
				@param vel Parameters list to read
			*/
			void storeParameters(interfaces::ValueElementList& vel);

			std::string getValue(
				const interfaces::ParametersVector&
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
			) const;
		};
	}
}

#endif // SYNTHESE_AdminPagesTreeInterfaceElement_H__
