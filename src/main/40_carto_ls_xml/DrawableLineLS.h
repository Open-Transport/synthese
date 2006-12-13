#ifndef SYNTHESE_CARTOLSXML_DRAWABLELINELS_H
#define SYNTHESE_CARTOLSXML_DRAWABLELINELS_H


#include "module.h"
#include <string>

class XMLNode;




namespace synthese
{
namespace carto
{
    class DrawableLine;
}
namespace env
{
    class Environment;
}



namespace cartolsxml
{


/** Drawable line XML loading/saving service class.

Sample XML format :

<drawableLine lineId="L1" fromLineStopIndex="5" toLineStopIndex="10"/>

 @ingroup m40
*/
class DrawableLineLS
{
 public:

    static const std::string DRAWABLELINE_TAG;
    static const std::string DRAWABLELINE_LINEID_ATTR;
    static const std::string DRAWABLELINE_FROMLINESTOPINDEX_ATTR;
    static const std::string DRAWABLELINE_TOLINESTOPINDEX_ATTR;
    static const std::string DRAWABLELINE_WITHPHYSICALSTOPS_ATTR;

 private:

    DrawableLineLS ();
    ~DrawableLineLS();

 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads a drawable line from an XML DOM node.
	Conversion from ids to pointers is done thanks to the
	environment parameter.
     */
    static synthese::carto::DrawableLine* Load (
	XMLNode& node,
	const synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::carto::DrawableLine* drawableLine);
    //@}


};



}
}


#endif

