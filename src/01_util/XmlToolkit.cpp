#include "XmlToolkit.h"


#include "XmlException.h"
#include "Conversion.h"


#include <sstream>
#include <boost/algorithm/string.hpp>



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
    return Conversion::ToBool (attrValue);
}


bool
XmlToolkit::GetBoolAttr (XMLNode& node,
			 const std::string& attrName,
			 bool defaultValue)
{
    const char* attrValue = node.getAttribute (attrName.c_str ());
    return (attrValue == 0)
	? defaultValue : Conversion::ToBool (attrValue);
}




boost::logic::tribool
XmlToolkit::GetTriboolAttr (XMLNode& node,
			    const std::string& attrName)
{
    const char* attrValue = CheckForRequiredAttr (node, attrName);
    return Conversion::ToTribool (attrValue);
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





int
XmlToolkit::GetIntAttr (XMLNode& node,
			const std::string& attrName)
{
    const char* attrValue = CheckForRequiredAttr (node, attrName);
    return Conversion::ToInt (attrValue);
}


int
XmlToolkit::GetIntAttr (XMLNode& node,
			const std::string& attrName,
			int defaultValue)
{
    const char* attrValue = node.getAttribute (attrName.c_str ());
    return (attrValue == 0)
	? defaultValue : Conversion::ToInt (attrValue);
}



long
XmlToolkit::GetLongAttr (XMLNode& node,
			 const std::string& attrName)
{
    const char* attrValue = CheckForRequiredAttr (node, attrName);
    return Conversion::ToLong (attrValue);
}


long
XmlToolkit::GetLongAttr (XMLNode& node,
			 const std::string& attrName,
			 long defaultValue)
{
    const char* attrValue = node.getAttribute (attrName.c_str ());
    return (attrValue == 0)
	? defaultValue : Conversion::ToLong (attrValue);
}



long long
XmlToolkit::GetLongLongAttr (XMLNode& node,
			     const std::string& attrName)
{
    const char* attrValue = CheckForRequiredAttr (node, attrName);
    return Conversion::ToLongLong (attrValue);
}


long long
XmlToolkit::GetLongLongAttr (XMLNode& node,
			     const std::string& attrName,
			     long long defaultValue)
{
    const char* attrValue = node.getAttribute (attrName.c_str ());
    return (attrValue == 0)
	? defaultValue : Conversion::ToLongLong (attrValue);
}



double
XmlToolkit::GetDoubleAttr (XMLNode& node,
			   const std::string& attrName)
{
    const char* attrValue = CheckForRequiredAttr (node, attrName);
    return Conversion::ToDouble (attrValue);
}


double
XmlToolkit::GetDoubleAttr (XMLNode& node,
			   const std::string& attrName,
			   double defaultValue)
{
    const char* attrValue = node.getAttribute (attrName.c_str ());
    return (attrValue == 0)
	? defaultValue : Conversion::ToDouble (attrValue);

}






}
}

