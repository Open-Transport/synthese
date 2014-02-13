#!/usr/bin/perl

use strict;
use DBI;

my $node_id_cible = 50;

my $dbh = DBI->connect(          
    "dbi:SQLite:dbname=config.db3", 
    "",                          
    "",                          
    { RaiseError => 1 },         
) or die $DBI::errstr;

my $sth = $dbh->prepare("SELECT id FROM t007_connection_places ORDER BY id;");
$sth->execute();

# On parcourt les id de t007_connection_places en cherchant ceux de node_id 0 ou 1
# On cherche en même temps le plus grand id respectant le node_id de notre base
# c'est le dernier qui respecte le node_id vu qu'on a mis ORDER BY dans la requête
my @tab_id_to_change = ();
my $num_id_to_change = 0;
my $last_id_of_this_node = 0;
while (my $ids = $sth->fetchrow_hashref())
{
	my $id=$$ids{'id'};
	my $id_hex = sprintf("%x", $id);
	my $node_id_hex = substr $id_hex, 3, 2; # Attention en copiant/collant cette ligne, elle est valable pour les tables < 16, sinon ça décale
	my $node_id = sprintf hex $node_id_hex;
	if ($node_id == 0 || $node_id == 1)
	{
		$tab_id_to_change[$num_id_to_change] = $id;
		$num_id_to_change++;
	}
	if ($node_id == $node_id_cible)
	{
		$last_id_of_this_node = $id;
	}
}
$sth->finish();
print "On a trouvé $num_id_to_change connection_places à changer\n";
print "Le plus grand id respectant le node 50 existant est $last_id_of_this_node\n";
sleep(5);

# Création du tableau des nouveaux id
my $cpt=0;
my @tab_new_id = ();
for ($cpt;$cpt<$num_id_to_change;$cpt++)
{
	$last_id_of_this_node++;
	$tab_new_id[$cpt] = $last_id_of_this_node;
}

# Requêtes de mise à jour de la base écrites dans un fichier car
# - c'est pas plus mal pour envoyer juste le SQL si on veut
# - le driver sqlite de perl ne gère pas les tables avec géométrie

