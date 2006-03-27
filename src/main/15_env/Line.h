#ifndef SYNTHESE_ENV_LINE_H
#define SYNTHESE_ENV_LINE_H


#include <vector>
#include <string>

#include "01_util/RGBColor.h"
#include "Registrable.h"
#include "Calendar.h"
#include "Path.h"



namespace synthese
{
namespace env
{

    class Alarm;
    class Axis;
    class Fare;
    class LineStop;
    class Point;
    class RollingStock;
    class Service;
    class TransportNetwork;


/** Line class.
 @ingroup m15
*/
class Line : 
    public Registrable<std::string,Line>,
    public Path
{
private:
    TransportNetwork* _network; 
    const Axis* _axis; 
    
    std::vector<LineStop*> _lineStops; 
    RollingStock* _rollingStock;
    
    Fare* _fare;
    Alarm* _alarm;
    
    std::string _shortName;  //!< Name (cartouche)
    std::string _longName; //!< Name for schedule card

    synthese::util::RGBColor _color;  //!< Line color
    std::string _style;    //!< CSS style (cartouche)
    std::string _image;    //!< Display image (cartouche)

    std::string _timetableName; //!< Name for timetable
    std::string _direction;   //!< Direction (shown on vehicles)

    bool _isWalkingLine;
    
    bool _departureBoardDisplay; 
    bool _timetableDisplay; 
    Calendar _calendar; 

	

public:

    Line (const std::string& id, 
	  const Axis* axis,
	  int firstYear, 
	  int lastYear);

    virtual ~Line();



    //! @name Getters/Setters
    //@{
    int getEdgesCount () const;
    const Edge* getEdge (int index) const;

    bool getDepartureBoardDisplay () const;
    void setDepartureBoardDisplay (bool departureBoardDisplay);

    bool getTimetableDisplay () const;
    void setTimetableDisplay (bool timetableDisplay);

    const Axis* getAxis () const;

    const Alarm* getAlarm() const;

    const synthese::util::RGBColor& getColor () const;
    void setColor (const synthese::util::RGBColor& color);

    const std::string& getDirection () const;
    void setDirection (const std::string& direction);

    const std::string& getImage () const;
    void setImage (const std::string& image);

    const std::string& getShortName () const;
    void setShortName (const std::string& shortName);

    const std::string& getLongName () const;
    void setLongName (const std::string& longName);

    const std::string& getTimetableName () const;
    void setTimetableName (const std::string& timetableName);

    const TransportNetwork* getNetwork () const;
    void setNetwork (TransportNetwork* network);
    
    const std::string& getStyle () const;
    void setStyle (const std::string& style);
    
    const Fare* getFare () const;
    void setFare (Fare* fare);

    const RollingStock* getRollingStock () const;
    void setRollingStock (RollingStock* rollingStock);

    const std::vector<LineStop*>& getLineStops() const;

    bool getWalkingLine () const;
    void setWalkingLine (bool isWalkingLine);
    
    //@}




    //! @name Update methods
    //@{
    void postInit ();

    void addLineStop (LineStop* lineStop);

    /** Updates line calendar.

    The generated calendar indicates whether or not a day contains at least one service.
    It takes into account services running after midnight : if at least one minute
    of a day is concerned by a service, then the whole day is selected.

    Thus, if a calculation request is done on a deselected calendar day, the line 
    can safely be filtered.
    */
    void updateCalendar ();
    //@}

    
    //! @name Query methods
    //@{
    /** Gets all the geographical points linked by the line
        between two of its line stops. If no from/to line stop
	index is provided, all the stops are considered.
	@param fromLineStopIndex 
	@param toLineStopIndex 

	This includes :
	- physical stops
	- via points
    */
    std::vector<const Point*> getPoints (int fromLineStopIndex = 0,
					 int toLineStopIndex = -1) const;

    bool isReservable () const;

    bool isInService (const synthese::time::Date& date) const;
    //@}
    
    
};


}
}


#endif
