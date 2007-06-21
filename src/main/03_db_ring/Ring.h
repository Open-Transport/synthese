#ifndef SYNTHESE_DBRING_RING_H
#define SYNTHESE_DBRING_RING_H


#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <vector>
#include <string>
#include <iostream>



namespace synthese
{



namespace dbring
{



/** Db ring class.
    This class is a link between a node and a ring. 
    It holds all info related to one specific ring, considered from one specific node.

@ingroup m03
*/


    typedef unsigned int RingId;


class Ring 
{
private:

private:
    
    RingId _id;
    NodeInfoMap _infos;               //!< Current infos about each node in this ring 
    boost::posix_time::ptime _timer;  //!< Timer measuring how long since no msg received or sent in this ring

    // TODO : add UpdateSequence here!

 public:

    Ring (const RingId& id);

    ~Ring ();

    
    const RingId& getId () const;

    NodeInfo& getInfo (const NodeId& nodeId);
    UpdateSequence& getUpds ();


    void appendNode (const NodeId& nodeId, const std::string& host, int port);


 protected:

 private:

};




}

}
#endif

