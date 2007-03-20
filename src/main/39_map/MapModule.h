#include "02_db/DbModuleClass.h"


namespace synthese
{
	namespace map
	{

		/** @defgroup m39 39 Map drawing

			Le trac� de carte se fait � partir de quatre cat�gories d'�l�ments d'entr�e  :
			- donn�es g�ographiques du serveur (charg�es � l'initialisation) : voir @ref uc10 "liste des donn�es"
			- donn�es g�ographiques propres � la carte, fournies avec la demande de carte, permettant de tracer sur la carte des �l�ments suppl�mentaires provenant par exemple de l'application cliente. Les donn�es pouvant �tre transmises � la vol�e sont les m�mes que les @ref uc10 "donn�es du serveur" � l'exception des fonds de carte bitmap
			- d�finition du contenu de la carte
			- zone de couverture
			- �chelle
			- d�finition de la liste des �l�ments g�ographiques � tracer
			- param�tres techniques
			- format du fichier produit
			- param�tres de compression

			Le r�sultat produit est un r�sultat contenant l'un ou l'autre ou les deux �l�ments suivants :
			- carte demand�e au format bitmap
			- donn�es vectorielles associ�es � la carte

			Selon le format choisi, les tables de liens et la carte sont ou non fabriqu�s :

			<table class="tableau">
			<tr><th>Format</th><th>Table de liens</th><th>Carte</th><th>Utilisation de Ghostscript</th></tr>
			<tr><th>PS</th><td>NON</td><td>OUI</td><td>NON</td></tr>	
			<tr><th>PDF</th><td>OUI</td><td>OUI</td><td>OUI</td></tr>	
			<tr><th>JPEG</th><td>NON</td><td>OUI</td><td>OUI</td></tr>	
			<tr><th>HTML</th><td>OUI</td><td>NON</td><td>NON</td></tr>
			<tr><th>Mapinfo</th><td>OUI</td><td>NON</td><td>NON</td></tr>
			</table>

			@{
		*/

		class MapModule : public db::DbModuleClass
		{
		public:

		    static const std::string PARAM_HTTP_TEMP_DIR;
		    static const std::string PARAM_HTTP_TEMP_URL;
		    static const std::string PARAM_BACKGROUNDS_DIR;
		    

		    void preInit ();
		    
		    
		    /** Called whenever a parameter registered by this module is changed
		     */
		    static void ParameterCallback (const std::string& name, 
						   const std::string& value);		
		    
		};
	    /** @} */
	}
}


