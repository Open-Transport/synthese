
#ifndef SYNTHESE_CLOG_H
#define SYNTHESE_CLOG_H

class cLog;

#include <fstream>
#include "cElementTrajet.h"
#include "cTrajet.h"

#define LOG_FICHIER_BOOT "boot.log"
#define LOG_FICHIER_ACCES "acces.log"
#define LOG_FICHIER_TRACE "trace.log"
#define LOG_CHEMIN_DESTINATIONS "destinations"
#define LOG_CHEMIN_PROVENANCES "provenances"
#define LOG_EXTENSION ".log"


typedef enum
{
	LogAll = 0,
	LogDebug = 20,
	LogInfo = 40,
	LogWarning = 60,
	LogError = 80,
	LogNone = 254,
	LogInconnu = 255
} tNiveauLog;

typedef int tNumeroMessageStandard;

/*!	\brief Fichier de log
	@ingroup m01
*/
class cLog
{
	ofstream								_Fichier;				//!< Fichier sur lequel seront �crits les �v�nements
	tNiveauLog								_Niveau;	//!< Niveau minimal des �v�nements � consign�s (les autres sont ignor�s)
	
	//!	\name Calculateurs
	//@{
	bool		ControleNiveau(tNiveauLog __Niveau)	const;
	//@}
	
	//!	\name Modificateurs
	//@{
	void		ChargeMessagesStandard();
	//@}

	//!	\name Messages par objet
	//@{
	void	EcritElementTrajet(tNiveauLog, const cElementTrajet*, const cTexte& __Message, const cTexte& __Code); 
	void	EcritTrajet(tNiveauLog, const cTrajet&, int __Numero, const cTexte& __Message, const cTexte& __Code); 
	//@}
	
public:
	
	//!	\name Modificateurs
	//@{
	void	SetNiveau(tNiveauLog __Niveau);
	void	Ouvrir(const cTexte& __NomFichier);
	void	Fermer();
	//@}
	
	//!	\name M�thodes d'�criture
	//@{
	void	Ecrit(tNiveauLog, const cTexte& __Message, const cTexte& __Code);
	void	Ecrit(tNiveauLog, const cElementTrajet* __ElementTrajet, const cTexte& __Message, const cTexte& __Code); 
	void	Ecrit(tNiveauLog, const cTrajet& __Trajet, int __Numero, const cTexte& __Message, const cTexte& __Code); 
	void	Ecrit(tNumeroMessageStandard, const cTexte& __Objet = "", const cTexte& __Donnees="");
	//@}
	
	cLog(tNiveauLog __Niveau = LogInconnu);
	~cLog();
};

#define SYNTHESE_CLOG

#define MESSAGE_APP_START						0
#define MESSAGE_SITES_PBALLOC					1
#define MESSAGE_SITES_PBOUVRIR					2
#define MESSAGE_SITES_PBENREG					3
#define MESSAGE_SITES_PBENV						4
#define MESSAGE_SITES_PBINT						5
#define MESSAGE_CALC_SATURE						6
#define MESSAGE_GARE_DESIGNATION_INCORRECTE		7

#endif
