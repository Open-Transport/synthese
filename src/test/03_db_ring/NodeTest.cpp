#include "03_db_ring/Node.h"

#include "01_util/Thread.h"

#include "00_tcp/TcpService.h"


#include <boost/test/auto_unit_test.hpp>

using namespace synthese::tcp;
using namespace synthese::util;
using namespace synthese::dbring;


BOOST_AUTO_TEST_CASE (testNodeStateCycle)
{
    
    // node0 alone out of ring.
    // initially, its ring table contains only its own info.
    Node node0 (0, "localhost", 9990);
    node0.loop ();
    BOOST_REQUIRE_EQUAL (OUTRING, node0.getInfo ().state );

    // node1 alone out of ring
    // initially, its ring table contains only its own info.
    Node node1 (1, "localhost", 9991);
    node1.loop ();
    BOOST_REQUIRE_EQUAL (OUTRING, node1.getInfo ().state );
    
    // at this stage, node0 does not know node1 and vice-versa.
    // we now update one of the nodes ring table (manual action)
    // node1 ring table is updated with node0 info (id, address, port).
    node1.appendNode (0, "localhost", 9990);

    // since node1 was OUTRING, it immediately sends a token message to node0
    // the sent token holds a snapshot of node1 local ring table.
    node1.loop ();
    BOOST_REQUIRE_EQUAL (ENTRING, node1.getInfo ().state );



    // node0 has received its first token message. it merges it with its own local
    // ring table and sends token message to its next successor (ie node1).
    node0.loop ();
    BOOST_REQUIRE_EQUAL (ENTRING, node0.getInfo ().state );

    // node1 has received back the token; since its status says entering ring,
    // it means that at least one other node is aware of its existence in ring.
    // Thus, node1 is now inside ring
    node1.loops ();
    BOOST_REQUIRE_EQUAL (INSRING, node1.getInfo ().state );

    // idem for node0
    node0.loops ();
    BOOST_REQUIRE_EQUAL (INSRING, node0.getInfo ().state );

    // A third node comes into play
    Node node2 (2, "localhost", 9992);
    node2.loops ();
    BOOST_REQUIRE_EQUAL (OUTRING, node2.getInfo ().state );

    // token is now on node1, another round to set token on node0
    node1.loops ();

    // appends node1 on node2
    // this should always be done this way : a insider should be added to the outsider
    node2.appendNode (1, "localhost", 9991);

    node2.loops ();
    BOOST_REQUIRE_EQUAL (ENTRING, node2.getInfo ().state );
    
    node1.loops ();
    // node1 is now aware of node2; however there is already a token
    // running. so the info is not forwarded immediateley.
    BOOST_REQUIRE_EQUAL (ENTRING, node2.getInfo ().state );

    node2.loops ();
    BOOST_REQUIRE_EQUAL (ENTRING, node2.getInfo ().state );
    
    // we forgot the token on node0... another round to make it pass through node2
    node0.loops ();
    node1.loops ();
    node2.loops ();

    //  everything should be up to date
    BOOST_REQUIRE_EQUAL (INSRING, node0.getInfo ().state );
    BOOST_REQUIRE_EQUAL (INSRING, node1.getInfo ().state );
    BOOST_REQUIRE_EQUAL (INSRING, node2.getInfo ().state );

    

    
}




class SickNode : public Node
{
private:

    bool _timeout;

    bool _dumb;
    bool _deaf;

public:

    SickNode (const NodeId& id, const std::string& host, int port, bool dumb, bool deaf)
	: Node (id, host, port)
	, _timeout (false)
	, _dumb (dumb)
	, _deaf (deaf)
    {
    }

    void setTimeout (bool timeout)
    {
	_timeout = timeout;
    }


    void setDumb (bool dumb) { _dumb = dumb; }

    void setDeaf (bool deaf) 
    { 
	if (deaf) 
	{
	    TcpService::closeService (getInfo ().port);
	    _tcpService = 0;
	}
	else if (_tcpService == 0)
	{
	    _tcpService = TcpService::openService (getInfo ().port);
	}
	_deaf = deaf; 
    }

 protected:

    bool timeout () const
    {
	return _timeout || _deaf;
    }

    bool recvToken (Token* token)
    {
	if (_deaf) return false;
	return Node::recvToken (token);
    }


    bool sendToken (const Token* token)
    {
	if (_dumb) return false;
	return Node::sendToken (token);
    }
};



