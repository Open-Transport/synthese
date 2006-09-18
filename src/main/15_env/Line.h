#ifndef SYNTHESE_ENV_LINE_H
#define SYNTHESE_ENV_LINE_H


#include <vector>
#include <string>

#include "01_util/RGBColor.h"
#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "Path.h"



namespace synthese
{
namespace env
{

    class Axis;
    class LineStop;
    class RollingStock;
    class Service;
    class TransportNetwork;


/** Line class.
 @ingroup m15
*/
class Line : 
    public synthese::util::Registrable<uid,Line>,
    public Path
{
private:
    TransportNetwork* _network; 
    const Axis* _axis; 
    
    uid _rollingStockId;
    
    std::string _name;  //!< Name (id)
    std::string _shortName;  //!< Name (cartouche)
    std::string _longName; //!< Name for schedule card

    synthese::util::RGBColor _color;  //!< Line color
    std::string _style;    //!< CSS style (cartouche)
    std::string _image;    //!< Display image (cartouche)

    std::string _timetableName; //!< Name for timetable
    std::string _direction;   //!< Direction (shown on vehicles)

    bool _isWalkingLine;
    
    bool _useInDepartureBoards; 
    bool _useInTimetables; 
    bool _useInRoutePlanning; 


public:

    Line (const uid& id,
	  const std::string& name, 
	  const Axis* axis);

    virtual ~Line();



    //! @name Getters/Setters
    //@{
    const uid& getId () const;

    const std::string& getName () const;
    void setName (const std::string& name);

    bool getUseInDepartureBoards () const;
    void setUseInDepartureBoards (bool useInDepartureBoards);

    bool getUseInTimetables () const;
    void setUseInTimetables (bool useInTimetables);

    bool getUseInRoutePlanning () const;
    void setUseInRoutePlanning (bool useInRoutePlanning);

    const Axis* getAxis () const;

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
    
    const uid& getRollingStockId () const;
    void setRollingStockId (const uid& id);

    bool getWalkingLine () const;
    void setWalkingLine (bool isWalkingLine);
    
    //@}




    //! @name Update methods
    //@{
    
    //! @name Query methods
    //@{

    bool isRoad () const;
    bool isLine () const;

    bool isReservable () const;

    //@}
    
    
};


}
}


#endif
