#ifndef SYNTHESE_BROADCAST_POINT_H
#define SYNTHESE_BROADCAST_POINT_H

namespace synthese
{
	namespace env
	{
		/** Point de diffusion d'informations.
			@ingroup m15

			Un point de diffusion est un lieu o� peuvent �tre dispos�s des �crans de diffusion d'information (DisplayScreen).

			Le point de diffusion appartient n�cessairement � un lieu (Place) et chaque terminal d'affichage appartient � un point de diffusion.

			Par h�ritage, un @ref PhysicalStop "arr�t physique" est automatiquement un point de diffusion 
		*/
		class BroadcastPoint : public cPoint
		{
		};
	}
}

#endif