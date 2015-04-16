
/** CMSModule class header.
	@file CMSModule.hpp
	@author Hugues Romain
	@date 2010

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

#ifndef SYNTHESE_CMSModule_H__
#define SYNTHESE_CMSModule_H__

#include "ModuleClassTemplate.hpp"

#include <boost/thread/mutex.hpp>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	//////////////////////////////////////////////////////////////////////////
	/// 36 CMS Module namespace.
	///	@author Hugues Romain
	///	@date 2010
	/// @ingroup m36
	namespace cms
	{
		/**	@defgroup m36Exceptions 36.01 Exceptions
			@ingroup m36

			@defgroup m36LS 36.10 Table synchronizers
			@ingroup m36

			@defgroup m36Pages 36.11 Interface Pages
			@ingroup m36

			@defgroup m36Library 36.11 Interface Library
			@ingroup m36

			@defgroup m36Rights 36.12 Rights
			@ingroup m36

			@defgroup m36Logs 36.13 DB Logs
			@ingroup m36

			@defgroup m36Admin 36.14 Administration pages
			@ingroup m36

			@defgroup m36Actions 36.15 Actions
			@ingroup m36

			@defgroup m36Functions 36.15 Functions
			@ingroup m36

			@defgroup m36File 36.16 File formats
			@ingroup m36

			@defgroup m36Alarm 36.17 Messages recipient
			@ingroup m36

			@defgroup m36 36 CMS
			@ingroup m3

			@todo Write Module documentation

			@{
		*/

		class Website;
		class Webpage;

		//////////////////////////////////////////////////////////////////////////
		/// 36 CMS Module class.
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		//////////////////////////////////////////////////////////////////////////
		/// Sites ares indexed in the module by their client URL, to allow to load
		/// a site by simple URL interpretation.
		class CMSModule:
			public server::ModuleClassTemplate<CMSModule>
		{
		public:
			typedef std::map<std::pair<std::string, std::string>, Website*> SitesByURL;

		private:
			static SitesByURL _sitesByURL;
			static boost::mutex _sitesByURLMutex;

		public:

			//////////////////////////////////////////////////////////////////////////
			/// Gets the site used by the current request.
			/// @param request current request
			/// @author Hugues Romain
			/// @version 3.1.16
			//////////////////////////////////////////////////////////////////////////
			/// The current function must belong to this list :
			///	<ul>
			///		<li>A subclass from FunctionWithSiteBase (the site is read in the base class)</li>
			///		<li>WebPageDisplayFunction (the site is read from the displayed page)</li>
			///	</ul>
			static const Website* GetSite(
				const server::Request& request,
				const util::ParametersMap& pm
			);



			//////////////////////////////////////////////////////////////////////////
			/// Gets the current displayed web page.
			/// @param request current request
			/// @author Hugues Romain
			/// @version 3.1.16
			//////////////////////////////////////////////////////////////////////////
			/// The current function must be WebPageDisplayFunction
			static const Webpage* GetWebPage(
				const server::Request& request
			);



			//////////////////////////////////////////////////////////////////////////
			/// Removes a site from the map of sites indexed by client URL.
			/// @param site the site to remove
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.2.0
			static void RemoveSite(
				const Website& site
			);



			//////////////////////////////////////////////////////////////////////////
			/// Adds a site on the map of sites indexed by client URL.
			/// @param site the site to add
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.2.0
			static void AddSite(
				Website& site
			);



			//////////////////////////////////////////////////////////////////////////
			/// Finds a site by its URL.
			/// @param hostName host name
			/// @param clientURL client URL to search
			/// @return pointer to the site if found, NULL else
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.2.0
			static Website* GetSiteByURL(
				const std::string& hostName,
				const std::string& clientURL
			);



			virtual void displayAdmin(
				std::ostream& stream,
				const server::Request& request,
				const admin::AdminInterfaceElement& currentPage
			) const;
		};
	}
	/** @} */
}

#endif // SYNTHESE_CMSModule_H__
