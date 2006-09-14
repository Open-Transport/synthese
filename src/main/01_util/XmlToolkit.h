#ifndef SYNTHESE_UTIL_XMLTOOLKIT_H
#define SYNTHESE_UTIL_XMLTOOLKIT_H


#include "module.h"

#include <string>
#include <iostream>

#include <boost/logic/tribool.hpp>

#include "XmlParser.h"



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



};



}

}
#endif
