/*!	\file cCommuneAccesPADe.inline.h
	\brief Implémentation inline des fonctions nécessitant la déclaration préalable des classes cCommune et cAccesPADe
*/

#ifdef SYNTHESE_CCOMMUNE_CLASSE
#ifdef SYNTHESE_CACCESPADE_CLASSE

#ifndef SYNTHESE_CCOMMUNEACCESPADE_INLINE_H
#define SYNTHESE_CCOMMUNEACCESPADE_INLINE_H

/*!	\brief Affichage standard d'une désignation de point d'arrêt
	\author Hugues Romain
	\date 2005
*/
template <class T> 
inline T& operator<<(T& flux, const cAccesPADe& Obj)
{
	flux << *Obj.getCommune() << "(" << Obj.getNom() << ")";
	return flux;
}



/*!	\brief Affichage HTML d'une désignation de point d'arrêt
	\author Hugues Romain
	\date 2005
*/
template <> 
inline cTexteHTML& operator<< <cTexteHTML>(cTexteHTML& flux, const cAccesPADe& Obj)
{
	flux << *Obj.getCommune() << "&nbsp;<i>" << Obj.getNom() << "</i>";
	return flux;
}

#endif
#endif
#endif
