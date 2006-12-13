#include "RoadChunkLS.h"

#include <assert.h>

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "01_util/UId.h"

#include "15_env/Environment.h"
#include "15_env/RoadChunk.h"
#include "15_env/Road.h"
#include "15_env/Address.h"

#include "PointLS.h"
#include "Exception.h"

#include <boost/algorithm/string.hpp>



using namespace synthese::util::XmlToolkit;

using synthese::env::Address;
using synthese::env::Road;
using synthese::env::RoadChunk;


namespace synthese
{
namespace envlsxml
{

const std::string RoadChunkLS::ROADCHUNK_TAG ("roadChunk");
const std::string RoadChunkLS::ROADCHUNK_ID_ATTR ("id");
const std::string RoadChunkLS::ROADCHUNK_ADDRESSID_ATTR ("addressId");
const std::string RoadChunkLS::ROADCHUNK_RANKINPATH_ATTR ("rankInPath");
const std::string RoadChunkLS::ROADCHUNK_ISDEPARTURE_ATTR ("isDeparture");
const std::string RoadChunkLS::ROADCHUNK_ISARRIVAL_ATTR ("isArrival");




void
RoadChunkLS::Load (XMLNode& node,
		  synthese::env::Environment& environment)
{
    // assert (ROADCHUNK_TAG == node.getName ());

    uid id (GetLongLongAttr (node, ROADCHUNK_ID_ATTR));

    if (environment.getRoadChunks ().contains (id)) return;

    uid addressId (GetLongLongAttr (node, ROADCHUNK_ADDRESSID_ATTR));
    int rankInPath (GetIntAttr (node, ROADCHUNK_RANKINPATH_ATTR));
    bool isDeparture (GetBoolAttr (node, ROADCHUNK_ISDEPARTURE_ATTR, true));
    bool isArrival (GetBoolAttr (node, ROADCHUNK_ISARRIVAL_ATTR, true));

    Address* address = environment.getAddresses ().get (addressId);
    Road* road = environment.getRoads ().get (address->getRoad ()->getKey ());
    
    RoadChunk* roadChunk = new RoadChunk (id,
					  address, 
					  rankInPath, 
					  isDeparture, isArrival);
    
    // Add via points
    int nbPoints = GetChildNodeCount(node, PointLS::POINT_TAG);
    for (int i=0; i<nbPoints; ++i) 
    {
	XMLNode pointNode = GetChildNode (node, PointLS::POINT_TAG, i);
	roadChunk->addViaPoint (PointLS::Load (pointNode));
    }
    
    road->addEdge (roadChunk);
    environment.getRoadChunks ().add (roadChunk);
}




XMLNode* 
RoadChunkLS::Save (const synthese::env::RoadChunk* roadChunk)
{
    // ...
    return 0;

}






}
}


