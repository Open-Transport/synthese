/*!	\file cArretLogiqueAccesPADe.inline.h
	\brief Impl�mentation inline des fonctions n�cessitant la d�claration pr�alable des classes cArretLogique et cAccesPADe
*/

#ifdef SYNTHESE_CPOINTARRET_CLASSE
#ifdef SYNTHESE_CACCESPADE_CLASSE

#ifndef SYNTHESE_CPOINTARRETACCESPADE_INLINE_H
#define SYNTHESE_CPOINTARRETACCESPADE_INLINE_H

inline cCommune* cAccesPADe::getCommuneReelle() const
{
	return vArretLogique->getCommune();
}

inline cGareLigne* cAccesPADe::getPremiereGareLigneDep() const
{
	return vArretLogique->PremiereGareLigneDep();
}

inline cGareLigne* cAccesPADe::getPremiereGareLigneArr() const
{
	return vArretLogique->PremiereGareLigneArr();
}

inline tIndex cAccesPADe::numeroArretLogique() const
{
	return vArretLogique->Index();
}

#endif
#endif
#endif
