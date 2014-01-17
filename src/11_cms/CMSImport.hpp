
//////////////////////////////////////////////////////////////////////////
/// CMSImport class header.
///	@file CMSImport.hpp
///	@author Bruno Coudoin
///	@date 2014
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2014 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_cms_CMSImport_H__
#define SYNTHESE_cms_CMSImport_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include "boost/filesystem.hpp"

namespace synthese
{
	namespace cms
	{
		class Website;
		class Webpage;

		//////////////////////////////////////////////////////////////////////////
		/// CMS Import action.
		/// @ingroup m56Actions refActions
		///
		/// This feature import a given directory on the disk where SYNTHESE runs
		/// in an existing CMS site of CMS page. In all cases DIRECTORY_PATH must
		/// be provided and one of:
		///		- PARENT_ID : creation of a child of an other page after the last sibling if a child page already exists
		///		- SITE_ID : creation of a page at the last position of a web site
		/// 	- None of the above, then a new site is created with the name being
		/// 	  the last path name of DIRECTORY_PATH
		///
		/// Other options:
		/// 	- MAX_AGE: Default max age for all the pages
		/// 	- EXCLUDE_LIST: A space separated list of files or directory to exclude.
		/// 	  The list accept regular expressions like ".git .svn *~ *.bak"
		class CMSImport:
			public util::FactorableTemplate<server::Action, CMSImport>
		{
		public:
			static const std::string PARAMETER_DIRECTORY_PATH;
			static const std::string PARAMETER_PARENT_ID;
			static const std::string PARAMETER_SITE_ID;
			static const std::string PARAMETER_MAX_AGE; // In seconds
			static const std::string PARAMETER_EXCLUDE_LIST;

		private:
			std::string _directoryPath;
			boost::shared_ptr<Webpage> _parent;
			boost::shared_ptr<Website> _site;
			boost::posix_time::time_duration _maxAge;
			std::string _excludeList;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

			void _importDir(Webpage *parent, boost::filesystem::path currentDir);

		public:
			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;


			//! @name Setters
			//@{
			//@}

		};
	}
}
#endif // SYNTHESE_cms_CMSImport_H__
