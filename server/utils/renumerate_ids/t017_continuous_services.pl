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

my $sth = $dbh->prepare("SELECT id FROM t017_continuous_services ORDER BY id;");
$sth->execute();

# Run on id of t017_continuous_services looking for those with node_id 0 or 1
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
	my $table_id_hex = sprintf("%x", 17);
	$table_id_hex .= "00";
	my $node_id_hex = sprintf("%x", $node_id_cible);
	$node_id_hex .= "00";
	my $last_id_of_this_node_hex = $table_id_hex.$node_id_hex."000000";
	$last_id_of_this_node = sprintf hex $last_id_of_this_node_hex;
}

print "We found $num_id_to_change continuous_services to change\n";
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

open(FILE,">replace_id_t017_continuous_services.sql") or die"open: $!";
$cpt=0;
for ($cpt;$cpt<$num_id_to_change;$cpt++)
{
	print "Update $cpt on $num_id_to_change (".$tab_id_to_change[$cpt]." => ".$tab_new_id[$cpt].")\n";
	#0.  Column id of t017_continuous_services
	my $sql = "UPDATE t017_continuous_services SET id = $tab_new_id[$cpt] WHERE id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#2. Column service_id of t044_reservations
	my $sql = "UPDATE t044_reservations SET service_id = $tab_new_id[$cpt] WHERE service_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#3. Column service_id of t076_service_calendar_links
	my $sql = "UPDATE t076_service_calendar_links SET service_id = $tab_new_id[$cpt] WHERE service_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#4. Column service_id of t089_service_quotas
	my $sql = "UPDATE t089_service_quotas SET service_id = $tab_new_id[$cpt] WHERE service_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
}

$dbh->disconnect();