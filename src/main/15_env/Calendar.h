#ifndef SYNTHESE_ENV_CALENDAR_H
#define SYNTHESE_ENV_CALENDAR_H


#include "Registrable.h"


#include "04_time/Year.h"
#include "04_time/Date.h"
#include <string>
#include <vector>


namespace synthese
{
namespace env
{



/** Calendar.
 @ingroup m15

 32 bits stored in an int. 
 The lowest bit is day one; the highest bit is day 32 (unused).
 */
class Calendar : public Registrable<int,Calendar>
{
public:

    /** Type of inclusion */
    typedef enum
    {
	INCLUSION_TYPE_POSITIVE = '+',
	INCLUSION_TYPE_NEGATIVE = '-'
    } InclusionType;

    /** Mask */
    typedef unsigned long Mask;

    /** Calendar */
    typedef std::vector<Mask> MonthesMask;

    /** Category */
    typedef unsigned char Category;
    
    /** Max category */
    static const Category CATEGORY_MAX;

private:

    std::string _name;

    std::vector<Mask> _yearDays; //!< Array containing masks (one per month)
    const synthese::time::Year _firstYear;  //!< Actual year for mask first byte
    const synthese::time::Year _lastYear;  //!< Actual year for mask last byte

    Category _category;
	
public:

    Calendar( const int& id,
			 int firstYear, 
			 int lastYear, 
			 const std::string& name,
			 const Category& category = CATEGORY_MAX);
    
    ~Calendar();


    //! @name Getters/Setters
    //@{
    const Category& getCategory () const;
    void setCategory (Category category);

    const std::vector<Mask>& getYearDays () const;
    void setYearDays (const std::vector<Mask>& yearDays);

    void setName (const std::string& name);
    //@}


    //! @name Query methods
    //@{
    synthese::time::Date getFirstOperationDay () const;

    
    void setInclusionToMask ( std::vector<Mask>& calendar, 
			      InclusionType type = INCLUSION_TYPE_POSITIVE ) const;

    void setInclusionToMask ( Calendar& calendar, 
			      InclusionType type = INCLUSION_TYPE_POSITIVE ) const;

    bool sharesAllElements ( const Calendar& base, const std::vector<Mask>& other) const; 
    bool sharesOneElement ( const std::vector<Mask>& other) const;
    bool sharesOneElement ( const Calendar& other) const;

    int card ( const std::vector<Mask>& ) const; 
    int card ( const Calendar& ) const; 

    /** Is a given date marked ?
	@param date Reference day.
    */
    bool isMarked ( const synthese::time::Date& date) const;

    std::vector<Mask> excludedElements ( const Calendar& other ) const;
    std::vector<Mask> logicalAnd ( const Calendar& ) const;

    //@}



    //! @name Update methods
    //@{
    void reset ( bool value = false );

    /** Modification of one date status in calendar.
	@param date Date to modify.
	@param type Date status : running or not.
	@return true if the given dates exists in this calendar, false otherwise.
    */
    bool setMark ( const synthese::time::Date& date, 
		     InclusionType type = INCLUSION_TYPE_POSITIVE );
    
    bool setMark ( const synthese::time::Date& startDate, 
		      const synthese::time::Date& endDate, 
		      InclusionType type = INCLUSION_TYPE_POSITIVE, 
		      int step = 1 );

    //@}


private:

    void doSetMark ( const synthese::time::Date& date, 
		     InclusionType type);

    /** Calculates index of the bit array to read for a given date
	@param date Any day in the month to read.
	@return The bit array index.
    */
    int getMonthIndex ( const synthese::time::Date& date ) const;


    /** Calculates index of the bit array to read for a given date
	@param year Year to read.
	@param month Month to read.
	@return The bit array index.
    */
    int getMonthIndex ( int year, int month ) const;


};



}
}

#endif
