#ifndef SYNTHESE_ENVLSXML_CONTINUOUSSERVICELS_H
#define SYNTHESE_ENVLSXML_CONTINUOUSSERVICELS_H


#include "module.h"
#include <string>

struct XMLNode;


namespace synthese
{
namespace env
{
    class Environment;
    class ContinuousService;
}

namespace envlsxml
{


/** Continuous service XML loading/saving service class.

Sample XML format :

@code
<address id="1"
      name="A1"
      free="true"
      authorized="false"/>
@endcode

 @ingroup m16
*/
class ContinuousServiceLS
{
 public:

    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_SERVICENUMBER, "TEXT", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_SCHEDULES, "TEXT", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_PATHID, "INTEGER", false);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_RANKINPATH, "INTEGER", false);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_RANGE, "INTEGER", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_MAXWAITINGTIME, "INTEGER", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_BIKECOMPLIANCEID, "INTEGER", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_HANDICAPPEDCOMPLIANCEID, "INTEGER", true);
    addTableColumn (CONTINUOUSSERVICES_TABLE_COL_PEDESTRIANCOMPLIANCEID, "INTEGER", true);

    static const std::string CONTINUOUSSERVICE_TAG;
    static const std::string CONTINUOUSSERVICE_ID_ATTR;
    static const std::string CONTINUOUSSERVICE_SERVICENUMBER_ATTR;
    static const std::string CONTINUOUSSERVICE_PATHID_ATTR;
    static const std::string CONTINUOUSSERVICE_DEPARTURESCHEDULE_ATTR;
    static const std::string CONTINUOUSSERVICE__ATTR;
    static const std::string CONTINUOUSSERVICE__ATTR;
    static const std::string CONTINUOUSSERVICE__ATTR;
    static const std::string CONTINUOUSSERVICE__ATTR;
    static const std::string CONTINUOUSSERVICE__ATTR;
    static const std::string CONTINUOUSSERVICE__ATTR;

 private:

    ContinuousServiceLS ();
    ~ContinuousServiceLS();


 public:
    
    
    //! @name Query methods.
    //@{

    static void Load (
	XMLNode& node,
	synthese::env::Environment& environment);

    /** @todo Not implemented.
     **/
    static XMLNode* Save (
	const synthese::env::ContinuousService* continuousService);
    //@}


};



}
}


#endif
