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
    const Line* _line;      //!< Parent line

    const double _metricOffset;      //!< Metric offset of stop on line
    const bool _scheduleInput; //!< Schedule with or without input

    synthese::time::Schedule* _departureBeginSchedule;  //!< First departure schedule
    synthese::time::Schedule* _departureEndSchedule;  //!< Last departure schedule
    synthese::time::Schedule* _arrivalBeginSchedule; //!< First arrival schedule
    synthese::time::Schedule* _arrivalEndSchedule;  //!< Last arrival schedule

    int _departureIndex[24];     //!< First line service index by departure hour of day
    int _arrivalIndex[24];  //!< First line service index by arrival hour of day


public:


    LineStop (const uid& id,
		  const Line* line,
	      double metricOffset,
	      const EdgeType& type,
	      const PhysicalStop* physicalStop,
	      bool scheduleInput);

    ~LineStop();


    //! @name Getters/Setters
    //@{
    const Path* getParentPath () const;
    const Vertex* getFromVertex () const;



    const synthese::time::Schedule& 
	getDepartureBeginSchedule (int serviceNumber) const;

    const synthese::time::Schedule& 
	getDepartureEndSchedule (int serviceNumber) const;

    const synthese::time::Schedule& 
	getArrivalBeginSchedule (int serviceNumber) const;

    const synthese::time::Schedule& 
	getArrivalEndSchedule (int serviceNumber) const;

    
    bool getScheduleInput () const;
    double getMetricOffset () const;
    

    //@}


    //! @name Query methods
    //@{
    bool isRunning( const synthese::time::DateTime& startMoment, 
		    const synthese::time::DateTime& endMoment ) const;


    void calculateArrival (const LineStop& departureLineStop, 
			    int serviceNumber,
			    const synthese::time::DateTime& departureMoment, 
			    synthese::time::DateTime& arrivalMoment ) const;

    void calculateDeparture (const LineStop& arrivalLineStop, 
			     int serviceNumber,
			     const synthese::time::DateTime& arrivalMoment, 
			     synthese::time::DateTime& departureMoment ) const;

    int getBestRunTime (const LineStop& other ) const;
    
    /** Checks consistency of input schedules.
      @param lineStopWithPreviousSchedule Previous line stop with schedule
      @return true if no problem detected, false otherwise
    */
    bool checkSchedule (const LineStop* lineStopWithPreviousSchedule ) const;
    
    /*! Estimates consistency of line stops sequence according to 
      metric offsets and physical stops coordinates.
      @param other Other line stop to compare.
      @return true if data seems consistent, false otherwise.
    */
    bool seemsGeographicallyConsistent (const LineStop& other) const;


    /** Provides next departure service number (method 1)
	@param departureMoment Presence hour at departure place
	@param maxDepartureMoment Maximum departure hour
	@param calculationMoment Calculation moment for reservation delay checking
	@return Found service index or -1 if none was found.
	@retval departureMoment Accurate departure moment. Meaningless if -1 returned.
	0 means scheduled service.
    */
    int getNextService (synthese::time::DateTime& departureMoment, 
			const synthese::time::DateTime& maxDepartureMoment,
			const synthese::time::DateTime& calculationMoment,
			int minNextServiceNumber = UNKNOWN_VALUE ) const;

    
    /** Provides next departure service number (method 2)
	@param departureMoment Presence hour at departure place
	@param maxDepartureMoment Maximum departure hour
	@param continuousServiceAmplitude 
	@param minNextServiceNumber Index to start service search from
	@param calculationMoment Calculation moment for reservation delay checking
	@return Found service index or -1 if none was found.
	@retval departureMoment Accurate departure moment. Meaningless if -1 returned.
	@retval continuousServiceAmplitude Continuous service amplitude. 0 means scheduled service.
    */
    int getNextService ( synthese::time::DateTime& departureMoment, 
			 const synthese::time::DateTime& maxDepartureMoment,
			 int& continuousServiceAmplitude, 
			 int minNextServiceNumber,
			 const synthese::time::DateTime& calculationMoment ) const;
 

    int getPreviousService ( synthese::time::DateTime& arrivalMoment, 
			     const synthese::time::DateTime& minArrivalMoment,
			     int continuousServiceAmplitude ) const;
    
    int getPreviousService ( synthese::time::DateTime& arrivalMoment, 
			     const synthese::time::DateTime& minArrivalMoment ) const;
    

    //@}





    //! @name Update methods
    //@{
 
    
    void updateDepartureIndex();

    void updateArrivalIndex();



    /** Fills schedules from buffer.
      @param buffer Buffer to parse
      @param position First character to parse from.
      @param columnWidth Number of characters between each schedule including data
      @param departurePassageDifferent Indicates if the function is called 
      a second time to describe departure schedules after having described arrival schedules.
      
      Use of departurePassageDifferent prevents overwriting arrival schedules 
      in case departure schedules are provided on another text line.
    */
    void setSchedules ( const std::string& buffer, 
			int position, 
			int columnWidth,
			bool departurePassageDifferent );

    //@}


private:

    /** Allocates memory for holding schedules.
     */
    void allocateSchedules (); 




};



}
}




#endif
