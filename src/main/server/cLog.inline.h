
#include "cTexteSQL.h"

inline bool cLog::ControleNiveau(tNiveauLog __Niveau)	const
{
	return __Niveau >= _Niveau;
}

inline void cLog::Fermer()
{
	_Fichier.close();
}

inline cLog::~cLog()
{
	Fermer();
}


inline void cLog::Ouvrir(const cTexte& __NomFichier)
{
	_Fichier.open(__NomFichier.Texte(), ofstream::out | ofstream::app);
}


/*!	\brief Consignation d'un événement de trajet dans le log
	\param __Niveau Niveau de l'alerte
	\param __Message Message complémentaire à ajouter
	\param __Code Code de l'erreur
*/
inline void cLog::Ecrit(tNiveauLog __Niveau, const cTexte& __Message, const cTexte& __Code)
{
	if (ControleNiveau(__Niveau))
	{
		cTexteSQL __DateISO;
		cMoment __Maintenant;
		__Maintenant.setMoment();
		__DateISO << __Maintenant;
	
		_Fichier << __DateISO << ";" << __Code << ";" << __Message << endl;
		_Fichier.flush();
	}
}



/*!	\brief Consignation d'un élément de trajet dans le log
	\param __Niveau Niveau de l'alerte
	\param __ElementTrajet Element de trajet à consigner
	\param __Message Message complémentaire à ajouter
	\param __Code Code de l'erreur
*/
inline void cLog::Ecrit(tNiveauLog __Niveau, const cElementTrajet* __ElementTrajet, const cTexte& __Message, const cTexte& __Code)
{
	if (ControleNiveau(__Niveau))
		EcritElementTrajet(__Niveau, __ElementTrajet, __Message, __Code);
}

inline void	cLog::Ecrit(tNiveauLog __Niveau, const cTrajet& __Trajet, int __Numero, const cTexte& __Message, const cTexte& __Code)
{
	if (ControleNiveau(__Niveau))
		EcritTrajet(__Niveau, __Trajet, __Numero, __Message, __Code);
}

inline void cLog::SetNiveau(tNiveauLog __Niveau)
{
	_Niveau = __Niveau;
}
