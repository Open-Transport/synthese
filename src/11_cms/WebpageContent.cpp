
/** WebpageContent class implementation.
	@file WebpageContent.cpp

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

#include "WebpageContent.hpp"

#include "FilesMap.hpp"

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace cms;

	template<>
	const Field ComplexObjectFieldDefinition<WebpageContent>::FIELDS[] = {
		Field("content1", SQL_BLOB, true),
		Field("ignore_white_chars", SQL_BOOLEAN),
		Field("mime_type", SQL_TEXT),
		Field("do_not_evaluate", SQL_BOOLEAN),
	Field() };



	namespace cms
	{
		bool WebpageContent::LoadFromRecord(
			WebpageContent& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			bool result(false);

			if(record.isDefined(FIELDS[2].name))
			{
				string value(record.getDefault<string>(FIELDS[2].name));
				MimeType mtValue;
				try
				{
					mtValue = MimeTypes::GetMimeTypeByString(value);
				}
				catch(Exception&)
				{
					std::vector<std::string> parts;
					boost::algorithm::split(parts, value, boost::is_any_of("/"));
					if(parts.size() >= 2)
					{
						mtValue = MimeType(parts[0], parts[1], "");
					}
					else
					{
						mtValue = MimeTypes::HTML;
					}
				}

				if(fieldObject._mimeType != mtValue)
				{
					fieldObject._mimeType = mtValue;
					result = true;
				}
			}

			result |= fieldObject._script.update(
				record.isDefined(FIELDS[0].name) ?
					record.get<string>(FIELDS[0].name) :
					fieldObject.getCMSScript().getCode(),
				record.isDefined(FIELDS[1].name) ?
					record.getDefault<bool>(FIELDS[1].name, false) :
					fieldObject.getCMSScript().getIgnoreWhiteChars(),
				record.isDefined(FIELDS[3].name) ?
					record.getDefault<bool>(FIELDS[3].name, false) :
					fieldObject.getCMSScript().getDoNotEvaluate()
			);

			return result;
		}



		void WebpageContent::SaveToParametersMap(
			const WebpageContent& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			// Content
			if(	withFiles || indeterminate(withFiles)
			){
				map.insert(
					prefix + FIELDS[0].name,
					fieldObject.getCMSScript().getCode()
				);
			}

			// Ignore white chars
			map.insert(
				prefix + FIELDS[1].name,
				fieldObject.getCMSScript().getIgnoreWhiteChars()
			);

			// Mime type
			map.insert(
				prefix + FIELDS[2].name,
				fieldObject._mimeType
			);

			// Ignore white chars
			map.insert(
				prefix + FIELDS[3].name,
				fieldObject.getCMSScript().getDoNotEvaluate()
			);
		}



		void WebpageContent::SaveToFilesMap(
			const WebpageContent& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			FilesMap::File item;
			item.content = fieldObject.getCMSScript().getCode();
			item.mimeType = fieldObject._mimeType;
			map.insert(
				FIELDS[0].name,
				item
			);
		}



		void WebpageContent::SaveToDBContent(
			const Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			// Content
			Blob b;
			b.first = const_cast<char*>(fieldObject.getCMSScript().getCode().c_str());
			b.second = fieldObject.getCMSScript().getCode().size();
			content.push_back(Cell(b));

			// Ignore white chars
			content.push_back(Cell(fieldObject.getCMSScript().getIgnoreWhiteChars()));

			// Mime type
			string s(fieldObject._mimeType);
			content.push_back(Cell(s));

			// Do not evaluate
			content.push_back(Cell(fieldObject.getCMSScript().getDoNotEvaluate()));
		}



		WebpageContent::WebpageContent(
		):	_mimeType(MimeTypes::HTML)
		{}



		WebpageContent::WebpageContent(
			const std::string& code,
			bool ignoreWhiteChars,
			MimeType mimeType,
			bool doNotEvaluate
		):	_script(code, ignoreWhiteChars, doNotEvaluate),
			_mimeType(mimeType)
		{
		}




		void WebpageContent::GetLinkedObjectsIds( LinkedObjectsIds& list, const Record& record )
		{

		}
}	}
