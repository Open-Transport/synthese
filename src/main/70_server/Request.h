#ifndef SYNTHESE_SERVER_REQUEST_H
#define SYNTHESE_SERVER_REQUEST_H


#include "module.h"


#include <04_time/Date.h>
#include <04_time/DateTime.h>

#include <iostream>
#include <string>
#include <map>

#include <boost/logic/tribool.hpp>


namespace synthese
{
namespace server
{

class Request;

/** Request

This class allows to store a server request and provides easy access to request parameters.
 
Request syntax is quite similar to URL syntax :
 - each parameter is defined by a couple <i>parameterName</i> = <i>parameterValue</i>
 - parameters separator is & character.

In most cases, a request must at least provide following parameters :
 - <i>fct</i> : function name
 - <i>site</i> : site code

@ingroup m70
*/
class Request
{
 private:
    
    static const std::string PARAMETER_SEPARATOR;
    static const std::string PARAMETER_ASSIGNMENT;
    static const int MAX_REQUEST_SIZE;

    std::map <std::string, std::string> _parameters; //!< Request parameters

 public:

    Request ();
    Request (const std::string& requestString);
    ~Request ();

    //! @name Getters/Setters
    //@{
    const std::map<std::string, std::string>& 
	getParameters () const;
    //@}

    //! @name Query methods
    //@{


    /** Tests whether or not a parameter is set in this request.
      @param name Parameter name.
      @return true true if parameter was found, false otherwise.
    */
    bool hasParameter (const std::string& name) const;

    /** Gets a parameter value as string.
      @param name Parameter name.
      @return The string value or the empty string if no such parameter.
    */
    std::string getParameter (const std::string& name) const;

    /** Gets a parameter as tri-state boolean.
      @param name Parameter name.
      @return The parsed boolean.
    */
    boost::logic::tribool getParameterAsTriBool (const std::string& name) const;

    /** Gets a parameter as int value.
      @param name Parameter name.
      @return The parsed int value or UNKNOWN_PARAMETER_VALUE if no such parameter.
    */
    int getParameterAsInt (const std::string& name) const;

    /** Gets a parameter as date.
      @param name Parameter name.
      @return The parsed date.
    */
    synthese::time::Date getParameterAsDate (const std::string& name) const;


    /** Gets a parameter as DateTime object.
      @param name Parameter name.
      @return The parsed DateTime object.
    */
    synthese::time::DateTime getParameterAsDateTime (const std::string& name) const;

    std::string toInternalString () const;

    //@}

    //! @name Update methods
    //@{


    /** Clears request parameters.
    */
    void clearParameters ();

    /** Parses the given string and adds found parameters.
	All the parameters are cleared before.
      @param requestString The string to parse.
    */
    void parseParameters (const std::string& requestString);

    void addParameter (const std::string& name, 
		       const std::string& value);

    void addParameter (const std::string& name, 
		       int value);

    void addParameter (const std::string& name, 
		       synthese::time::Date date);

    void addParameter (const std::string& name, 
		       synthese::time::DateTime dateTime);

    //@}

 private:

    std::string truncateRequestStringIfNeeded (const std::string& requestString) const;

};


}
}



#endif
