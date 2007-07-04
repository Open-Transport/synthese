#ifndef SYNTHESE_ENV_TRIDENTEXPORT_H
#define SYNTHESE_ENV_TRIDENTEXPORT_H

#include "01_util/UId.h"


#include <iostream>


namespace synthese
{


namespace env
{

    class Service;


/** Trident/Chouette export.
 */
    
class TridentExport
{
public:

    
private:
    
    TridentExport ();
    ~TridentExport();

    
public:

    /** -> ChouettePTNetwork
     */
    static void Export (std::ostream& os, 
			const uid& commercialLineId);

private:
    
    static std::string TridentId (const std::string& peer, const std::string clazz, const uid& id);
    static std::string TridentId (const std::string& peer, const std::string clazz, const std::string& s);
    static std::string TridentId (const std::string& peer, const std::string clazz, const Service* srv);


};







}
}


#endif
