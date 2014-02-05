
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

#include "DBTransaction.hpp"
#include "FileFormatTemplate.h"
#include "Importer.hpp"
#include "InterSYNTHESEPackageContent.hpp"
#include "NoExportPolicy.hpp"
#include "OneFileTypeImporter.hpp"

#include "boost/filesystem.hpp"
#include "boost/regex.hpp"

namespace synthese
{
	namespace cms
	{
		class Website;
		class Webpage;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// CMS Import
		///
		/// This feature import a given directory on the disk where SYNTHESE runs
		/// in an existing CMS site or CMS page. In all cases DIRECTORY must
		/// be provided and one of:
		///		- PARENT_ID : creation of a child of an other page after the last sibling if a child page already exists
		///		- SITE_ID : creation of a page at the last position of a web site
		/// 	- None of the above, then a new site is created with the name being
		/// 	  the last path name of DIRECTORY_PATH
		///
		/// Other options:
		/// 	- MAX_AGE: Default max age for all the pages
		/// 	- EXCLUDE_LIST: A space separated list of files or directory to exclude.
		/// 	  The list accepts regular expressions like ".git .svn *~ *.bak"
		class CMSImport:
			public impex::FileFormatTemplate<CMSImport>
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::Importer
			{

			private:
				std::string _directory;
				boost::shared_ptr<cms::Webpage> _parent;
				mutable boost::shared_ptr<cms::Website> _site;
				boost::posix_time::time_duration _maxAge;
				std::string _excludeList;
				std::vector<boost::regex> _excludeListRegEx;
				typedef std::map<std::string, cms::Webpage*> MetadataPageMap;
				mutable MetadataPageMap _metadataPages;
				mutable boost::shared_ptr<inter_synthese::InterSYNTHESEPackageContent> _ispc;

				void _getPageFullPath(cms::Webpage *page, string &path) const;
				cms::Website *_createDefaultSite() const;
				void _readMetadata(std::string &metadata) const;

				bool _isExcluded(
					const vector<boost::regex> &regexps,
					const std::string &fileName
				) const;

				void _importDir(const boost::filesystem::path &directoryPath,
					boost::shared_ptr<cms::Website> &site,
					cms::Webpage *parent,
					boost::filesystem::path currentDir
				) const;

			public:
				static const bool IMPORTABLE;
				static const bool IS_PERMANENT_THREAD;
				static const std::string PARAMETER_DIRECTORY;
				static const std::string PARAMETER_PARENT_ID;
				static const std::string PARAMETER_IMPORT_SITE_ID;
				static const std::string PARAMETER_MAX_AGE; // In seconds
				static const std::string PARAMETER_EXCLUDE_LIST;

				Importer_(
					util::Env& env,
					const impex::Import& import,
					impex::ImportLogLevel minLogLevel,
					const std::string& logPath,
					boost::optional<std::ostream&> outputStream,
					util::ParametersMap& pm
				);

				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.2.1
				virtual util::ParametersMap getParametersMap() const;

				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.2.1
				virtual void setFromParametersMap(
					const util::ParametersMap& map,
					bool doImport
				);

				virtual bool parseFiles() const;
				virtual db::DBTransaction _save() const;

			};

			typedef impex::NoExportPolicy<CMSImport> Exporter_;

		};

	}
}
#endif // SYNTHESE_cms_CMSImport_H__
