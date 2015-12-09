
#include "PublicBikeJourneyPlannerService.hpp"
#include "PublicBikeStation.hpp"
#include "PublicBikeStationsList.hpp"

#include "PublicBikingModule.inc.cpp"

void synthese::public_biking::moduleRegister()
{
    synthese::public_biking::PublicBikeJourneyPlannerService::integrate();
    synthese::public_biking::PublicBikeStationsListFunction::integrate();

    // Registries
    INTEGRATE(synthese::public_biking::PublicBikeNetwork);
    INTEGRATE(synthese::public_biking::PublicBikeStation);
}
