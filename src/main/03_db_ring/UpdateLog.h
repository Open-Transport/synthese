#ifndef SYNTHESE_DBRING_UPDATELOG_H
#define SYNTHESE_DBRING_UPDATELOG_H

#include "01_util/UId.h"
#include "03_db_ring/UpdateRecord.h"

#include <map>
#include <set>
#include <string>

#include <iostream>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>




namespace synthese
{



namespace dbring
{


    typedef boost::shared_ptr<UpdateRecord> UpdateRecordSPtr;

    struct updateRecordCmp
    {
	bool operator()(const UpdateRecordSPtr& op1, const UpdateRecordSPtr& op2) const
	{
	    return op1->getTimestamp () < op2->getTimestamp ();
	}
    };


    typedef std::map<uid, UpdateRecordSPtr> UpdateRecordMap;
    typedef std::set<UpdateRecordSPtr, updateRecordCmp> UpdateRecordSet;

class UpdateLog
{

private:
    
    UpdateRecordMap _map;    //!< Update records mapped by uid.
    UpdateRecordSet _set;         //!< Update records sorted by timestamp.      

    boost::shared_ptr<boost::recursive_mutex> _updateMutex; 

public:

    UpdateLog ();
    ~UpdateLog ();

    bool hasUpdateRecord (const uid& key) const;
    const UpdateRecordSPtr& getUpdateRecord (const uid& key) const;
    UpdateRecordSPtr& getUpdateRecord (const uid& key);
    void setUpdateRecord (const UpdateRecordSPtr& updateRecord);

    const UpdateRecordSet& getUpdateRecords () const { return _set; }
    UpdateRecordSet& getUpdateRecords () { return _set; }

    void flush ();

private:

    friend std::ostream& operator<< ( std::ostream& os, const UpdateLog& op );
    friend std::istream& operator>> ( std::istream& is, UpdateLog& op );

};


typedef boost::shared_ptr<UpdateLog> UpdateLogSPtr;


std::ostream& operator<< ( std::ostream& os, const UpdateLog& op );
std::istream& operator>> ( std::istream& is, UpdateLog& op );


}
}


#endif
