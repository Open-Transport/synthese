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

my $sth = $dbh->prepare("SELECT id FROM t016_scheduled_services ORDER BY id;");
$sth->execute();

# Run on id of t016_scheduled_services looking for those with node_id 0 or 1
# In the same time we look for the highest id with node_id of our database
# it is the last with node_id because of ORDER BY in the request
my @tab_id_to_change = ();
my $num_id_to_change = 0;
my $last_id_of_this_node = 0;
while (my $ids = $sth->fetchrow_hashref())
{
	my $id=$$ids{'id'};
	my $id_hex = sprintf("%x", $id);
	my $node_id_hex = substr $id_hex, 4, 2; # 4 because table code = 16
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
print "We found $num_id_to_change scheduled_services to change\n";
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

open(FILE,">replace_id_t016_scheduled_services.sql") or die"open: $!";
$cpt=0;
my %t081_driver_services_services = ();
my %t077_vehicle_services_services = ();
for ($cpt;$cpt<$num_id_to_change;$cpt++)
{
	print "Update $cpt on $num_id_to_change (".$tab_id_to_change[$cpt]." => ".$tab_new_id[$cpt].")\n";
	#0.  Column id of t016_scheduled_services
	my $sql = "UPDATE t016_scheduled_services SET id = $tab_new_id[$cpt] WHERE id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#1. Column services of t081_driver_services (to split)
	$sth = $dbh->prepare("SELECT id, services FROM t081_driver_services;");
	$sth->execute();
	while (my $result = $sth->fetchrow_hashref())
	{
		my $id_driver_service=$$result{'id'};
		my $services=$$result{'services'};
		my $services_copy = $services;
		if( exists( $t081_driver_services_services{$id_driver_service} ) )
		{
			$services_copy = $t081_driver_services_services{$id_driver_service};
		}
		$services_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$t081_driver_services_services{$id_driver_service}=$services_copy;
	}
	$sth->finish();
	#2. Column services of t077_vehicle_services
	$sth = $dbh->prepare("SELECT id, services FROM t077_vehicle_services;");
	$sth->execute();
	while (my $result = $sth->fetchrow_hashref())
	{
		my $id_vehicle_service=$$result{'id'};
		my $services=$$result{'services'};
		my $services_copy = $services;
		if( exists( $t077_vehicle_services_services{$id_vehicle_service} ) )
		{
			$services_copy = $t077_vehicle_services_services{$id_vehicle_service};
		}
		$services_copy =~ s/$tab_id_to_change[$cpt]/$tab_new_id[$cpt]/g;
		$t077_vehicle_services_services{$id_vehicle_service}=$services_copy;
	}
	$sth->finish();
	#3. Column service_id of t070_compositions
	my $sql = "UPDATE t070_compositions SET service_id = $tab_new_id[$cpt] WHERE service_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#4. Column service_id of t072_vehicle_positions
	my $sql = "UPDATE t072_vehicle_positions SET service_id = $tab_new_id[$cpt] WHERE service_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#5. Column service_id of t044_reservations
	my $sql = "UPDATE t044_reservations SET service_id = $tab_new_id[$cpt] WHERE service_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#6. Column service_id of t076_service_calendar_links
	my $sql = "UPDATE t076_service_calendar_links SET service_id = $tab_new_id[$cpt] WHERE service_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	# 7. Column service_id of t089_service_quotas
	my $sql = "UPDATE t089_service_quotas SET service_id = $tab_new_id[$cpt] WHERE service_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
}

foreach my $id_t081_driver_services ( keys %t081_driver_services_services ) {
	my $sql = "UPDATE t081_driver_services SET services = '$t081_driver_services_services{$id_t081_driver_services}' WHERE id = $id_t081_driver_services";
	print FILE $sql.";\n";
}
foreach my $id_t077_vehicle_services ( keys %t077_vehicle_services_services ) {
	my $sql = "UPDATE t077_vehicle_services SET services = '$t077_vehicle_services_services{$id_t077_vehicle_services}' WHERE id = $id_t077_vehicle_services";
	print FILE $sql.";\n";
}

$dbh->disconnect();