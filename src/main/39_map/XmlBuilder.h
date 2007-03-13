#ifndef SYNTHESE_MAP_XMLBUILDER_H
#define SYNTHESE_MAP_XMLBUILDER_H



#include "15_env/Line.h"


struct XMLNode;


namespace synthese
{




namespace map
{

    class DrawableLine;
    class Map;


class XmlBuilder
{
 public:


 private:

    XmlBuilder ();
    ~XmlBuilder();


 public:


    static DrawableLine* CreateDrawableLine (XMLNode& node, const synthese::env::Line::Registry& lines);

    static Map* CreateMap (XMLNode& node, const synthese::env::Line::Registry& lines);


};



}
}


#endif

