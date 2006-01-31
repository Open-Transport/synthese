/*!	\file SYNTHESE.inline.h
	\brief Fonctions inline classe principale SYNTHESE
*/


#ifndef SYNTHESE_CSYNTHESE_INLINE_H
#define SYNTHESE_CSYNTHESE_INLINE_H



/*!	\brief Accesseur niveau de log
	\return le niveau de log
*/
inline tNiveauLog SYNTHESE::getNiveauLog() const
{
	return _NiveauLOG;
}


inline void	SYNTHESE::SetCheminLog(const cTexte& __CheminLog)
{
	_CheminLOG = __CheminLog;
	OuvrirLogs();
}



/*!	\brief Accesseur chemin des log
	\return le niveau de log
*/
inline const cTexte& SYNTHESE::getCheminLog() const
{
	return _CheminLOG;
}


inline cTableauDynamique<cEnvironnement*>&	SYNTHESE::TableauEnvironnements()
{
	return _Environnement;
}

inline cTableauDynamique<cInterface*>&	SYNTHESE::TableauInterfaces()
{
	return _Interface;
}

inline cTableauDynamique<cSite*>&	SYNTHESE::TableauSites()
{
	return _Site;
}

inline cLog& SYNTHESE::FichierLogAcces()
{
	return _FichierLogAcces;
}

inline cLog& SYNTHESE::FichierLogBoot()
{
	return _FichierLogBoot;
}

#endif
