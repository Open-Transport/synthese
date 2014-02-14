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

# Run on id of t007_connection_places looking for those with node_id 0 or 1
# In the same time we look for the highest id with node_id of our database
# it is the last with node_id because of ORDER BY in the request
my @tab_id_to_change = ();
my $num_id_to_change = 0;
my $last_id_of_this_node = 0;
while (my $ids = $sth->fetchrow_hashref())
{
	my $id=$$ids{'id'};
	my $id_hex = sprintf("%x", $id);
	my $node_id_hex = substr $id_hex, 3, 2; # 3 because table code < 16
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
print "We found $num_id_to_change physical_stops to change\n";
print "The highest id with node $node_id_cible is $last_id_of_this_node\n";
sleep(5);

# New id table
my $cpt=0;
my @tab_new_id = ();
for ($cpt;$cpt<$num_id_to_change;$cpt++)
{
	$last_id_of_this_node++;
	$tab_new_id[$cpt] = $last_id_of_this_node;
}

# Requests to update the database written in a file because
# - we will be able to check what is really done in our database
# - sqlite perl driver does not manage with spatial tables

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
	print "Update $cpt on $num_id_to_change (".$tab_id_to_change[$cpt]." => ".$tab_new_id[$cpt].")\n";
	#0.  Column id of t007_connection_places
	my $sql = "UPDATE t007_connection_places SET id = $tab_new_id[$cpt] WHERE id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#1.  Column optional_reservation_places of t042_commercial_lines (to split)
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
	}
	$sth->finish();
	#2.  Column stop_areas of t082_free_drt_areas (to split)
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
	}
	$sth->finish();
	#3.  Column place_id of t012_physical_stops
	my $sql = "UPDATE t012_physical_stops SET place_id = $tab_new_id[$cpt] WHERE place_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#4.  Column departure_place_id of t044_reservations
	my $sql = "UPDATE t044_reservations SET departure_place_id = $tab_new_id[$cpt] WHERE departure_place_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#5.  Column arrival_place_id of t044_reservations
	my $sql = "UPDATE t044_reservations SET arrival_place_id = $tab_new_id[$cpt] WHERE arrival_place_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#6.  Column broadcast_point_id of t041_display_screens
	my $sql = "UPDATE t041_display_screens SET broadcast_point_id = $tab_new_id[$cpt] WHERE broadcast_point_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#7.  Column forbidden_arrival_places_ids of t041_display_screens (to split)
	#8.  Column displayed_places_ids of t041_display_screens (to split)
	#9.  Column forced_destinations_ids of t041_display_screens (to split)
	#10. Column transfer_destinations of t041_display_screens (to split)
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
	}
	$sth->finish();
	#11. Column place_id of t053_timetable_rows
	my $sql = "UPDATE t053_timetable_rows SET place_id = $tab_new_id[$cpt] WHERE place_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#12. Column stops of t064_hiking_trails (to split)
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
	}
	$sth->finish();
	#13. Column stop_area_id of t091_timetable_rowgroup_items
	my $sql = "UPDATE t091_timetable_rowgroup_items SET stop_area_id = $tab_new_id[$cpt] WHERE stop_area_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#14. Column stop_area_id of t098_vdv_server_subscriptions
	my $sql = "UPDATE t098_vdv_server_subscriptions SET stop_area_id = $tab_new_id[$cpt] WHERE stop_area_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#15. Column stops of t071_drt_areas (to split)
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
	}
	$sth->finish();
	#16. Column rights of t027_profiles (to split)
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
	}
	$sth->finish();
	#17. Column object_id of t040_alarm_object_links
	my $sql = "UPDATE t040_alarm_object_links SET object_id = $tab_new_id[$cpt] WHERE object_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#18. Column aliased_place_id of t011_place_aliases ?
	my $sql = "UPDATE t011_place_aliases SET aliased_place_id = $tab_new_id[$cpt] WHERE aliased_place_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#19. Column object_id of t045_log_entries ?
	my $sql = "UPDATE t045_log_entries SET object_id = $tab_new_id[$cpt] WHERE object_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#20. Column object2_id of t045_log_entries ?
	my $sql = "UPDATE t045_log_entries SET object2_id = $tab_new_id[$cpt] WHERE object2_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#21. Column place_id of t058_display_screen_cpu ?
	my $sql = "UPDATE t058_display_screen_cpu SET place_id = $tab_new_id[$cpt] WHERE place_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
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
