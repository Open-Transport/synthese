/** En-tête classe Noeud de calcul d'itinéraires.
@file RoutePlanningBound.h
*/

#ifndef SYNTHESE_ROUTEPLANNINGNODE_H
#define SYNTHESE_ROUTEPLANNINGNODE_H

#include <map>


class LogicalPlace;
class NetworkAccessPoint;
class cArretPhysique;


/** Node of route planning.
 
 This class describes a place where an occurence of the route planner begins or ends.
 
 The place contains a list of network access points, with their corresponding approach duration (min) and length (km).
 
 The approach can be made up of two components :
  - the internal transfer between access points of same logical place (length = 0, duration = internal transfer duration, included security delay)
  - the pedestrian transfer between access points of different logical places (length = real, duration = speed / length + transfer durations between network access point and start address of pedestrian route)
 
 NB : The detailed description of the approach trip is not included in this class, and must be calculated.
 
 If the supporting logical place is volatile (precise address...), it is destroyed when the node is destroyed (see ~RoutePlanningNode() )
*/
class RoutePlanningNode
{
    public:

        /** Distance in meters */
        typedef double DistanceInMeters;

        /** Speed in km/h */
        typedef double SpeedInKmh;

        /** Approach description */
        typedef std::pair<int, DistanceInMeters> ApproachDescription;

        /** Network access point / distance map */
        typedef std::map<const cArretPhysique*, ApproachDescription> AccessPointsMap;

    private:

        //! @name Data
        //@{
        const LogicalPlace* const _logicalPlace; //!< Logical place supporting the node
        AccessPointsMap _data; //!< Data : List of network access points with approach duration and distance
        int _approachDurationAmplitude; //!< Difference between highest et lowest approch duration
        //@}

    public:

        //! @name Accessors
        //@{
        const AccessPointsMap& getAccessPoints() const { return _data; }
        const int& getApproachDurationAmplitude() const { return _approachDurationAmplitude; }
        //@}

        //! @name Calculators
        //@{
        bool includes( const cArretPhysique* ) const;
        const int& leastApproachDuration() const;
        //@}

        //! @name Modificators
        //@{
        void addAccessPoint( const cArretPhysique*, const int&, const DistanceInMeters& );
        //@}

        //! @name Constructors and destructor
        //@{
        RoutePlanningNode( const NetworkAccessPoint*,
                           const bool forDeparture,
                           const DistanceInMeters& maxApproachDistance,
                           const SpeedInKmh& approachSpeed );

        RoutePlanningNode( const LogicalPlace*,
                           const bool forDeparture,
                           const DistanceInMeters& maxApproachDistance,
                           const SpeedInKmh& approachSpeed );

        ~RoutePlanningNode();
        //@}
};

#endif

