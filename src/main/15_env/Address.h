#ifndef SYNTHESE_ENV_ADDRESS_H
#define SYNTHESE_ENV_ADDRESS_H


#include <vector>
#include <set>
#include "Gateway.h"



namespace synthese
{
namespace env
{


class LogicalPlace;
class Road;



/** Adress (road + metric offset)
 An address is a position on a road given a metric offset from the start of the road.
*/
class Address : public Gateway
{
private:

    const Road* _road;    //!< Address road.
    double _metricOffset;  //!< Metric offset (meters)

public:

    Address (int rank,
	     const std::string& name,
	     const LogicalPlace* logicalPlace,
	     const Road* road, 
	     double metricOffset);

    ~Address();


    //! @name Getters/Setters
    //@{
    const Road* getRoad() const;
    double getMetricOffset () const;
    //@}




};

}
}

#endif
