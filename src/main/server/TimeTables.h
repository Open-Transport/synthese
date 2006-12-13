
#ifndef SYNTHESE_TIMETIABLES_H
#define SYNTHESE_TIMETIABLES_H

#include "cSite.h"


/** Timetables collection.
 @ingroup m35
*/
class TimeTables : public cSite
{
    public:
        TimeTables( const std::string& );
        ~TimeTables();
};

#endif

