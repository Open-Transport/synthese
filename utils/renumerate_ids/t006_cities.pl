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

# On parcourt les id de t006_cities en cherchant ceux de node_id 0 ou 1
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
print "On a trouvé $num_id_to_change cities à changer\n";
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

open(FILE,">replace_id_t006_cities.sql") or die"open: $!";
$cpt=0;
my %t082_free_drt_areas_cities = ();
for ($cpt;$cpt<$num_id_to_change;$cpt++)
{
	print "Mise à jour $cpt de $num_id_to_change (".$tab_id_to_change[$cpt]." => ".$tab_new_id[$cpt].")\n";
	#0.  Colonne id de t006_cities
	my $sql = "UPDATE t006_cities SET id = $tab_new_id[$cpt] WHERE id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un id !!";
	#1. Colonne aliased_city_id de t065_city_aliases
	my $sql = "UPDATE t065_city_aliases SET aliased_city_id = $tab_new_id[$cpt] WHERE aliased_city_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un aliased_city_id !!";
	#2. Colonne city_id de t011_place_aliases
	my $sql = "UPDATE t011_place_aliases SET city_id = $tab_new_id[$cpt] WHERE city_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un city_id !!";
	#3. Colonne city_id de t013_public_places
	my $sql = "UPDATE t013_public_places SET city_id = $tab_new_id[$cpt] WHERE city_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un city_id !!";
	#4. Colonne city_id de t060_road_places
	my $sql = "UPDATE t060_road_places SET city_id = $tab_new_id[$cpt] WHERE city_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un city_id !!";
	#5. Colonne cities de t082_free_drt_areas (à splitter)
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
	#6. Colonne city_id de t007_connection_places
	my $sql = "UPDATE t007_connection_places SET city_id = $tab_new_id[$cpt] WHERE city_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un city_id !!";
	#7. Colonne object_id de t001_object_site_links
	my $sql = "UPDATE t001_object_site_links SET object_id = $tab_new_id[$cpt] WHERE object_id = $tab_id_to_change[$cpt]";
	print FILE $sql.";\n";#$dbh->do($sql) or die "Impossible de mettre à jour un object_id !!";
}

foreach my $id_t082_free_drt_areas ( keys %t082_free_drt_areas_cities ) {
	my $sql = "UPDATE t082_free_drt_areas SET cities = '$t082_free_drt_areas_cities{$id_t082_free_drt_areas}' WHERE id = $id_t082_free_drt_areas";
	print FILE $sql.";\n";
}

$dbh->disconnect();