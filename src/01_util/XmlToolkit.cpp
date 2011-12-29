
/** XmlToolkit class implementation.
	@file XmlToolkit.cpp

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

#include "XmlToolkit.h"
#include "XmlException.h"
#include "Conversion.h"

#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;

namespace synthese
{
	namespace util
	{
		int
		XmlToolkit::GetChildNodeCount (XMLNode& node,
						   const std::string& nodeName)
		{
			return node.nChildNode (nodeName.c_str());
		}



		XMLNode
		XmlToolkit::GetChildNode (XMLNode& node,
					  const std::string& nodeName,
					  int index)
		{
			return node.getChildNode (nodeName.c_str(), index);
		}



		XMLNode
		XmlToolkit::ParseString (const std::string& xmlString, const std::string& rootNodeTag)
		{
			return XMLNode::parseString (xmlString.c_str (), rootNodeTag.c_str ());
		}





		XMLNode
		XmlToolkit::ParseFile (const boost::filesystem::path& file,
					   const std::string& rootNodeTag)
		{
			return XMLNode::openFileHelper (file.string ().c_str (), rootNodeTag.c_str ());
		}




		const char*
		XmlToolkit::CheckForRequiredAttr (XMLNode& node,
						  const std::string& attrName)
		{
			const char* attrValue = node.getAttribute (attrName.c_str ());
			if (attrValue == 0)
			{
				std::stringstream msg;
				msg << "No such attribute '" << attrName << "' on element <" <<
					node.getName () << ">";
				throw XmlException (msg.str ());
			}
			return attrValue;
		}



		bool
		XmlToolkit::HasAttr (XMLNode& node,
					 const std::string& attrName)
		{
			return node.getAttribute (attrName.c_str ()) != 0;
		}



		std::string
		XmlToolkit::GetStringAttr (XMLNode& node,
					   const std::string& attrName)
		{
			const char* attrValue = CheckForRequiredAttr (node, attrName);
			return std::string (attrValue);
		}



		std::string
		XmlToolkit::GetStringAttr (XMLNode& node,
					   const std::string& attrName,
					   const std::string& defaultValue)
		{
			const char* attrValue = node.getAttribute (attrName.c_str ());
			return (attrValue == 0)
			? defaultValue : std::string (attrValue);

		}



		bool
		XmlToolkit::GetBoolAttr (XMLNode& node,
					 const std::string& attrName)
		{
			const char* attrValue = CheckForRequiredAttr (node, attrName);
			return lexical_cast<bool>(attrValue);
		}


		bool
		XmlToolkit::GetBoolAttr (XMLNode& node,
					 const std::string& attrName,
					 bool defaultValue)
		{
			const char* attrValue = node.getAttribute (attrName.c_str ());
			return (attrValue == 0)
			? defaultValue : lexical_cast<bool>(attrValue);
		}




		boost::logic::tribool
		XmlToolkit::GetTriboolAttr (XMLNode& node,
						const std::string& attrName)
		{
			const char* attrValue = CheckForRequiredAttr (node, attrName);
			return Conversion::ToTribool(attrValue);
		}




		boost::logic::tribool
		XmlToolkit::GetTriboolAttr (XMLNode& node,
						const std::string& attrName,
						boost::logic::tribool defaultValue)
		{
			const char* attrValue = node.getAttribute (attrName.c_str ());
			return (attrValue == 0)
			? defaultValue : Conversion::ToTribool (attrValue);
		}



		int	XmlToolkit::GetIntAttr(
			XMLNode& node,
			const std::string& attrName
		){
			const char* attrValue = CheckForRequiredAttr (node, attrName);
			return lexical_cast<int>(attrValue);
		}


		int
		XmlToolkit::GetIntAttr (XMLNode& node,
					const std::string& attrName,
					int defaultValue)
		{
			const char* attrValue = node.getAttribute (attrName.c_str ());
			return (attrValue == 0)
			? defaultValue : lexical_cast<int>(attrValue);
		}



		long
		XmlToolkit::GetLongAttr (XMLNode& node,
					 const std::string& attrName)
		{
			const char* attrValue = CheckForRequiredAttr (node, attrName);
			return lexical_cast<long>(attrValue);
		}



		long
		XmlToolkit::GetLongAttr (XMLNode& node,
					 const std::string& attrName,
					 long defaultValue)
		{
			const char* attrValue = node.getAttribute (attrName.c_str ());
			return (attrValue == 0)
			? defaultValue : lexical_cast<long>(attrValue);
		}



		long long
		XmlToolkit::GetLongLongAttr (XMLNode& node,
						 const std::string& attrName)
		{
			const char* attrValue = CheckForRequiredAttr (node, attrName);
			return lexical_cast<long long>(attrValue);
		}


		long long
		XmlToolkit::GetLongLongAttr (XMLNode& node,
						 const std::string& attrName,
						 long long defaultValue)
		{
			const char* attrValue = node.getAttribute (attrName.c_str ());
			return (attrValue == 0)
			? defaultValue : lexical_cast<long long>(attrValue);
		}



		double
		XmlToolkit::GetDoubleAttr (XMLNode& node,
					   const std::string& attrName)
		{
			const char* attrValue = CheckForRequiredAttr (node, attrName);
			return lexical_cast<double>(attrValue);
		}


		double
		XmlToolkit::GetDoubleAttr (XMLNode& node,
					   const std::string& attrName,
					   double defaultValue)
		{
			const char* attrValue = node.getAttribute (attrName.c_str ());
			return (attrValue == 0)
			? defaultValue : lexical_cast<double>(attrValue);
		}
}	}
