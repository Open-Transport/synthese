#ifndef SYNTHESE_BROADCAST_POINT_H
#define SYNTHESE_BROADCAST_POINT_H

namespace synthese
{
	namespace env
	{
		/** Point de diffusion d'informations.
			@ingroup m15

			Un point de diffusion est un lieu où peuvent être disposés des écrans de diffusion d'information (DisplayScreen).

			Le point de diffusion appartient nécessairement à un lieu (Place) et chaque terminal d'affichage appartient à un point de diffusion.

			Par héritage, un @ref PhysicalStop "arrêt physique" est automatiquement un point de diffusion 
		*/
		class BroadcastPoint : public cPoint
		{
		};
	}
}

#endif