
/** XmlToolkit class header.
	@file XmlToolkit.h

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


#ifndef SYNTHESE_UTIL_XMLTOOLKIT_H
#define SYNTHESE_UTIL_XMLTOOLKIT_H

#include "XmlParser.h"

#include <iostream>
#include <string>
#include <boost/logic/tribool.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace util
	{
		/** Toolkit for XML related tasks.
			@ingroup m01
		*/
		namespace XmlToolkit
		{

			const char* CheckForRequiredAttr (XMLNode& node,
								 const std::string& attrName);

			bool HasAttr (XMLNode& node,
				  const std::string& attrName);

			int GetChildNodeCount (XMLNode& node,
					   const std::string& nodeName);

			XMLNode GetChildNode (XMLNode& node,
					  const std::string& nodeName,
					  int index);


			XMLNode ParseString (const std::string& str, const std::string& rootNodeTag);

			XMLNode ParseFile (const boost::filesystem::path& file, const std::string& rootNodeTag);


			std::string GetStringAttr (XMLNode& node,
						   const std::string& attrName);

			std::string GetStringAttr (XMLNode& node,
						   const std::string& attrName,
						   const std::string& defaultValue);

			bool GetBoolAttr (XMLNode& node,
					  const std::string& attrName);

			bool GetBoolAttr (XMLNode& node,
					  const std::string& attrName,
					  bool defaultValue);

			boost::logic::tribool GetTriboolAttr (XMLNode& node,
							  const std::string& attrName);

			boost::logic::tribool GetTriboolAttr (XMLNode& node,
							  const std::string& attrName,
							  boost::logic::tribool defaultValue);

			int GetIntAttr (XMLNode& node,
					   const std::string& attrName);

			int GetIntAttr (XMLNode& node,
					   const std::string& attrName,
					   int defaultValue);

			long GetLongAttr (XMLNode& node, const std::string& attrName);

			long GetLongAttr (XMLNode& node,
						 const std::string& attrName,
						 long defaultValue);

			long long GetLongLongAttr (XMLNode& node,
							  const std::string& attrName);

			long long GetLongLongAttr (XMLNode& node,
							  const std::string& attrName,
							  long long defaultValue);

			double GetDoubleAttr (XMLNode& node,
						 const std::string& attrName);

			double GetDoubleAttr (XMLNode& node,
						 const std::string& attrName,
						 double defaultValue);

			void ToXsdDateTime(
				std::ostream& str,
				const boost::posix_time::ptime& value
			);


			boost::posix_time::ptime GetXsdDateTime(
				const std::string& str
			);

			boost::posix_time::ptime GetIneoDateTime(
				const std::string& str
			);
}	}	}

#endif
