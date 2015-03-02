# Procédure d'import de l'appli CMS Terminus

Ce document décrit la procédure d'import de l'appli CMS Terminus à partir d'une livraison de sources de type "tarball" (fichier tar ou tar.gz). La procédure requiert une instance de SYNTHESE installée et opérationnelle. Cette instance peut déjà contenir une version de l'appli CMS Terminus, mais ce n'est pas obligatoire.

Paramètres :
> - SERVEUR : le nom du serveur sur lequel on souhaite effectuer l'import
> - PORT : le port sur lequel le serveur écoute les requêtes HTTP (si différent de 80)
> - CMS_DIR : le répertoire dans lequel les sources CMS de Terminus ont été décompressées (ce répertoire doit contenir le fichier metadata.json et doit être créé sur le serveur cible de l'import)

Etapes
------

1)  Démarrer SYNTHESE

2)  Depuis un navigateur, aller sur l'admin dans la page de consultation des sites CMS (URL : http://SERVEUR:PORT/admin?rub=ModuleAdmin&mod=11_cms) : si un site nommé Terminus existe, cliquer sur son bouton "Ouvrir" et noter la valeur du champ "roid" dans l'URL du navigateur (ex : 7036878712733798)

3) Depuis un navigateur, aller sur connect dans la vue client (URL : http://SERVEUR:PORT/connect/client)

4)  (optionnel) Créer un job d'import CMS :

 - cliquer sur le + à droite de "Serveurs connectés"
 - dans la popup cliquer sur l'onglet Import, dans le champ "Nom" saisir "Import Terminus" et dans le champ "Type de connexion" choisir "CMS Import"
 - cliquer sur "Ajouter"

5) Dans la liste des "Serveurs connectés", cliquer sur "Import Terminus"

6) Dans l'onglet "Propriétés":

 - choisir un système d'identifiants quelconque (n'importe lequel fait l'affaire, mais il ne faut pas laisser le champ à "non défini")
 - cliquer sur "Enregistrer"

7) Dans l'onglet "Exécution" :

 - dans le champ "Répertoire" saisir le champ d'accès absolu vers CMS_DIR
 - dans le champ "Site ID" saisir l'identifiant sur site Terminus s'il existe, rien sinon
 - mettre le champ "Simulation" à "NON"

8) Cliquer sur "Exécuter import" : dans "Journal d'import" doit figurer la trace "Statut : Réussi - Simulation : NON"