/*
BOOST_AUTO_TEST_CASE (testNodeStateCycleWithSickNodes)
{
    SickNode node0 (0, "localhost", 9990, false, false);
    SickNode node1 (1, "localhost", 9991, false, false);
    SickNode node2 (2, "localhost", 9992, false, false);
    
    node0.appendNode (1, "localhost", 9991);
    node0.appendNode (2, "localhost", 9992);

    // everybody send ...
    node0.loops ();
    node1.loops ();
    node2.loops ();

    // ... everybody receive back
    node0.loops ();
    node1.loops ();
    node2.loops ();

    // the inital ring should be formed, the token is on node0
    BOOST_REQUIRE_EQUAL (INSRING, node0.getInfo ().state );
    BOOST_REQUIRE_EQUAL (INSRING, node1.getInfo ().state );
    BOOST_REQUIRE_EQUAL (INSRING, node2.getInfo ().state );

    // node1, next up successor of node0, is sick
    node1.setDeaf (true);
    node1.setDumb (true);

    node0.loop (); // just one loop !!

    // node0 should have detected node1 was now OUTRING
    node1.loops (); // node1 should not disturb anything...
    BOOST_REQUIRE_EQUAL (OUTRING, node1.getInfo ().state );
    
    node2.loop (); // just one loop !! 
    node1.loops (); // node1 should not disturb anything...
    BOOST_REQUIRE_EQUAL (OUTRING, node1.getInfo ().state );

    node0.loop (); // just one loop !! 
    node2.loop (); // just one loop !! 

    node1.loops (); // node1 should not disturb anything...
    BOOST_REQUIRE_EQUAL (OUTRING, node1.getInfo ().state );

    node0.loop (); // just one loop !! 

    node2.loop (); // just one loop !! 

    // OK the token is still being transmitted (now on node0)

    node1.setDeaf (false);
    node1.setDumb (false);


    // node1 has been cured. it should send again an init token
    node1.loop (); 
    BOOST_REQUIRE_EQUAL (ENTRING, node1.getInfo ().state );
    
    // node2 receives now node1 init and does not forward it (init token)
    node2.loop (); 
    
    // node2 is now aware that node1 is ENTRING back
    BOOST_REQUIRE_EQUAL (INSRING, node2.getInfo (0).state );
    BOOST_REQUIRE_EQUAL (ENTRING, node2.getInfo (1).state );
    BOOST_REQUIRE_EQUAL (INSRING, node2.getInfo (2).state );
    
    node2.loops ();
    node0.loops ();
    node1.loops ();
    node2.loops ();
    node0.loops ();
    
    // Everybody inside ring again
    BOOST_REQUIRE_EQUAL (INSRING, node0.getInfo (0).state );
    BOOST_REQUIRE_EQUAL (INSRING, node0.getInfo (1).state );
    BOOST_REQUIRE_EQUAL (INSRING, node0.getInfo (2).state );
    BOOST_REQUIRE_EQUAL (INSRING, node1.getInfo (0).state );
    BOOST_REQUIRE_EQUAL (INSRING, node1.getInfo (1).state );
    BOOST_REQUIRE_EQUAL (INSRING, node1.getInfo (2).state );
    BOOST_REQUIRE_EQUAL (INSRING, node2.getInfo (0).state );
    BOOST_REQUIRE_EQUAL (INSRING, node2.getInfo (1).state );
    BOOST_REQUIRE_EQUAL (INSRING, node2.getInfo (2).state );



}




BOOST_AUTO_TEST_CASE (testTokenRegeneration)
{
    SickNode node0 (0, "localhost", 9990, false, false);
    SickNode node1 (1, "localhost", 9991, false, false);
    SickNode node2 (2, "localhost", 9992, false, false);
    
    node0.appendNode (1, "localhost", 9991);
    node0.appendNode (2, "localhost", 9992);

    // everybody send ...
    node0.loops ();
    node1.loops ();
    node2.loops ();

    // ... everybody receive back
    node0.loops ();
    node1.loops ();
    node2.loops ();

    // the inital ring should be formed, the token is on node0
    BOOST_REQUIRE_EQUAL (INSRING, node0.getInfo ().state );
    BOOST_REQUIRE_EQUAL (INSRING, node1.getInfo ().state );
    BOOST_REQUIRE_EQUAL (INSRING, node2.getInfo ().state );


    // node0, is sick
    node0.setDumb (true);

    node0.loop (); 

    node0.setDeaf (true);
    // token is lost!

    node0.loops ();
    node1.loops ();
    node2.loops ();

    // Both node1 and node2 are in recv timeout
    node1.setTimeout (true);
    node2.setTimeout (true);

    node1.loop ();
    node1.setTimeout (false);

    // node1 should have sent again an init token.
    node2.loop ();
    node2.setTimeout (false);

    node1.loop ();

    node2.loop ();

    // Check there is only one token
    node2.loop ();

    node1.loop ();

    // The cycle is ok again, without node0

    node0.setDumb (false);
    node0.setDeaf (false);

    // node0 is cured
    
    node0.setTimeout (true);
    node0.loop ();
    node0.setTimeout (false);
    
    node1.loop ();
    node2.loop ();
    node0.loop ();
    node1.loop ();
    node2.loop ();
    node0.loop ();
    
    // back to inital ring state 
    BOOST_REQUIRE_EQUAL (INSRING, node0.getInfo ().state );
    BOOST_REQUIRE_EQUAL (INSRING, node1.getInfo ().state );
    BOOST_REQUIRE_EQUAL (INSRING, node2.getInfo ().state );

    
}


*/


