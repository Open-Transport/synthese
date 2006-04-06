
#include <cstdlib>
#include <sstream>
#include <assert.h>


#include "Request.h"

#include "04_time/Date.h"
#include "04_time/DateTime.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>


using namespace std;

using namespace boost::logic;


namespace synthese
{
namespace server
{


const std::string Request::PARAMETER_SEPARATOR ("&");
const std::string Request::PARAMETER_ASSIGNMENT ("=");
const int Request::MAX_REQUEST_SIZE (4096);


Request::Request ()
{
}


Request::Request (const std::string& requestString)
{
    parseParameters(requestString);
}



Request::~Request ()
{
}


void 
Request::clearParameters ()
{
    _parameters.clear ();
}



std::string 
Request::truncateRequestStringIfNeeded (const std::string& requestString) const
{
    std::string s (requestString);

    // Why do we need to filter '+' characters ?
    boost::algorithm::replace_all (s, "+", " ");

    /*
    if (s.size () > MAX_REQUEST_SIZE) {
	bool parameterTruncated = (s.substr (MAX_REQUEST_SIZE, 1) != PARAMETER_SEPARATOR);
	s = s.substr (0, MAX_REQUEST_SIZE);

	// Filter last parameter which if it has been truncated
	if (parameterTruncated) 
	{
	    s = s.substr (0, s.rfind (PARAMETER_SEPARATOR));
	}
    }
    */
    return s;
}



void 
Request::parseParameters (const std::string& requestString)
{
    std::string s (truncateRequestStringIfNeeded (requestString));

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(PARAMETER_SEPARATOR.c_str ());

    tokenizer parametersTokens (s, sep);
    for (tokenizer::iterator parameterToken = parametersTokens.begin();
	 parameterToken != parametersTokens.end (); ++ parameterToken)
    {
	size_t pos = parameterToken->find (PARAMETER_ASSIGNMENT);
	if (pos == string::npos) continue;
	
	std::string parameterName (parameterToken->substr (0, pos));
	std::string parameterValue (parameterToken->substr (pos+1));

	_parameters.insert (make_pair (parameterName, parameterValue));
    }
}





const std::map<std::string, std::string>& 
Request::getParameters () const
{
    return _parameters;
}




std::string 
Request::getFunctionCode () const
{
    return getParameter (PARAMETER_FUNCTION);
}




bool 
Request::hasParameter (const std::string& name) const
{
    return _parameters.find (name) != _parameters.end ();
}



std::string 
Request::getParameter (const std::string& name) const
{
    std::map<std::string, std::string>::const_iterator iter = 
	_parameters.find (name);
    
    if (iter != _parameters.end ())
    {
	return iter->second;
    }
    else
    {
        // TODO throw an exception here !!!
	return std::string ("");
    }
}



synthese::time::Date 
Request::getParameterAsDate (const std::string& name) const
{
    synthese::time::Date date;

    if ( hasParameter (name) )
    {
        date = getParameter (name);
    }
    else
    {
        date.updateDate (synthese::time::TIME_UNKNOWN);
    }

    return date;
}




synthese::time::DateTime 
Request::getParameterAsDateTime (const std::string& name) const
{
    synthese::time::DateTime dateTime;

    if ( hasParameter (name) )
    {
        dateTime = getParameter (name);
    }
    else
    {
        dateTime.updateDateTime (synthese::time::TIME_UNKNOWN);
    }

    return dateTime;
}





tribool
Request::getParameterAsTriBool (const std::string& name) const
{
    if ( hasParameter (name) )
    {
        switch ( atoi( getParameter (name).c_str () ) )
        {
	case 0:
	    return false;
	case -1:
	    return indeterminate;
	default:
	    return true;
        }
    }

    return indeterminate;
}


int 
Request::getParameterAsInt (const std::string& name) const
{
    if ( hasParameter (name) )
    {
	return atoi (getParameter (name).c_str ()); 
    }
    else 
    {
	return UNKNOWN_PARAMETER_VALUE;
    }

}


void 
Request::addParameter (const std::string& name, 
		       const std::string& value)
{
    _parameters.insert (std::make_pair (name, value));
}

void 
Request::addParameter (const std::string& name, 
		       int value)
{
    stringstream ss;
    ss << value;
    addParameter (name, ss.str ());
}


void 
Request::addParameter (const std::string& name, 
		       synthese::time::Date date)
{
    addParameter (name, date.toInternalString ());
}



void 
Request::addParameter (const std::string& name, 
		       synthese::time::DateTime dateTime)
{
    addParameter (name, dateTime.toInternalString ());
}




std::string 
Request::toInternalString () const
{
    std::stringstream ss;
    
    for ( map<string, string>::const_iterator iter = _parameters.begin(); 
	  iter != _parameters.end(); 
	  ++iter )
    {
	if (iter != _parameters.begin ()) ss << "&";
	ss << iter->first << "=" << iter->second;
    }

    return truncateRequestStringIfNeeded(ss.str());
}












}
}
