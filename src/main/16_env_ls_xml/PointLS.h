#ifndef SYNTHESE_ENVLSXML_POINTLS_H
#define SYNTHESE_ENVLSXML_POINTLS_H


#include "module.h"
#include <string>

class XMLNode;


namespace synthese
{
namespace env
{
    class Point;
}

namespace envlsxml
{


/** Point XML loading/saving service class.

Sample XML format :

@code
<point x="1334.3" y="444.3"/>
@endcode

 @ingroup m16
*/
class PointLS
{
 public:

    static const std::string POINT_TAG;
    static const std::string POINT_X_ATTR;
    static const std::string POINT_Y_ATTR;

 private:

    PointLS ();
    ~PointLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads a point from an XML DOM node.
     */
    static synthese::env::Point Load (
	XMLNode& node);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::Point* point);
    //@}


};



}
}


#endif

