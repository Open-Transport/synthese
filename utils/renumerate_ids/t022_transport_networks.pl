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

my $sth = $dbh->prepare("SELECT id FROM t022_transport_networks ORDER BY id;");
$sth->execute();

# Run on id of t022_transport_networks looking for those with node_id 0 or 1
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

if ($last_id_of_this_node == 0)
{
	# There is no id with node_id in this table !
	my $table_id_hex = sprintf("%x", 22);
	$table_id_hex .= "00";
	my $node_id_hex = sprintf("%x", $node_id_cible);
	$node_id_hex .= "00";
	my $last_id_of_this_node_hex = $table_id_hex.$node_id_hex."000000";
	$last_id_of_this_node = sprintf hex $last_id_of_this_node_hex;
}

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

open(FILE,">replace_id_t022_transport_networks.sql") or die"open: $!";
$cpt=0;
my %t027_profiles_rights = ();
my %t105_imports_parameters = ();
for ($cpt;$cpt<$num_id_to_change;$cpt++)
{
	print "Update $cpt on $num_id_to_change (".$tab_id_to_change[$cpt]." => ".$tab_new_id[$cpt].")\n";
	#0.  Column id of t022_transport_networks
	my $sql = "UPDATE t022_transport_networks SET id = $tab_new_id[$cpt] WHERE id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#1. Column network_id of t042_commercial_lines
	my $sql = "UPDATE t042_commercial_lines SET network_id = $tab_new_id[$cpt] WHERE network_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#2. Column rights of t027_profiles (to split)
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
	#3. Column network_id of t080_dead_runs
	my $sql = "UPDATE t080_dead_runs SET network_id = $tab_new_id[$cpt] WHERE network_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#4. Column object_id of t040_alarm_object_links
	my $sql = "UPDATE t040_alarm_object_links SET object_id = $tab_new_id[$cpt] WHERE object_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#5. Column transport_network_id of t099_vdv_clients
	my $sql = "UPDATE t099_vdv_clients SET transport_network_id = $tab_new_id[$cpt] WHERE transport_network_id = $tab_id_to_change[$cpt]";
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
}

foreach my $id_t027_profiles ( keys %t027_profiles_rights ) {
	my $sql = "UPDATE t027_profiles SET rights = '$t027_profiles_rights{$id_t027_profiles}' WHERE id = $id_t027_profiles";
	print FILE $sql.";\n";
}
foreach my $id_t105_imports_parameters ( keys %t105_imports_parameters ) {
	my $sql = "UPDATE t105_imports SET parameters = '$t105_imports_parameters{$id_t105_imports_parameters}' WHERE id = $id_t105_imports_parameters";
	print FILE $sql.";\n";
}

$dbh->disconnect();