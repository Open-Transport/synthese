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

    static RequestDispatcher* _instance;

    std::map<std::string, const RequestHandler*> _handlers;

    RequestDispatcher ();

 protected:


 public:

    ~RequestDispatcher ();


    //! @name Getters/Setters
    //@{
    static RequestDispatcher* getInstance ();
    //@}


    //! @name Query methods
    //@{
    void dispatchRequest (const Request& request, std::ostream& stream) const;
    //@}



    //! @name Update methods
    //@{
    void registerHandler (const RequestHandler* handler);
    //@}



};


}
}



#endif
