#ifndef SYNTHESE_ENVLSXML_ALARMLS_H
#define SYNTHESE_ENVLSXML_ALARMLS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class Alarm;
}

namespace envlsxml
{


/** Alarm XML loading/saving service class.

Sample XML format :

@code
<alarm id="1"
      name="A1"
      free="true"
      authorized="false"/>
@endcode

 @ingroup m16
*/
class AlarmLS
{
 public:

    static const std::string ALARM_TAG;
    static const std::string ALARM_ID_ATTR;
    static const std::string ALARM_MESSAGE_ATTR;
    static const std::string ALARM_PERIODSTART_ATTR;
    static const std::string ALARM_PERIODEND_ATTR;

    static const std::string ALARM_LEVEL_ATTR;
    static const std::string ALARM_LEVEL_ATTR_INFO;
    static const std::string ALARM_LEVEL_ATTR_WARNING;



 private:

    AlarmLS ();
    ~AlarmLS();


 public:
    
    
    //! @name Query methods.
    //@{
    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::Alarm* alarm);
    //@}


};



}
}


#endif

