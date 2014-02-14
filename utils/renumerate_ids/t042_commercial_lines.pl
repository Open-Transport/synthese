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

my $sth = $dbh->prepare("SELECT id FROM t042_commercial_lines ORDER BY id;");
$sth->execute();

# Run on id of t042_commercial_lines looking for those with node_id 0 or 1
# In the same time we look for the highest id with node_id of our database
# it is the last with node_id because of ORDER BY in the request
my @tab_id_to_change = ();
my $num_id_to_change = 0;
my $last_id_of_this_node = 0;
while (my $ids = $sth->fetchrow_hashref())
{
	my $id=$$ids{'id'};
	my $id_hex = sprintf("%x", $id);
	my $node_id_hex = substr $id_hex, 4, 2; # 4 because table code > 16
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
print "We found $num_id_to_change commercial_lines to change\n";
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

open(FILE,">replace_id_t042_commercial_lines.sql") or die"open: $!";
$cpt=0;
my %t027_profiles_rights = ();
my %t069_vehicles_allowed_lines = ();
my %t041_display_screens_allowed_lines_ids = ();
my %t052_timetables_authorized_lines = ();
for ($cpt;$cpt<$num_id_to_change;$cpt++)
{
	print "Update $cpt on $num_id_to_change (".$tab_id_to_change[$cpt]." => ".$tab_new_id[$cpt].")\n";
	#0.  Column id of t042_commercial_lines
	my $sql = "UPDATE t042_commercial_lines SET id = $tab_new_id[$cpt] WHERE id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#1. Column commercial_line_id of t082_free_drt_areas
	my $sql = "UPDATE t082_free_drt_areas SET commercial_line_id = $tab_new_id[$cpt] WHERE commercial_line_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#2. Column commercial_line_id of t009_lines
	my $sql = "UPDATE t009_lines SET commercial_line_id = $tab_new_id[$cpt] WHERE commercial_line_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#3. Column object_id of t040_alarm_object_links
	my $sql = "UPDATE t040_alarm_object_links SET object_id = $tab_new_id[$cpt] WHERE object_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#4. Column rights of t027_profiles (to split)
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
	#5. Column priority_line_id of t056_non_concurrency_rules
	my $sql = "UPDATE t056_non_concurrency_rules SET priority_line_id = $tab_new_id[$cpt] WHERE priority_line_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#6. Column hidden_line_id of t056_non_concurrency_rules
	my $sql = "UPDATE t056_non_concurrency_rules SET hidden_line_id = $tab_new_id[$cpt] WHERE hidden_line_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#7. Column allowed_lines of t069_vehicles
	$sth = $dbh->prepare("SELECT id, allowed_lines FROM t069_vehicles;");
	$sth->execute();
	while (my $result = $sth->fetchrow_hashref())
	{
		my $id_vehicle=$$result{'id'};
		my $allowed_lines=$$result{'allowed_lines'};
		my $allowed_lines_copy = $allowed_lines;
		if( exists( $t069_vehicles_allowed_lines{$id_vehicle} ) )
		{
			$allowed_lines_copy = $t069_vehicles_allowed_lines{$id_vehicle};
		}
		$allowed_lines_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$t069_vehicles_allowed_lines{$id_vehicle}=$allowed_lines_copy;
	}
	$sth->finish();
	#8. Column allowed_lines_ids of t041_display_screens
	$sth = $dbh->prepare("SELECT id, allowed_lines_ids FROM t041_display_screens;");
	$sth->execute();
	while (my $result = $sth->fetchrow_hashref())
	{
		my $id_display_screen=$$result{'id'};
		my $allowed_lines_ids=$$result{'allowed_lines_ids'};
		my $allowed_lines_ids_copy = $allowed_lines_ids;
		if( exists( $t041_display_screens_allowed_lines_ids{$id_display_screen} ) )
		{
			$allowed_lines_ids_copy = $t041_display_screens_allowed_lines_ids{$id_display_screen};
		}
		$allowed_lines_ids_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$t041_display_screens_allowed_lines_ids{$id_display_screen}=$allowed_lines_ids_copy;
	}
	$sth->finish();
	#9. Column authorized_lines of t052_timetables
	$sth = $dbh->prepare("SELECT id, authorized_lines FROM t052_timetables;");
	$sth->execute();
	while (my $result = $sth->fetchrow_hashref())
	{
		my $id_timetable=$$result{'id'};
		my $authorized_lines=$$result{'authorized_lines'};
		my $authorized_lines_copy = $authorized_lines;
		if( exists( $t052_timetables_authorized_lines{$id_timetable} ) )
		{
			$authorized_lines_copy = $t052_timetables_authorized_lines{$id_timetable};
		}
		$authorized_lines_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$t052_timetables_authorized_lines{$id_timetable}=$authorized_lines_copy;
	}
	$sth->finish();
	#10. Column line_id of t044_reservations
	my $sql = "UPDATE t044_reservations SET line_id = $tab_new_id[$cpt] WHERE line_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
}

foreach my $id_t027_profiles ( keys %t027_profiles_rights ) {
	my $sql = "UPDATE t027_profiles SET rights = '$t027_profiles_rights{$id_t027_profiles}' WHERE id = $id_t027_profiles";
	print FILE $sql.";\n";
}
foreach my $id_t069_vehicles ( keys %t069_vehicles_allowed_lines ) {
	my $sql = "UPDATE t069_vehicles SET allowed_lines = '$t069_vehicles_allowed_lines{$id_t069_vehicles}' WHERE id = $id_t069_vehicles";
	print FILE $sql.";\n";
}
foreach my $id_t041_display_screens ( keys %t041_display_screens_allowed_lines_ids ) {
	my $sql = "UPDATE t041_display_screens SET allowed_lines = '$t041_display_screens_allowed_lines_ids{$id_t041_display_screens}' WHERE id = $id_t041_display_screens";
	print FILE $sql.";\n";
}
foreach my $id_t052_timetables ( keys %t052_timetables_authorized_lines ) {
	my $sql = "UPDATE t052_timetables SET authorized_lines = '$t052_timetables_authorized_lines{$id_t052_timetables}' WHERE id = $id_t052_timetables";
	print FILE $sql.";\n";
}

$dbh->disconnect();