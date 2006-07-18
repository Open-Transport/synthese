#ifndef SYNTHESE_ENV_CALENDAR_H
#define SYNTHESE_ENV_CALENDAR_H



#include "04_time/Date.h"

#include <boost/dynamic_bitset.hpp>


namespace synthese
{
namespace env
{



/** Calendar.
 @ingroup m15

 Holds a bitset representing year days.
 Each year day can be marked or not.

 The first bit of the internal bitset corresponds to the first
 marked date. The last bit corresponds to the last date marked.
 */
class Calendar 
{
public:


private:

    synthese::time::Date _firstMarkedDate;
    synthese::time::Date _lastMarkedDate;

    boost::dynamic_bitset<> _markedDates;    

public:

    Calendar();
    
    ~Calendar();


    //! @name Getters/Setters
    //@{
    synthese::time::Date getFirstMarkedDate () const;
    synthese::time::Date getLastMarkedDate () const;
    //@}


    //! @name Query methods
    //@{
    int getNbMarkedDates () const;
    bool isMarked (synthese::time::Date date) const;
    //@}



    //! @name Update methods
    //@{
    void mark (synthese::time::Date date, bool state = true);

    //@}

    Calendar& operator&= (const Calendar& op);
    Calendar& operator|= (const Calendar& op);

    static void LogicalAnd (Calendar& dest, const Calendar& op1, const Calendar& op2);
    static void LogicalOr (Calendar& dest, const Calendar& op1, const Calendar& op2);

private:

    void pop_front (int nbBits);
    void pop_back (int nbBits);

    void push_front (int nbBits, bool value = false);
    void push_back (int nbBits, bool value = false);

	void updateFirstMark ();
	void updateLastMark ();

    static synthese::time::Date DateAfter (synthese::time::Date date, unsigned int nbBits);
    static synthese::time::Date DateBefore (synthese::time::Date date, unsigned int nbBits);

    static int NbBitsBetweenDates (synthese::time::Date date1, synthese::time::Date date2);


};


Calendar operator& (const Calendar& op1, const Calendar& op2);
Calendar operator| (const Calendar& op1, const Calendar& op2);



}
}

#endif
