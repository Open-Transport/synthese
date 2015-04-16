#pragma once

#include "VIX-BSC-defines.hpp"

namespace synthese
{

	class CIntSurvMsg
	{
	public:
		CIntSurvMsg(void);
		~CIntSurvMsg(void);
	public:
		//TODO: use boost/cstdint.hpp type
		// 					offset	size	name		range		description
		unsigned char	type		;	// 1	1	type		[1,101]		Type du message. Ici, doit etre INT_SURV
		unsigned char	year		;	// 2	1	ANNEE		[99..199]	Année courante par rapport à l’année 1900
		unsigned char	month		;	// 3	1	MOIS		[1..12]		Mois courant
		unsigned char	day		;	// 4	1	JOUR		[1..31]		Jour courant
		unsigned char	hour		;	// 5	1	HEURE		[0..23]		Heure courante
		unsigned char	min		;	// 6	1	MINUTE		[0..59]		Minute courante
		unsigned char	sec		;	// 7	1	SECONDES	[0..59]		Seconde courante
		short		num_driver	;	// 8	2	NUM_COND	[0..9999]	Dernier numéro conducteur saisi
		short		num_park	;	// 10	2	NUM_PARC	[0..9999]	Numéro de véhicule
		unsigned char	etat_expl	;	// 12	1	ETAT_EXPL	[0,1,2]		0  : hors exploitation
		//									1  : en exploitation (=en service commercial)
		//									2  : en HLP.
		unsigned int	num_line	;	// 13	4	MNE_LIGNE	[0..9999]	WARNING: TEXT. Numéro de la ligne. Mnémonique de la ligne(x31x20x20x20) pour ligne :"1   ". (x31x32x38x20) pour "128 "
		short		num_service	;	// 17	2	NUM_SERV	[0..9999]	Numéro de service agent
		short		num_journey	;	// 19	2	NUM_CRSE	[0..9999]	NON UTILISE. Désactivé dans le code VIX a la demande de Spie ou RTTB. (Confirmé par VIX)
		short		num_stop	;	// 21	2	NUM_ARRET	[0..9999]	Numéro d’arrêt
		unsigned char	direction	;	// 23	1	SENS		[0,1]		0=aller et 1=retour (correspond au sens de l’itinéraire dans les fichiers HEURES)

		void IntLigneToString(char *p, unsigned int i);

		// must return the number of real char inserted. (So 0 if any error, 1 or 2)
		int insertCharToBufferTransparentMode(unsigned char *buf, int bufSize, unsigned char cToInsert);

		// prepare the INT_SURV message to be sent
		// return the number of bytes to write. 0 if got an error.
		int StreamToBuffer(unsigned char *buf, int bufSize);
		
		bool UpdateVariablesFromEnv();
	};
}