open(FILE,">replace_id_t007_connection_places.sql") or die"open: $!";
$cpt=0;
my %t042_commercial_lines_optional_reservation_places = ();
my %t082_free_drt_areas_stop_areas = ();
my %t041_display_screens_forbidden_arrival_places_ids = ();
my %t041_display_screens_displayed_places_ids = ();
my %t041_display_screens_forced_destinations_ids = ();
my %t041_display_screens_transfer_destinations = ();
my %t064_hiking_trails_stops = ();
my %t071_drt_areas_stops = ();
my %t027_profiles_rights = ();
for ($cpt;$cpt<$num_id_to_change;$cpt++)
{
	print "Mise à jour $cpt de $num_id_to_change (".$tab_id_to_change[$cpt]." => ".$tab_new_id[$cpt].")\n";
	#0.  Colonne id de t007_connection_places
	my $sql = "UPDATE t007_connection_places SET id = $tab_new_id[$cpt] WHERE id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un id !!";
	#1.  Colonne optional_reservation_places de t042_commercial_lines (à splitter)
	# !! Les splits modifiés doivent conserver leur valeur
	$sth = $dbh->prepare("SELECT id, optional_reservation_places FROM t042_commercial_lines;");
	$sth->execute();
	while (my $result = $sth->fetchrow_hashref())
	{
		my $id_commercial_line=$$result{'id'};
		my $optional_reservation_places=$$result{'optional_reservation_places'};
		my $optional_reservation_places_copy = $optional_reservation_places;
		if( exists( $t042_commercial_lines_optional_reservation_places{$id_commercial_line} ) )
		{
			$optional_reservation_places_copy = $t042_commercial_lines_optional_reservation_places{$id_commercial_line};
		}
		$optional_reservation_places_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$t042_commercial_lines_optional_reservation_places{$id_commercial_line}=$optional_reservation_places_copy;
		# if ($optional_reservation_places_copy ne $optional_reservation_places)
		# {
			# # Update à faire
			# $sql = "UPDATE t042_commercial_lines SET optional_reservation_places = '$optional_reservation_places_copy' WHERE id = $id_commercial_line";
			# print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un optional_reservation_places de t042_commercial_lines !!";
		# }
	}
	$sth->finish();
	#2.  Colonne stop_areas de t082_free_drt_areas (à splitter)
	$sth = $dbh->prepare("SELECT id, stop_areas FROM t082_free_drt_areas;");
	$sth->execute();
	while (my $result = $sth->fetchrow_hashref())
	{
		my $id_free_drt_area=$$result{'id'};
		my $stop_areas=$$result{'stop_areas'};
		my $stop_areas_copy = $stop_areas;
		if( exists( $t082_free_drt_areas_stop_areas{$id_free_drt_area} ) )
		{
			$stop_areas_copy = $t082_free_drt_areas_stop_areas{$id_free_drt_area};
		}
		$stop_areas_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$t082_free_drt_areas_stop_areas{$id_free_drt_area}=$stop_areas_copy;
		# if ($stop_areas_copy ne $stop_areas)
		# {
			# # Update à faire
			# $sql = "UPDATE t082_free_drt_areas SET stop_areas = '$stop_areas_copy' WHERE id = $id_free_drt_area";
			# print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un stop_areas de t082_free_drt_areas !!";
		# }
	}
	$sth->finish();
	#3.  Colonne place_id de t012_physical_stops
	my $sql = "UPDATE t012_physical_stops SET place_id = $tab_new_id[$cpt] WHERE place_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un place_id !!";
	#4.  Colonne departure_place_id de t044_reservations
	my $sql = "UPDATE t044_reservations SET departure_place_id = $tab_new_id[$cpt] WHERE departure_place_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un departure_place_id !!";
	#5.  Colonne arrival_place_id de t044_reservations
	my $sql = "UPDATE t044_reservations SET arrival_place_id = $tab_new_id[$cpt] WHERE arrival_place_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un arrival_place_id !!";
	#6.  Colonne broadcast_point_id de t041_display_screens
	my $sql = "UPDATE t041_display_screens SET broadcast_point_id = $tab_new_id[$cpt] WHERE broadcast_point_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un broadcast_point_id !!";
	#7.  Colonne forbidden_arrival_places_ids de t041_display_screens (à splitter)
	#8.  Colonne displayed_places_ids de t041_display_screens (à splitter)
	#9.  Colonne forced_destinations_ids de t041_display_screens (à splitter)
	#10. Colonne transfer_destinations de t041_display_screens (à splitter)
	$sth = $dbh->prepare("SELECT id, forbidden_arrival_places_ids, displayed_places_ids, forced_destinations_ids, transfer_destinations FROM t041_display_screens;");
	$sth->execute();
	while (my $result = $sth->fetchrow_hashref())
	{
		my $id_display_screen=$$result{'id'};
		my $forbidden_arrival_places_ids=$$result{'forbidden_arrival_places_ids'};
		my $displayed_places_ids=$$result{'displayed_places_ids'};
		my $forced_destinations_ids=$$result{'forced_destinations_ids'};
		my $transfer_destinations=$$result{'transfer_destinations'};
		my $forbidden_arrival_places_ids_copy = $forbidden_arrival_places_ids;
		my $displayed_places_ids_copy = $displayed_places_ids;
		my $forced_destinations_ids_copy = $forced_destinations_ids;
		my $transfer_destinations_copy = $transfer_destinations;
		if( exists( $t041_display_screens_forbidden_arrival_places_ids{$id_display_screen} ) )
		{
			$forbidden_arrival_places_ids_copy = $t041_display_screens_forbidden_arrival_places_ids{$id_display_screen};
		}
		if( exists( $t041_display_screens_displayed_places_ids{$id_display_screen} ) )
		{
			$displayed_places_ids_copy = $t041_display_screens_displayed_places_ids{$id_display_screen};
		}
		if( exists( $t041_display_screens_forced_destinations_ids{$id_display_screen} ) )
		{
			$forced_destinations_ids_copy = $t041_display_screens_forced_destinations_ids{$id_display_screen};
		}
		if( exists( $t041_display_screens_transfer_destinations{$id_display_screen} ) )
		{
			$transfer_destinations_copy = $t041_display_screens_transfer_destinations{$id_display_screen};
		}
		$forbidden_arrival_places_ids_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$displayed_places_ids_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$forced_destinations_ids_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$transfer_destinations_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$t041_display_screens_forbidden_arrival_places_ids{$id_display_screen}=$forbidden_arrival_places_ids_copy;
		$t041_display_screens_displayed_places_ids{$id_display_screen}=$displayed_places_ids_copy;
		$t041_display_screens_forced_destinations_ids{$id_display_screen}=$forced_destinations_ids_copy;
		$t041_display_screens_transfer_destinations{$id_display_screen}=$transfer_destinations_copy;
		# if ($forbidden_arrival_places_ids_copy != $forbidden_arrival_places_ids)
		# {
			# # Update à faire
			# $sql = "UPDATE t041_display_screens SET forbidden_arrival_places_ids = '$forbidden_arrival_places_ids_copy' WHERE id = $id_display_screen";
			# print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un forbidden_arrival_places_ids de t041_display_screens !!";
		# }
		# if ($displayed_places_ids_copy ne $displayed_places_ids)
		# {
			# # Update à faire
			# $sql = "UPDATE t041_display_screens SET displayed_places_ids = '$displayed_places_ids_copy' WHERE id = $id_display_screen";
			# print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un displayed_places_ids de t041_display_screens !!";
		# }
		# if ($forced_destinations_ids_copy ne $forced_destinations_ids)
		# {
			# # Update à faire
			# $sql = "UPDATE t041_display_screens SET forced_destinations_ids = '$forced_destinations_ids_copy' WHERE id = $id_display_screen";
			# print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un forced_destinations_ids de t041_display_screens !!";
		# }
		# if ($transfer_destinations_copy ne $transfer_destinations)
		# {
			# # Update à faire
			# $sql = "UPDATE t041_display_screens SET transfer_destinations = '$transfer_destinations_copy' WHERE id = $id_display_screen";
			# print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un transfer_destinations de t041_display_screens !!";
		# }
	}
	$sth->finish();
	#11. Colonne place_id de t053_timetable_rows
	my $sql = "UPDATE t053_timetable_rows SET place_id = $tab_new_id[$cpt] WHERE place_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un place_id !!";
	#12. Colonne stops de t064_hiking_trails (à splitter)
	$sth = $dbh->prepare("SELECT id, stops FROM t064_hiking_trails;");
	$sth->execute();
	while (my $result = $sth->fetchrow_hashref())
	{
		my $id_hiking_trail=$$result{'id'};
		my $stops=$$result{'stops'};
		my $stops_copy = $stops;
		if( exists( $t064_hiking_trails_stops{$id_hiking_trail} ) )
		{
			$stops_copy = $t064_hiking_trails_stops{$id_hiking_trail};
		}
		$stops_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$t064_hiking_trails_stops{$id_hiking_trail}=$stops_copy;
		# if ($stops_copy ne $stops)
		# {
			# # Update à faire
			# $sql = "UPDATE t064_hiking_trails SET stops = '$stops_copy' WHERE id = $id_hiking_trail";
			# print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un stops de t064_hiking_trails !!";
		# }
	}
	$sth->finish();
	#13. Colonne stop_area_id de t091_timetable_rowgroup_items
	my $sql = "UPDATE t091_timetable_rowgroup_items SET stop_area_id = $tab_new_id[$cpt] WHERE stop_area_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un stop_area_id !!";
	#14. Colonne stop_area_id de t098_vdv_server_subscriptions
	my $sql = "UPDATE t098_vdv_server_subscriptions SET stop_area_id = $tab_new_id[$cpt] WHERE stop_area_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un stop_area_id !!";
	#15. Colonne stops de t071_drt_areas (à splitter)
	$sth = $dbh->prepare("SELECT id, stops FROM t071_drt_areas;");
	$sth->execute();
	while (my $result = $sth->fetchrow_hashref())
	{
		my $id_drt_area=$$result{'id'};
		my $stops=$$result{'stops'};
		my $stops_copy = $stops;
		if( exists( $t071_drt_areas_stops{$id_drt_area} ) )
		{
			$stops_copy = $t071_drt_areas_stops{$id_drt_area};
		}
		$stops_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$t071_drt_areas_stops{$id_drt_area}=$stops_copy;
		# if ($stops_copy ne $stops)
		# {
			# # Update à faire
			# $sql = "UPDATE t071_drt_areas SET stops = '$stops_copy' WHERE id = $id_drt_area";
			# print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un stops de t071_drt_areas !!";
		# }
	}
	$sth->finish();
	#16. Colonne rights de t027_profiles (à splitter)
	$sth = $dbh->prepare("SELECT id, rights FROM t027_profiles;");
	$sth->execute();
	while (my $result = $sth->fetchrow_hashref())
	{
		my $id_profile=$$result{'id'};
		my $rights=$$result{'rights'};
		my $rights_copy = $rights;
		if( exists( $t027_profiles_rights{$id_profile} ) )
		{
			$rights_copy = $t027_profiles_rights{$id_profile};
		}
		$rights_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$t027_profiles_rights{$id_profile}=$rights_copy;
		# if ($rights_copy ne $rights)
		# {
			# # Update à faire
			# $sql = "UPDATE t027_profiles SET rights = '$rights_copy' WHERE id = $id_profile";
			# print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un rights de t027_profiles !!";
		# }
	}
	$sth->finish();
	#17. Colonne object_id de t040_alarm_object_links
	my $sql = "UPDATE t040_alarm_object_links SET object_id = $tab_new_id[$cpt] WHERE object_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un object_id !!";
	#18. Colonne aliased_place_id de t011_place_aliases ?
	my $sql = "UPDATE t011_place_aliases SET aliased_place_id = $tab_new_id[$cpt] WHERE aliased_place_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un aliased_place_id !!";
	#19. Colonne object_id de t045_log_entries ?
	my $sql = "UPDATE t045_log_entries SET object_id = $tab_new_id[$cpt] WHERE object_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un object_id !!";
	#20. Colonne object2_id de t045_log_entries ?
	my $sql = "UPDATE t045_log_entries SET object2_id = $tab_new_id[$cpt] WHERE object2_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un object2_id !!";
	#21. Colonne place_id de t058_display_screen_cpu ?
	my $sql = "UPDATE t058_display_screen_cpu SET place_id = $tab_new_id[$cpt] WHERE place_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un place_id !!";
}

