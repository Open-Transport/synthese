#ifndef SYNTHESE_ENVLSXML_ROADCHUNKLS_H
#define SYNTHESE_ENVLSXML_ROADCHUNKLS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class Road;
    class RoadChunk;
}

namespace envlsxml
{


/** Road chunk XML loading/saving service class.

Sample XML format :

@code
<lineStop id="1" metricOffset="352.2"
	  type="D" physicalStopId="3">
  <point x="120.5" y="4444.2"/>
  <point x="130.5" y="4434.4"/>
  <point x="140.2" y="4414.2"/>
</lineStop>
@endcode

 @ingroup m16
*/
class RoadChunkLS
{
 public:


    static const std::string ROADCHUNK_TAG;
    static const std::string ROADCHUNK_ID_ATTR;
    static const std::string ROADCHUNK_ADDRESSID_ATTR;
    static const std::string ROADCHUNK_RANKINPATH_ATTR;
    static const std::string ROADCHUNK_ISDEPARTURE_ATTR;
    static const std::string ROADCHUNK_ISARRIVAL_ATTR;


 private:

    RoadChunkLS ();
    ~RoadChunkLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads a road chunk from an XML DOM node.
	Conversion from ids to pointers is done thanks to the
	environment parameter.
     */
    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::RoadChunk* roadChunk);
    //@}


};



}
}


#endif
