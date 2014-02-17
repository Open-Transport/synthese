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

my $sth = $dbh->prepare("SELECT id FROM t006_cities ORDER BY id;");
$sth->execute();

# Run on id of t006_cities looking for those with node_id 0 or 1
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
print "We found $num_id_to_change cities to change\n";
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

open(FILE,">replace_id_t006_cities.sql") or die"open: $!";
$cpt=0;
my %t082_free_drt_areas_cities = ();
my %t105_imports_parameters = ();
for ($cpt;$cpt<$num_id_to_change;$cpt++)
{
	print "Update $cpt on $num_id_to_change (".$tab_id_to_change[$cpt]." => ".$tab_new_id[$cpt].")\n";
	#0.  Column id of t006_cities
	my $sql = "UPDATE t006_cities SET id = $tab_new_id[$cpt] WHERE id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#1. Column aliased_city_id of t065_city_aliases
	my $sql = "UPDATE t065_city_aliases SET aliased_city_id = $tab_new_id[$cpt] WHERE aliased_city_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#2. Column city_id of t011_place_aliases
	my $sql = "UPDATE t011_place_aliases SET city_id = $tab_new_id[$cpt] WHERE city_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#3. Column city_id of t013_public_places
	my $sql = "UPDATE t013_public_places SET city_id = $tab_new_id[$cpt] WHERE city_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#4. Column city_id of t060_road_places
	my $sql = "UPDATE t060_road_places SET city_id = $tab_new_id[$cpt] WHERE city_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#5. Column cities of t082_free_drt_areas (to split)
	$sth = $dbh->prepare("SELECT id, cities FROM t082_free_drt_areas;");
	$sth->execute();
	while (my $result = $sth->fetchrow_hashref())
	{
		my $id_free_drt_area=$$result{'id'};
		my $cities=$$result{'cities'};
		my $cities_copy = $cities;
		if( exists( $t082_free_drt_areas_cities{$id_free_drt_area} ) )
		{
			$cities_copy = $t082_free_drt_areas_cities{$id_free_drt_area};
		}
		$cities_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$t082_free_drt_areas_cities{$id_free_drt_area}=$cities_copy;
	}
	$sth->finish();
	#6. Column city_id of t007_connection_places
	my $sql = "UPDATE t007_connection_places SET city_id = $tab_new_id[$cpt] WHERE city_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#7. Column object_id of t001_object_site_links
	my $sql = "UPDATE t001_object_site_links SET object_id = $tab_new_id[$cpt] WHERE object_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#8. Column parameters de t105_imports (to split)
	$sth = $dbh->prepare("SELECT id, parameters FROM t105_imports;");
	$sth->execute();
	while (my $result = $sth->fetchrow_hashref())
	{
		my $id_import=$$result{'id'};
		my $parameters=$$result{'parameters'};
		my $parameters_copy = $parameters;
		if( exists( $t105_imports_parameters{$id_import} ) )
		{
			$parameters_copy = $t105_imports_parameters{$id_import};
		}
		$parameters_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$t105_imports_parameters{$id_import}=$parameters_copy;
	}
	$sth->finish();
	#9. Column object_id of t040_alarm_object_links
	my $sql = "UPDATE t040_alarm_object_links SET object_id = $tab_new_id[$cpt] WHERE object_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
}

foreach my $id_t082_free_drt_areas ( keys %t082_free_drt_areas_cities ) {
	my $sql = "UPDATE t082_free_drt_areas SET cities = '$t082_free_drt_areas_cities{$id_t082_free_drt_areas}' WHERE id = $id_t082_free_drt_areas";
	print FILE $sql.";\n";
}
foreach my $id_t105_imports_parameters ( keys %t105_imports_parameters ) {
	my $sql = "UPDATE t105_imports SET parameters = '$t105_imports_parameters{$id_t105_imports_parameters}' WHERE id = $id_t105_imports_parameters";
	print FILE $sql.";\n";
}

$dbh->disconnect();