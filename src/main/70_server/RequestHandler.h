#ifndef SYNTHESE_SERVER_REQUESTHANDLER_H
#define SYNTHESE_SERVER_REQUESTHANDLER_H


#include "module.h"
#include "01_util/Factory.h"

#include <iostream>
#include <string>



namespace synthese
{
	using synthese::util::Factory;
namespace server
{


class Request;

/** Request handler.

Once registered into the request dispatcher singleton this class
allows to handle a particular function.
Use the generic factory to register.

@ingroup m70
*/
class RequestHandler
{
 protected:

    RequestHandler ();
    RequestHandler (const RequestHandler& ref);

 public:

    virtual ~RequestHandler ();

    //! @name Query methods
    //@{
    virtual void handleRequest (const Request& request, std::ostream& stream) const = 0;
    //@}



    //! @name Update methods
    //@{

    //@}



};


}
}



#endif
