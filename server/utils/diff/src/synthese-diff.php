<?php

if ($argc != 3)
{
	print "Usage : ". $argv[0] ." <newbase.db3> <oldbase.db3>\n";
	exit;
}

$db = new PDO('sqlite:'. $argv[1]);
$dbo = new PDO('sqlite:'. $argv[2]);

// New stops

print "<?xml version='1.0' encoding='UTF-8'?>";
print '<syntheseDelta xsi:noNamespaceSchemaLocation="http://www.reseaux-conseil.com/trident/synthese-deltas.xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">';

$nstops = $db->query('SELECT l.id, p.name, p.x, p.y FROM t010_line_stops AS l INNER JOIN t012_physical_stops AS p ON p.id = l.physical_stop_id ');
while ($nstop = $nstops->fetch())
{
	$ostops = $dbo->query('SELECT p.name, p.x, p.y FROM  t010_line_stops AS l INNER JOIN t012_physical_stops AS p ON p.id = l.physical_stop_id WHERE l.id='. $nstop['id']);
	$row = $ostops->fetch();
	if(!$row)
		print '<StopPoint Id="SYNTHESE:StopPoint:'. $nstop['id'] .'" update="Add" />';
	else
	{
		if ($row['name'] != $nstop['name'] and ($row['x'] != $nstop['x'] or $row['y'] != $nstop['y']))
			print '<StopPoint Id="SYNTHESE:StopPoint:'. $nstop['id'] .'" update="UpdateAll" />';
		elseif ($row['name'] != $nstop['name'])
			print '<StopPoint Id="SYNTHESE:StopPoint:'. $nstop['id'] .'" update="UpdateData" />';
		elseif ($row['x'] != $nstop['x'] or $row['y'] != $nstop['y'])
			print '<StopPoint Id="SYNTHESE:StopPoint:'. $nstop['id'] .'" update="UpdateGeo" />';
	}
}
$nstops = $dbo->query('SELECT * FROM t010_line_stops');
while ($nstop = $nstops->fetch())
{
	$ostops = $db->query('SELECT * FROM t010_line_stops WHERE id='. $nstop['id']);
	$row = $ostops->fetch();
	if(!$row)
		print '<StopPoint Id="SYNTHESE:StopPoint:'. $nstop['id'] .'" update="Delete" />';
}


// Chouette route

$nstops = $db->query('SELECT * FROM t009_lines');
while ($nstop = $nstops->fetch())
{
	$ostops = $dbo->query('SELECT * FROM t009_lines WHERE id='. $nstop['id']);
	$row = $ostops->fetch();
	if(!$row)
		print '<ChouetteRoute Id="SYNTHESE:ChouetteRoute:'. $nstop['id'] .'" update="Add" />';
	else
	{
		if ($row['name'] != $nstop['name'])
			print '<ChouetteRoute Id="SYNTHESE:ChouetteRoute:'. $nstop['id'] .'" update="UpdateData" />';
	}
}
$nstops = $dbo->query('SELECT * FROM t009_lines');
while ($nstop = $nstops->fetch())
{
	$ostops = $db->query('SELECT * FROM t009_lines WHERE id='. $nstop['id']);
	$row = $ostops->fetch();
	if(!$row)
		print '<ChouetteRoute Id="SYNTHESE:ChouetteRoute:'. $nstop['id'] .'" update="Delete" />';
}

// Connection Link


$nstops = $db->query('SELECT * FROM t007_connection_places WHERE connection_type > 0');
while ($nstop = $nstops->fetch())
{
	$ostops = $dbo->query('SELECT * FROM t007_connection_places WHERE id='. $nstop['id'] .' AND connection_type > 0');
	$row = $ostops->fetch();
	if(!$row)
	{
		$pstops = $db->query('SELECT * FROM t012_physical_stops WHERE place_id="'. $nstop['id'] .'"');
		while ($pstop = $pstops->fetch())
		{
			$pstops2 = $db->query('SELECT * FROM t012_physical_stops WHERE place_id="'. $nstop['id'] .'"');
			while ($pstop2 = $pstops2->fetch())
				print '<ConnectionLink Id="SYNTHESE:ConnectionLink:'. $pstop['id'] .'t'. $pstop2['id'] .'" update="Add" />';
		}
	}
	else
	{
		if ($row['transfer_delays'] != $nstop['transfer_delays'])
		{
			$pstops = $db->query('SELECT * FROM t012_physical_stops WHERE place_id="'. $nstop['id'] .'"');
			while ($pstop = $pstops->fetch())
			{
				$pstops2 = $db->query('SELECT * FROM t012_physical_stops WHERE place_id="'. $nstop['id'] .'"');
				while ($pstop2 = $pstops2->fetch())
					print '<ConnectionLink Id="SYNTHESE:ConnectionLink:'. $pstop['id'] .'t'. $pstop2['id'] .'" update="UpdateData" />';
			}
		}
	}
}
$nstops = $dbo->query('SELECT * FROM t007_connection_places');
while ($nstop = $nstops->fetch())
{
	$ostops = $db->query('SELECT * FROM t007_connection_places WHERE id='. $nstop['id']);
	$row = $ostops->fetch();
	if(!$row)
		print '<ConnectionLink Id="SYNTHESE:ConnectionLink:'. $nstop['id'] .'" update="Delete" />';
}


print '</syntheseDelta>';

?>