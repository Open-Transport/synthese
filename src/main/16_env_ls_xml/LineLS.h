#ifndef SYNTHESE_ENVLSXML_LINELS_H
#define SYNTHESE_ENVLSXML_LINELS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class Line;
}

namespace envlsxml
{


/** Line XML loading/saving service class.

Sample XML format :

@code
<line id="5"
      name="L5"
      axisId="A2"
      color="red">

   <lineStop id="1" metricOffset="0"
             type="departure" physicalStopId="1">
     <point x="120.5" y="4444.2"/>
     <point x="130.5" y="4434.4"/>
     <point x="140.2" y="4414.2"/>
   </lineStop>

   <lineStop id="2" metricOffset="100"
             type="passage" physicalStopId="2"/>

</line>
@endcode

 @ingroup m16
*/
class LineLS
{
 public:

    static const std::string LINE_TAG;
    static const std::string LINE_ID_ATTR;
    static const std::string LINE_TRANSPORTNETWORKID_ATTR;
    static const std::string LINE_AXISID_ATTR;
    static const std::string LINE_NAME_ATTR;
    static const std::string LINE_SHORTNAME_ATTR;
    static const std::string LINE_LONGNAME_ATTR;
    static const std::string LINE_COLOR_ATTR;
    static const std::string LINE_STYLE_ATTR;
    static const std::string LINE_IMAGE_ATTR;
    static const std::string LINE_TIMETABLENAME_ATTR;
    static const std::string LINE_DIRECTION_ATTR;
    static const std::string LINE_ISWALKINGLINE_ATTR;
    static const std::string LINE_USEINDEPARTUREBOARDS_ATTR;
    static const std::string LINE_USEINTIMETABLES_ATTR;
    static const std::string LINE_USEINROUTEPLANNING_ATTR;
    static const std::string LINE_ROLLINGSTOCKID_ATTR;
    static const std::string LINE_FAREID_ATTR;
    static const std::string LINE_ALARMID_ATTR;
    static const std::string LINE_BIKECOMPLIANCEID_ATTR;
    static const std::string LINE_HANDICAPPEDCOMPLIANCEID_ATTR;
    static const std::string LINE_PEDESTRIANCOMPLIANCEID_ATTR;
    static const std::string LINE_RESERVATIONRULEID_ATTR;

 private:

    LineLS ();
    ~LineLS();


 public:
    
    
    //! @name Query methods.
    //@{

    /** Loads a line from an XML DOM node.
	Conversion from ids to pointers is done thanks to the
	environment parameter.
     */
    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::Line* line);
    //@}


};



}
}


#endif

