#ifndef SYNTHESE_ENVLSXML_RESERVATIONRULELS_H
#define SYNTHESE_ENVLSXML_RESERVATIONRULELS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class ReservationRule;
}

namespace envlsxml
{


/** Public place XML loading/saving service class.

Sample XML format :

@code
<publicPlace id="1"
             name="Central station"
	     cityId="2"/>
@endcode

 @ingroup m16
*/
class ReservationRuleLS
{
 public:

    static const std::string RESERVATIONRULE_TAG;
    static const std::string RESERVATIONRULE_ID_ATTR;

    static const std::string RESERVATIONRULE_TYPE_ATTR;
    static const std::string RESERVATIONRULE_TYPE_ATTR_IMPOSSIBLE;
    static const std::string RESERVATIONRULE_TYPE_ATTR_OPTIONAL;
    static const std::string RESERVATIONRULE_TYPE_ATTR_COMPULSORY;
    static const std::string RESERVATIONRULE_TYPE_ATTR_ATLEASTONE;

    static const std::string RESERVATIONRULE_ONLINE_ATTR;
    static const std::string RESERVATIONRULE_ORIGINISREFERENCE_ATTR;
    static const std::string RESERVATIONRULE_MINDELAYMINUTES_ATTR;
    static const std::string RESERVATIONRULE_MINDELAYDAYS_ATTR;
    static const std::string RESERVATIONRULE_MAXDELAYDAYS_ATTR;
    static const std::string RESERVATIONRULE_HOURDEADLINE_ATTR;
    static const std::string RESERVATIONRULE_PHONEEXCHANGENUMBER_ATTR;
    static const std::string RESERVATIONRULE_PHONEEXCHANGEOPENINGHOURS_ATTR;
    static const std::string RESERVATIONRULE_DESCRIPTION_ATTR;
    static const std::string RESERVATIONRULE_WEBSITEURL_ATTR;



 private:

    ReservationRuleLS ();
    ~ReservationRuleLS();


 public:
    
    
    //! @name Query methods.
    //@{
    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::ReservationRule* publicPlace);
    //@}


};



}
}


#endif
