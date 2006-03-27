#include "Calendar.h"



namespace synthese
{
namespace env
{

const Calendar::Category Calendar::CATEGORY_MAX (255);



    Calendar::Calendar( const int& id,
			int firstYear, 
			int lastYear,
			const std::string& name,
			const Category& category)
    : Registrable<int,Calendar> (id)
    , _name (name)
    , _firstYear (firstYear)
    , _lastYear (lastYear)
    , _category (category)
{
    
}
    

Calendar::~Calendar()
{

}





const Calendar::Category& 
Calendar::getCategory () const
{
    return _category;
}




void 
Calendar::setCategory (Category category)
{
    _category = category;
}




const std::vector<Calendar::Mask>& 
Calendar::getYearDays () const
{
    return _yearDays;
}



void 
Calendar::setYearDays ( const std::vector<Calendar::Mask>& yearDays )
{
    for ( int i = 0; i < ( _lastYear.getValue () - _firstYear.getValue () + 1 ) * 
	      synthese::time::MONTHS_PER_YEAR ; i++ )
    {
        _yearDays[i] = yearDays[i];
    }
}





void 
Calendar::setName (const std::string& name)
{
    _name = name;
}




bool 
Calendar::setMark ( const synthese::time::Date& date, 
			      InclusionType type)
{
    if ( date.isValid () )
    {
        if ( date.isYearUnknown() )
        {
            synthese::time::Date currentDate;
            for ( synthese::time::Year Annee = _firstYear; 
		  Annee.getValue () <= _lastYear.getValue (); Annee++ )
            {
                currentDate.updateDate( date.getDay(), date.getMonth (), Annee.getValue () );
                doSetMark ( currentDate, type );
            }
        }
        else if ( date.getYear () >= _firstYear.getValue () && 
		  date.getYear () <= _lastYear.getValue () )
	{
	    doSetMark ( date, type );
	}

        return true;
    }
    else
    {
        return false;
    }
}



void 
Calendar::doSetMark ( const synthese::time::Date& date, 
				InclusionType type )
{
    Mask mask = 1;
    mask <<= ( date.getDay() - 1 );

    if ( type == INCLUSION_TYPE_POSITIVE )
    {
        _yearDays[ getMonthIndex( date ) ] |= mask;

    }
    else
    {
        mask = ~mask;
        _yearDays[ getMonthIndex( date ) ] &= mask;
    }
}



bool 
Calendar::isMarked ( const synthese::time::Date& date ) const
{
    if ( date.isUnknown () )
        return false;
    
    Mask mask = 1;
    mask <<= ( date.getDay() - 1 );
    return ( mask & _yearDays[ getMonthIndex( date ) ] ) != 0;
}




bool 
Calendar::sharesAllElements ( const Calendar& base, 
					  const std::vector<Calendar::Mask>& other) const
{
    for ( int y = _firstYear.getValue (); y <= _lastYear.getValue (); y++ )
        for ( int m = 1; m <= synthese::time::MONTHS_PER_YEAR; m++ )
            if ( ( base._yearDays[ getMonthIndex( y, m ) ] & _yearDays[ getMonthIndex( y, m ) ] ) != ( other[ getMonthIndex( y, m ) ] & _yearDays[ getMonthIndex( y, m ) ] ) )
                return false;
    return true;
}




bool 
Calendar::sharesOneElement ( const std::vector<Calendar::Mask>& other) const
{
    for ( int y = _firstYear.getValue (); y <= _lastYear.getValue (); y++ )
        for ( int m = 1; m <= synthese::time::MONTHS_PER_YEAR; m++ )
            if ( other[ getMonthIndex( y, m ) ] & _yearDays[ getMonthIndex( y, m ) ] )
                return true;
    return false;
}





bool 
Calendar::sharesOneElement ( const Calendar& other ) const
{
    return ( sharesOneElement ( other._yearDays ) );
}




std::vector<Calendar::Mask>
Calendar::logicalAnd (const Calendar& other) const
{
    std::vector<Calendar::Mask> newMask;
    for ( int y = _firstYear.getValue (); y <= _lastYear.getValue (); y++ )
        for ( int m = 1; m <= synthese::time::MONTHS_PER_YEAR; m++ )
            newMask[ getMonthIndex( y, m ) ] = other._yearDays[ getMonthIndex( y, m ) ] & _yearDays[ getMonthIndex( y, m ) ];
    return ( newMask );
}





std::vector<Calendar::Mask> 
Calendar::excludedElements ( const Calendar& other ) const
{
    // L'opérateur ! ne semble pas convenir: est ce du bit a bit ?
    std::vector<Calendar::Mask> newMask;
    for ( int y = _firstYear.getValue (); y <= _lastYear.getValue (); y++ )
        for ( int m = 1; m <= synthese::time::MONTHS_PER_YEAR; m++ )
            newMask[ getMonthIndex( y, m ) ] = !_yearDays[ getMonthIndex( y, m ) ] & other._yearDays[ getMonthIndex( y, m ) ];
    return ( newMask );
}




int 
Calendar::card ( const std::vector<Calendar::Mask>& other) const
{
    int t = 0;
    Mask tmpMask;
    Mask tmpMask2;
    for ( int y = _firstYear.getValue (); y <= _lastYear.getValue (); y++ )
        for ( int m = 1; m <= synthese::time::MONTHS_PER_YEAR; m++ )
        {
            tmpMask = 1;
            tmpMask2 = _yearDays[ getMonthIndex( y, m ) ];
            for ( int d = 1; d <= 31; d++ )
            {
                if ( tmpMask2 & tmpMask & other[ getMonthIndex( y, m ) ] ) t++;
                tmpMask <<= 1;
            }
        }
    return ( t );
}




int 
Calendar::card( const Calendar& other ) const
{
    return ( card ( other._yearDays ) );
}




void 
Calendar::reset ( bool value )
{
    for ( int y = _firstYear.getValue (); y <= _lastYear.getValue (); y++ )
    {
        for ( int m = 1; m <= synthese::time::MONTHS_PER_YEAR; m++ )
	{
            if ( value )
            {
                _yearDays[ getMonthIndex( y, m ) ] = 4294967295UL;
            }
            else
	    {
                _yearDays[ getMonthIndex( y, m ) ] = 0;
	    }
	}
    }
}




int 
Calendar::getMonthIndex (const synthese::time::Date& date) const
{
    return getMonthIndex ( date.getYear (), date.getMonth () );
}



int 
Calendar::getMonthIndex ( int year, int month ) const
{
    return ( year - _firstYear.getValue () ) * synthese::time::MONTHS_PER_YEAR + month;
}






void 
Calendar::setInclusionToMask ( std::vector<Calendar::Mask>& calendar, 
				      InclusionType type ) const
{
    if ( type == INCLUSION_TYPE_POSITIVE )
        for ( int y = _firstYear.getValue (); y <= _lastYear.getValue (); y++ )
            for ( int m = 1; m <= synthese::time::MONTHS_PER_YEAR; m++ )
                calendar[ getMonthIndex( y, m ) ] |= _yearDays[ getMonthIndex( y, m ) ];
    else
    {
        Mask tempMask;
        for ( int y = _firstYear.getValue (); y <= _lastYear.getValue (); y++ )
            for ( int m = 1; m <= synthese::time::MONTHS_PER_YEAR; m++ )
            {
                tempMask = _yearDays[ getMonthIndex( y, m ) ];
                tempMask = ~tempMask;
                calendar[ getMonthIndex( y, m ) ] &= tempMask;
            }
    }
}




void 
Calendar::setInclusionToMask ( Calendar& calendar, 
				      InclusionType type ) const
{
    setInclusionToMask ( calendar._yearDays, type );
}




synthese::time::Date 
Calendar::getFirstOperationDay () const
{
    synthese::time::Date curDate;
    Mask tempMask;
    Mask tempMask2;

    for ( int y = _firstYear.getValue (); y <= _lastYear.getValue (); y++ )
        for ( int m = 1; m <= synthese::time::MONTHS_PER_YEAR; m++ )
        {
            tempMask = 1;
            tempMask2 = _yearDays[ getMonthIndex( y, m ) ];
            for ( int d = 1; d <= 31; d++ )
            {
                if ( tempMask2 & tempMask )
                {
                    curDate.updateDate( d, m, y );
                    return ( curDate );
                }
                tempMask <<= 1;
            }
        }
    return ( curDate );
}






bool 
Calendar::setMark ( const synthese::time::Date& startDate, 
			      const synthese::time::Date& endDate, 
			      InclusionType type, 
			      int step )
{
    if ( startDate.isValid () && endDate.isValid () && step > 0 && startDate.isYearUnknown () == endDate.isYearUnknown () )
    {
        if ( startDate.isYearUnknown () )
        {
            synthese::time::Date currentDate;
            synthese::time::Date currentEndDate = endDate;
            for ( synthese::time::Year y = _firstYear; y <= _lastYear; y++ )
            {
                currentDate.updateDate( startDate.getDay(), startDate.getMonth (), y.getValue () );
                if ( startDate <= endDate )
                    currentEndDate.updateDate( currentEndDate.getDay(), currentEndDate.getMonth (), y.getValue () );
                else if ( y == _lastYear )
                    currentEndDate.updateDate( synthese::time::TIME_MAX, synthese::time::TIME_MAX, _lastYear.getValue () );
                else
                    currentEndDate.updateDate( currentEndDate.getDay(), currentEndDate.getMonth (), y.getValue () + 1 );

                for ( ; currentDate <= currentEndDate; currentDate += step )
                    setMark ( currentDate, type );
            }
        }
        else
            for ( synthese::time::Date currentDate = startDate; currentDate <= endDate; currentDate += step )
                setMark ( currentDate, type );
        return ( true );
    }
    else
        return ( false );
}







}
}

