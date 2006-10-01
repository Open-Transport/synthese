
#ifndef SYNTHESE_DEPARTURESTABLE_ARRIVALDEPARTURETABLEGENERATOR_H
#define SYNTHESE_DEPARTURESTABLE_ARRIVALDEPARTURETABLEGENERATOR_H

/* THIS CODE COMES FROM SYNTHESE 2. IT MUST BE REFRESHED. */
#ifdef RIEN


#include <set>
#include <utility>
#include <vector>
#include <map>
#include "cGareLigne.h"
#include "cPointArret.h"
#include "cLigne.h"
#include "cQuai.h"

namespace synthese
{
namespace departurestable
{

/** Result of departure/arrival table computing.
	@ingroup m34

	SYNTHESE3 : the place will be removed, replaced with the physical stops list which is enough for reaching the linestops
*/
class ArrivalDepartureTableGenerator
{
public:
	typedef enum { DISPLAY_ARRIVALS, DISPLAY_DEPARTURES } Direction;
	typedef enum { ENDS_ONLY, WITH_PASSING } EndFilter;
	typedef enum { FORCE_UNLIMITED_SIZE, SIZE_AS_DEFINED } UnlimitedSize;
	static const size_t UNLIMITED_SIZE = 0;
	struct Element { const cGareLigne* linestop; int serviceNumber;	cMoment realDepartureTime; };
	struct ElementLess : public binary_function<Element, Element, bool> {
		bool operator()(const Element& _Left, const Element& _Right) const {
			return (_Left.realDepartureTime < _Right.realDepartureTime
				|| _Left.realDepartureTime == _Right.realDepartureTime 
					&& _Left.linestop != _Right.linestop
					); } };
	typedef std::vector<const cGare*> ActualDisplayedArrivalsList;
	typedef std::map<Element, ActualDisplayedArrivalsList, ElementLess> ArrivalDepartureList;
	typedef std::set<const cGare*> DisplayedPlacesList;
	typedef std::set<const cQuai*> PhysicalStopFilter;
	typedef std::set<const cLigne*> LineFilter;
	typedef std::set<const cGare*> ForbiddenPlacesList;

private:

protected:
	 set<const cGare*> _displayedPlaces;	//!< Places to be displayed according to the demand rules

	//! \name Parameters
	//@{
		const size_t	_maxSize;	//!< Maximal size of the departure table according to the demand rules.
		const Direction	_direction;
		const EndFilter	_endFilter;
		const PhysicalStopFilter _physicalStopFilter;
		const LineFilter			_lineFilter;
		const cMoment	_startDateTime;
		const cMoment	_endDateTime;
		const cGare*	_place;
		const ForbiddenPlacesList _forbiddenPlaces;
	//@}

	//!	\name Results
	//@{
		ArrivalDepartureList _result; //!< The result
	//@}

	
	/** Autorisation d'afficher une ligne sur le tableau de départs.
		@param linestop linestop to test
	*/
	bool _allowedLineStop(const cGareLigne*) const;
	
	/** Insertion.
		@return iterator on the element.
	*/
	ArrivalDepartureList::iterator _insert(const cGareLigne* linestop, int serviceNumber,	const cMoment& realDepartureTime, UnlimitedSize unlimitedSize=SIZE_AS_DEFINED);

	/** Constructor.
		@param maxSize Maximal size of the departure table (default = unlimited).
	*/
	ArrivalDepartureTableGenerator(
		const cGare* place
		, Direction, EndFilter
		, const PhysicalStopFilter&, const LineFilter&
		, const DisplayedPlacesList&, const ForbiddenPlacesList&
		, const cMoment& startDateTime, const cMoment& endDateTime
		, size_t maxSize = UNLIMITED_SIZE
	);
	virtual ~ArrivalDepartureTableGenerator() {}

public:
	
	virtual const ArrivalDepartureList& generate() = 0;

};

}
}

#endif

#endif
