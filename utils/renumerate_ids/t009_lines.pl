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

my $sth = $dbh->prepare("SELECT id FROM t009_lines ORDER BY id;");
$sth->execute();

# Run on id of t009_lines looking for those with node_id 0 or 1
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
print "We found $num_id_to_change lines to change\n";
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

open(FILE,">replace_id_t009_lines.sql") or die"open: $!";
$cpt=0;
for ($cpt;$cpt<$num_id_to_change;$cpt++)
{
	print "Update $cpt on $num_id_to_change (".$tab_id_to_change[$cpt]." => ".$tab_new_id[$cpt].")\n";
	#0.  Column id of t009_lines
	my $sql = "UPDATE t009_lines SET id = $tab_new_id[$cpt] WHERE id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#1. Column path_id of t017_continuous_services
	my $sql = "UPDATE t017_continuous_services SET path_id = $tab_new_id[$cpt] WHERE path_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#2. Column object_id of t040_alarm_object_links
	my $sql = "UPDATE t040_alarm_object_links SET object_id = $tab_new_id[$cpt] WHERE object_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#3. Column line_id of t010_line_stops
	my $sql = "UPDATE t010_line_stops SET line_id = $tab_new_id[$cpt] WHERE line_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#4. Column path_id of t016_scheduled_services
	my $sql = "UPDATE t016_scheduled_services SET path_id = $tab_new_id[$cpt] WHERE path_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
	#5. Column line_id of t044_reservations
	my $sql = "UPDATE t044_reservations SET line_id = $tab_new_id[$cpt] WHERE line_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";
}

$dbh->disconnect();