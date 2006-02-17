%% resa database

-record(personnes,
{
	id,
	nom,
	prenom,
	telephone,
	numero_abonne,
	adresse,
	email
}).

-record(profils,
{
	id,
	login,
	password,
	resa_perso,
	resa_tout,
	gestion_personne,
	gestion_profil_autres,
	gestion_profil_admin,
	vue_resa_sans_code,
	filtre_velo,
	filtre_handicape,
	filtre_tarif,
	periode_journee
}).

-record(resa,
{
	id,
	predecesseur,
	ligne,
	service,
	modalite,
	depart,
	arrivee,
	adresse_depart,
	adresse_arrivee,
	moment_depart,
	moment_arrivee,
	nombre_places,
	moment_reservation,
	moment_annulation,
	personne_client,
	personne_saisie,
	personne_annulation
}).

%% indexes:
%resa: {ligne,service}, moment_depart, personne_client
%personnes: {nom,prenom,telephone}
%profils: login
%
% avec: mnesia:add_table_index(Tab, AttributeName) -> {aborted, R} |{atomic, ok}
