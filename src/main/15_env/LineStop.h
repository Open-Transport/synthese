#ifndef SYNTHESE_ENV_LINESTOP_H
#define SYNTHESE_ENV_LINESTOP_H


#include <string>
#include "module.h"
#include "Edge.h"

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "04_time/DateTime.h"
#include "04_time/Schedule.h"


namespace synthese
{
namespace env

{

class PhysicalStop;
class Line;



/** Association class between line and physical stop.
 @ingroup m15
*/
 class LineStop : 
     public synthese::util::Registrable<uid,LineStop>, public Edge
{
public:
    
private:

    const PhysicalStop*  _physicalStop;   //!< Physical stop

    double _metricOffset;      //!< Metric offset of stop on line


public:


    LineStop (const uid& id,
	      const Line* line,
	      int rankInPath,
	      bool isDeparture,
	      bool isArrival,
	      double metricOffset,
	      const PhysicalStop* physicalStop);

    ~LineStop();


    //! @name Getters/Setters
    //@{
    const Vertex* getFromVertex () const;


    double getMetricOffset () const;
    void setMetricOffset (double metricOffset);
    

    //@}


    //! @name Query methods
    //@{

    /*! Estimates consistency of line stops sequence according to 
      metric offsets and physical stops coordinates.
      @param other Other line stop to compare.
      @return true if data seems consistent, false otherwise.
    */
    bool seemsGeographicallyConsistent (const LineStop& other) const;


    //@}





    //! @name Update methods
    //@{
 
    


    //@}


private:





};



}
}




#endif
