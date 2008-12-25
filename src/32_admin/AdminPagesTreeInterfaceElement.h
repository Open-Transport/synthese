
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

#include "32_admin/AdminInterfaceElement.h"

#include "30_server/FunctionRequest.h"

#include "11_interfaces/LibraryInterfaceElement.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace admin
	{
		class AdminRequest;
		
		/** Tree of admin pages links.
			@ingroup m14Values refLibrary

			Parameters :
				- Sub page introducer
				- Last sub page introducer
				- Level indenter to use before the last subpage
				- Level indenter to use after the last subpage
				- Node + subpages opening
				- Sub pages introducer if opened (might be a div opening) 
				- Sub pages introducer if closed (might be a div opening) 
				- Sub pages ending
				- Opened Node sub page introducer
				- Closed Node sub page introducer
				- Opened Node last sub page introducer
				- Closed Node last sub page introducer
		*/
		class AdminPagesTreeInterfaceElement
			: public util::FactorableTemplate<interfaces::LibraryInterfaceElement, AdminPagesTreeInterfaceElement>
		{
			std::string getSubPages(
				const AdminInterfaceElement::PageLinksTree& pages
				, const AdminInterfaceElement& currentPage
				, int level
				, std::string prefix
				, bool last
			) const;

			boost::shared_ptr<interfaces::LibraryInterfaceElement> _subpageIntroducerVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _lastSubpageIntroducerVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _levelIndenterVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _lastLevelIndenterVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _subpagesIntroducerIfOpenedVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _subpagesIntroducerIfClosedVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _openingVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _endingVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _openedFolderSubpageIntroducerVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _closedFolderSubpageIntroducerVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _openedFolderLastSubpageIntroducerVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _closedFolderLastSubpageIntroducerVIE;

			mutable std::string _subpageIntroducer;
			mutable std::string _lastSubpageIntroducer;
			mutable std::string _levelIndenter;
			mutable std::string _lastLevelIndenter;
			mutable std::string _subpagesIntroducerIfOpened;
			mutable std::string _subpagesIntroducerIfClosed;
			mutable std::string _opening;
			mutable std::string _ending;
			mutable std::string _openedFolderSubpageIntroducer;
			mutable std::string _closedFolderSubpageIntroducer;
			mutable std::string _openedFolderLastSubpageIntroducer;
			mutable std::string _closedFolderLastSubpageIntroducer;

		public:
			/** Controls and store the internals parameters.
				@param vel Parameters list to read
			*/
			void storeParameters(interfaces::ValueElementList& vel);

			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector&
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
			) const;
		};
	}
}

#endif // SYNTHESE_AdminPagesTreeInterfaceElement_H__