foreach my $id_t042_commercial_lines ( keys %t042_commercial_lines_optional_reservation_places ) {
	my $sql = "UPDATE t042_commercial_lines SET optional_reservation_places = '$t042_commercial_lines_optional_reservation_places{$id_t042_commercial_lines}' WHERE id = $id_t042_commercial_lines";
	print FILE $sql.";\n";
}
foreach my $id_t082_free_drt_areas ( keys %t082_free_drt_areas_stop_areas ) {
	my $sql = "UPDATE t082_free_drt_areas SET stop_areas = '$t082_free_drt_areas_stop_areas{$id_t082_free_drt_areas}' WHERE id = $id_t082_free_drt_areas";
	print FILE $sql.";\n";
}
foreach my $id_t041_display_screens ( keys %t041_display_screens_forbidden_arrival_places_ids ) {
	my $sql = "UPDATE t041_display_screens SET forbidden_arrival_places_ids = '$t041_display_screens_forbidden_arrival_places_ids{$id_t041_display_screens}' WHERE id = $id_t041_display_screens";
	print FILE $sql.";\n";
}
foreach my $id_t041_display_screens ( keys %t041_display_screens_displayed_places_ids ) {
	my $sql = "UPDATE t041_display_screens SET displayed_places_ids = '$t041_display_screens_displayed_places_ids{$id_t041_display_screens}' WHERE id = $id_t041_display_screens";
	print FILE $sql.";\n";
}
foreach my $id_t041_display_screens ( keys %t041_display_screens_forced_destinations_ids ) {
	my $sql = "UPDATE t041_display_screens SET forced_destinations_ids = '$t041_display_screens_forced_destinations_ids{$id_t041_display_screens}' WHERE id = $id_t041_display_screens";
	print FILE $sql.";\n";
}
foreach my $id_t041_display_screens ( keys %t041_display_screens_transfer_destinations ) {
	my $sql = "UPDATE t041_display_screens SET transfer_destinations = '$t041_display_screens_transfer_destinations{$id_t041_display_screens}' WHERE id = $id_t041_display_screens";
	print FILE $sql.";\n";
}
foreach my $id_t064_hiking_trails ( keys %t064_hiking_trails_stops ) {
	my $sql = "UPDATE t064_hiking_trails SET stops = '$t064_hiking_trails_stops{$id_t064_hiking_trails}' WHERE id = $id_t064_hiking_trails";
	print FILE $sql.";\n";
}
foreach my $id_t071_drt_areas ( keys %t071_drt_areas_stops ) {
	my $sql = "UPDATE t071_drt_areas SET stops = '$t071_drt_areas_stops{$id_t071_drt_areas}' WHERE id = $id_t071_drt_areas";
	print FILE $sql.";\n";
}
foreach my $id_t027_profiles ( keys %t027_profiles_rights ) {
	my $sql = "UPDATE t027_profiles SET rights = '$t027_profiles_rights{$id_t027_profiles}' WHERE id = $id_t027_profiles";
	print FILE $sql.";\n";
}



$dbh->disconnect();
