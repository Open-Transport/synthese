
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

#include <ostream>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace transportwebsite
	{
		class Site;

		/** WebPage class.
			@ingroup m36
		*/
		class WebPage:
		public util::Registrable
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<WebPage>	Registry;

		private:
			std::string _title;
			std::string _content1;
			std::string _include1;
			std::string _content2;
			std::string _include2;
			std::string _content3;
			Site* _site;

		public:
			WebPage(util::RegistryKeyType id = UNKNOWN_VALUE);

			const std::string& getTitle() const;
			const std::string& getContent1() const;
			const std::string& getInclude1() const;
			const std::string& getContent2() const;
			const std::string& getInclude2() const;
			const std::string& getContent3() const;
			Site* getSite() const;

			void setTitle(const std::string& value);
			void setContent1(const std::string& value);
			void setInclude1(const std::string& value);
			void setContent2(const std::string& value);
			void setInclude2(const std::string& value);
			void setContent3(const std::string& value);
			void setSite(Site* value);

			static void DisplayInclude(
				std::ostream& stream,
				const std::string& includeString,
				const server::Request& request
			);
		};
	}
}

#endif // SYNTHESE_transportwebsite_WebPage_h__
