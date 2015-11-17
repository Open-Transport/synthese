#!/usr/bin/perl

use strict;
use DBI;

my $dbh_before = DBI->connect(          
    "dbi:SQLite:dbname=config-before.db3", 
    "",                          
    "",                          
    { RaiseError => 1 },         
) or die $DBI::errstr;

my $dbh_after = DBI->connect(          
    "dbi:SQLite:dbname=config.db3", 
    "",                          
    "",                          
    { RaiseError => 1 },         
) or die $DBI::errstr;

my $sth = $dbh_before->prepare("SELECT id, road_place_id FROM t015_roads WHERE 1 ORDER BY id;");
$sth->execute();

my %tab_id_to_move = ();
my $num_id_to_move = 0;
while (my $result  = $sth->fetchrow_hashref())
{
	my $id=$$result {'id'};
	my $road_place_id=$$result {'road_place_id'};
	# On update dans la nouvelle base cette entrée
	my $sql = "UPDATE t015_roads set road_place_ids=$road_place_id where id=$id;";
	$dbh_after->do($sql);
}
$sth->finish();

$dbh_before->disconnect();
$dbh_after->disconnect();
