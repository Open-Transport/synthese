
/** WebpageContent class header.
	@file WebpageContent.hpp

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

#ifndef SYNTHESE_cms_WebpageContent_hpp__
#define SYNTHESE_cms_WebpageContent_hpp__

#include "CMSScript.hpp"
#include "Factory.h"
#include "Function.h"
#include "ComplexObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"
#include "MimeType.hpp"
#include "MimeTypes.hpp"

#include <boost/logic/tribool.hpp>
#include <ostream>

namespace synthese
{
	class FilesMap;
	class ObjectBase;

	namespace util
	{
		class Env;
	}

	namespace cms
	{
		class Webpage;

		/** WebpageContent class.
			@ingroup m36
		*/
		class WebpageContent:
			public ComplexObjectFieldDefinition<WebpageContent>
		{
		public:
			typedef WebpageContent Type;

		private:
			CMSScript _script;
			util::MimeType _mimeType;


		public:
			//////////////////////////////////////////////////////////////////////////
			/// Default constructor.
			WebpageContent();



			//////////////////////////////////////////////////////////////////////////
			/// Constructor by entire string parsing.
			WebpageContent(
				const std::string& code,
				bool ignoreWhiteChars = false,
				util::MimeType mimeType = util::MimeTypes::HTML,
				bool doNotEvaluate = false
			);



			/// @name Getters
			//@{
				const CMSScript& getCMSScript() const { return _script; }
				const util::MimeType& getMimeType() const { return _mimeType; }
			//@}



			static bool LoadFromRecord(
				Type& fieldObject,
				ObjectBase& object,
				const Record& record,
				const util::Env& env
			);



			static void SaveToFilesMap(
				const Type& fieldObject,
				const ObjectBase& object,
				FilesMap& map
			);



			static void SaveToParametersMap(
				const Type& fieldObject,
				const ObjectBase& object,
				util::ParametersMap& map,
				const std::string& prefix,
				boost::logic::tribool withFiles
			);



			static void SaveToDBContent(
				const Type& fieldObject,
				const ObjectBase& object,
				DBContent& content
			);


			static void GetLinkedObjectsIds(
				LinkedObjectsIds& list, 
				const Record& record
			);
		};
}	}

#endif // SYNTHESE_cms_WebpageContent_hpp__

