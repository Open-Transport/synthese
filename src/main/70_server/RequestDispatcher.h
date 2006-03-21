#ifndef SYNTHESE_SERVER_REQUESTDISPATCHER_H
#define SYNTHESE_SERVER_REQUESTDISPATCHER_H


#include "module.h"

#include <map>
#include <iostream>
#include <string>



namespace synthese
{
namespace server
{

class Request;
class RequestHandler;


/** Request dispatcher class.

Dispatches requests to proper request handlers according to 
function codes.

@ingroup m70
*/
class RequestDispatcher
{
 private:

    std::map<std::string, const RequestHandler*> _handlers;

 protected:


 public:

    RequestDispatcher ();
    ~RequestDispatcher ();


    //! @name Getters/Setters
    //@{
    //@}


    //! @name Query methods
    //@{
    void dispatchRequest (const Request& request) const;
    //@}



    //! @name Update methods
    //@{
    void registerHandler (const RequestHandler* handler);
    //@}



};


}
}



#endif
