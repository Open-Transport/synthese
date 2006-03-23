#ifndef SYNTHESE_SERVER_REQUESTHANDLER_H
#define SYNTHESE_SERVER_REQUESTHANDLER_H


#include "module.h"

#include <iostream>
#include <string>



namespace synthese
{
namespace server
{

class Request;

/** Request handler.

Once registered into the request dispatcher singleton this class
allows to handle a particular function.

@ingroup m70
*/
class RequestHandler
{
 private:
    
    const std::string _functionCode;

 protected:

    RequestHandler (const std::string& functionCode);
    RequestHandler (const RequestHandler& ref);

 public:

    virtual ~RequestHandler ();

    //! @name Getters/Setters
    //@{
    const std::string& getFunctionCode () const;
    //@}


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
