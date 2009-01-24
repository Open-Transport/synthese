#include "Calendar.h"

using namespace boost;

namespace synthese
{
    namespace time
    {
		Calendar::Calendar()
			: _firstMarkedDate (Date::UNKNOWN_DATE)
			, _lastMarkedDate (Date::UNKNOWN_DATE)
		{
		}



		Calendar::~Calendar()
		{
		}



		Date Calendar::getFirstActiveDate(
		) const {
			return _firstMarkedDate;
		}



		Date Calendar::getLastActiveDate(
		) const {
			return _lastMarkedDate;
		}



		int Calendar::getNbActiveDates(
		) const {
			return static_cast<int>(_markedDates.count());
		}
	
	
	
		std::vector<time::Date> Calendar::getActiveDates(
		) const {
			std::vector<time::Date> result;
			if (_firstMarkedDate == Date::UNKNOWN_DATE) return result;
			
			for (time::Date d = _firstMarkedDate; d <= _lastMarkedDate; d++)
			{
				if (isActive(d)) result.push_back (d);
			}
			return result;
		}
	
	
	
		bool Calendar::isActive(const Date& date) const
		{
			if (_firstMarkedDate == Date::UNKNOWN_DATE) return false;
			if (date < _firstMarkedDate) return false;
			if (date > _lastMarkedDate) return false;
			return _markedDates[NbBitsBetweenDates (_firstMarkedDate, date)];
		}
	
	
	
		void Calendar::setActive(const Date& date)
		{
			if (_firstMarkedDate == Date::UNKNOWN_DATE)
			{
				_firstMarkedDate = date;
				_lastMarkedDate = date;
				_markedDates.push_back (true);
			} 
			else if (date < _firstMarkedDate)
			{
				push_front (NbBitsBetweenDates (date, _firstMarkedDate));
				_markedDates[0] = true;
				_firstMarkedDate = date;
			}
			else if (date > _lastMarkedDate)
			{
				push_back (NbBitsBetweenDates (_lastMarkedDate, date));
				_markedDates[_markedDates.size () -1] = true;
				_lastMarkedDate = date;
			} 
			else 
			{
				_markedDates[NbBitsBetweenDates (_firstMarkedDate, date)] = true;
			}
		}
		
		
		
		void Calendar::clearDates()
		{
			_firstMarkedDate = Date::UNKNOWN_DATE;
			_lastMarkedDate = Date::UNKNOWN_DATE;
			_markedDates.clear();
		}
		
		
		
		void Calendar::setInactive(const Date& date)
		{
			if (_firstMarkedDate == Date::UNKNOWN_DATE) return;
			if (date < _firstMarkedDate) return;
			if (date > _lastMarkedDate) return;
	
			if (date == _firstMarkedDate)
			{
				_markedDates[0] = false;
				updateFirstMark();
			}
			else if (date == _lastMarkedDate)
			{
				_markedDates[_markedDates.size ()-1] = false;
				updateLastMark();
			}
			else 
			{
				_markedDates[NbBitsBetweenDates (_firstMarkedDate, date)] = false;
			}
		}
	
	
	
		Date Calendar::DateAfter(
			 Date date, unsigned int nbBits
		){
			int bits(
				(date.getYear () * (32*12)) + (date.getMonth () * 32) + (date.getDay ()) + nbBits
			);
			return Date (
				(bits % 384) % 32,
				(bits % 384) / 32,
				bits / 384
			);
		}
	
	
	
		Date 
		Calendar::DateBefore (Date date, unsigned int nbBits)
		{
			int bits(
				(date.getYear () * (32*12)) + (date.getMonth () * 32) + (date.getDay ()) - nbBits
			);
			return Date (
				(bits % 384) % 32,
				(bits % 384) / 32,
				bits / 384
			);
		}
	
	
		
		void Calendar::pop_front (int nbBits)
		{
			_markedDates >>= nbBits;
			_markedDates.resize (_markedDates.size () - nbBits);
		}
	
	
	
		void Calendar::pop_back (int nbBits)
		{
			_markedDates.resize (_markedDates.size () - nbBits);    
		}
	
	
	
		void Calendar::push_back(
			int nbBits,
			bool value
		){
			_markedDates.resize (_markedDates.size () + nbBits, value);
		}
	
	
	
		void Calendar::push_front (int nbBits, bool value)
		{
			_markedDates.resize (_markedDates.size () + nbBits, false);
			_markedDates <<= nbBits;
			if (value) {
			for (int i=0; i<nbBits; ++i) _markedDates[i] = value;
			}
	
		}
		
	
	
		int Calendar::NbBitsBetweenDates (Date date1, Date date2)
		{
			return 
			((date2.getYear () - date1.getYear ()) * (32*12)) +
			((date2.getMonth () - date1.getMonth ()) * 32) +
			(date2.getDay () - date1.getDay ());
		
		}
	
	
		
		Calendar& Calendar::operator&= (const Calendar& op)
		{
			LogicalAnd (*this, *this, op);
			return *this;
		}
	
	
	
		Calendar& Calendar::operator|=(
			const Calendar& op
		){
			LogicalOr (*this, *this, op);
			return *this;
		}
	
	
	
		void Calendar::LogicalOr(
			Calendar& dest,
			const Calendar& op1,
			const Calendar& op2
		){
			Calendar cop1 (op1);
			Calendar cop2 (op2);
			if (op1._firstMarkedDate >= op2._firstMarkedDate) 
			{
			dest._firstMarkedDate = op2._firstMarkedDate;
				cop1.push_front (NbBitsBetweenDates (op2._firstMarkedDate, op1._firstMarkedDate));
			}
			else if (op1._firstMarkedDate < op2._firstMarkedDate) 
			{
				dest._firstMarkedDate = op1._firstMarkedDate;
				cop2.push_front (NbBitsBetweenDates (op1._firstMarkedDate, op2._firstMarkedDate));
			} 
	
			if (op1._lastMarkedDate >= op2._lastMarkedDate) 
			{
				dest._lastMarkedDate = op1._lastMarkedDate;
				cop2.push_back (NbBitsBetweenDates (op2._lastMarkedDate, op1._lastMarkedDate));
			}
			else if (op1._lastMarkedDate < op2._lastMarkedDate) 
			{
				dest._lastMarkedDate = op2._lastMarkedDate;
				cop1.push_back (NbBitsBetweenDates (op1._lastMarkedDate, op2._lastMarkedDate));
			}
		
			// first and last marked dates cannot change (or).
			dest._markedDates = cop1._markedDates | cop2._markedDates;
		}
	
	
	
		void Calendar::LogicalAnd(
			Calendar& dest,
			const Calendar& op1,
			const Calendar& op2
		){
			dest._firstMarkedDate = op1._firstMarkedDate;
			dest._lastMarkedDate = op1._lastMarkedDate;
	
			Calendar cop (op2);
			if (op1._firstMarkedDate > op2._firstMarkedDate) 
			{
				cop.pop_front (NbBitsBetweenDates (op2._firstMarkedDate, op1._firstMarkedDate));
			}
			else if (op1._firstMarkedDate < op2._firstMarkedDate) 
			{
				cop.push_front (NbBitsBetweenDates (op1._firstMarkedDate, op2._firstMarkedDate));
			}
	
			if (op1._lastMarkedDate > op2._lastMarkedDate) 
			{
				cop.push_back (NbBitsBetweenDates (op2._lastMarkedDate, op1._lastMarkedDate));
			}
			else if (op1._lastMarkedDate < op2._lastMarkedDate) 
			{
				cop.pop_back (NbBitsBetweenDates (op1._lastMarkedDate, op2._lastMarkedDate));
			}
		
			dest._markedDates = op1._markedDates & cop._markedDates;
	
			dest.updateFirstMark ();
			dest.updateLastMark ();
		}
	
	
	
		Calendar operator& (const Calendar& op1, const Calendar& op2)
		{
			Calendar dest;
			Calendar::LogicalAnd (dest, op1, op2);
			return dest;
		}
	
	
	
		Calendar operator| (const Calendar& op1, const Calendar& op2)
		{
			Calendar dest;
			Calendar::LogicalOr (dest, op1, op2);
			return dest;
		}
	
	
	
		void Calendar::updateFirstMark ()
		{
			std::size_t nbBits = _markedDates.find_first ();
			if (nbBits == dynamic_bitset<>::npos)
			{
			_markedDates.clear ();
			_firstMarkedDate = Date::UNKNOWN_DATE;
			_lastMarkedDate = Date::UNKNOWN_DATE;
			}
			else
			{
			pop_front (nbBits);
			_firstMarkedDate = Calendar::DateAfter (_firstMarkedDate, nbBits);
			}
	
		}
	
	
	
		void 
		Calendar::updateLastMark ()
		{
			size_t nbBits = _markedDates.find_first ();
			if (nbBits == dynamic_bitset<>::npos)
			{
			_markedDates.clear ();
			_firstMarkedDate = Date::UNKNOWN_DATE;
			_lastMarkedDate = Date::UNKNOWN_DATE;
			}
			else 
			{
			int pos = (int) nbBits;
			while (++pos < _markedDates.size ()) 
			{
				if (_markedDates[pos]) nbBits = pos;
			}
			pop_back (_markedDates.size ()-1-nbBits);
			_lastMarkedDate = Calendar::DateAfter (_firstMarkedDate, nbBits);
	
			}
		}
		
		
		bool Calendar::operator||( const Calendar& op ) const
		{
			if(	_firstMarkedDate.isUnknown() ||
				_lastMarkedDate.isUnknown() ||
				op._firstMarkedDate.isUnknown() ||
				op._lastMarkedDate.isUnknown()
			){
				return false;
			}

			const Date& firstMarkedDate(
				_firstMarkedDate < op._firstMarkedDate ? op._firstMarkedDate : _firstMarkedDate
			);
			const Date& lastMarkedDate(
				_lastMarkedDate < op._lastMarkedDate ? _lastMarkedDate : op._lastMarkedDate
			);

			int thisShifting(NbBitsBetweenDates(_firstMarkedDate, firstMarkedDate));
			int opShifting(NbBitsBetweenDates(op._firstMarkedDate, firstMarkedDate));
			
			for (int todo(NbBitsBetweenDates(firstMarkedDate, lastMarkedDate)); todo >= 0; --todo)
			{
				if (_markedDates[thisShifting] && op._markedDates[opShifting])
					return true;

				++thisShifting;
				++opShifting;
			}

			return false;
		}



		bool Calendar::operator==( const Calendar& op ) const
		{
			return _firstMarkedDate == op._firstMarkedDate
				&& _lastMarkedDate == op._lastMarkedDate
				&& _markedDates == op._markedDates;
		}



		void Calendar::subDates( const Calendar& calendar )
		{
			/// @todo TO BE IMPLEMENTED
		}
    }
}
