/** Common SYNTHESE3 admin interface.
	@file install.sql

	Defines :
		- simple login screen
		- generic admin screen
		- html redirection page

	Used IDs :
		- interface : 6755399441055745 (1)
		- interface pages : 184464073709551519 à 184464073709551521
*/

/* Clean */

DELETE FROM t023_interface_pages WHERE interface_id=6755399441055745;
DELETE FROM t024_interfaces WHERE id=6755399441055745;

/* Interface */

REPLACE INTO t024_interfaces VALUES(6755399441055745, '', 'SYNTHESE3 Administration','/synthese3/admin');

/* Interface pages */

REPLACE INTO "t023_interface_pages" VALUES(6473924464345188, 6755399441055745, 'admin','',0,'text/html','
print {<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">}
print {<html xmlns="http://www.w3.org/1999/xhtml" lang="fr" xml:lang="fr">}
print {<head>}
print {<title>SYNTHESE3 Admin - } {{adminpos {/} {/} 0 0 0 0}} {</title>}
print {<style type="text/css">@import url(colors.css); @import url(admin.css); @import url(calendar-win2k-1.css); @import url(treeview.css);</style>}
print {<script type="text/javascript" src="calendar.js"></script><script type="text/javascript" src="lang/calendar-fr.js"></script><script type="text/javascript" src="calendar-setup.js"></script>}
print {<script type="text/javascript" src="treeview.js"></script>}
print {</head>}
print {<body>}
print {<div id="wrapall">}
print {<div id="title">}

print {<div id="logo">}
print {<img src="synthese.png" alt="SYNTHESE" />}
print {<div id="calls"><div id="call"}
if {{callduration}} { class="incall"}
print {>}
if {{callduration}} {EN APPEL}
if {{callcustomer}} {{print { avec } {{callcustomer}}}}
if {{callduration}} {{print {<span id="calldurationspan"></span><script type="text/javascript>durationSinceCallStart=} {{callduration}} {; updateCallStopwatch()</script>}}}
print {</div>}
callbutton {phone.png} {Prendre appel} {phone_delete.png} {Stopper appel}
print {</div>}

print {</div>}

print {<div id="adminpos">&nbsp;}
if @0 {{adminpos {&nbsp;/&nbsp;} {<div id="pagename">} 1 1 1 1}} {<div id="pagename"><img src="key.png" alt="Login" />Login}
print {</div>}
print {</div>}

print {<div id="user">}
print {<div id="username"><img src="user.png" alt="Utilisateur" />} @0 {</div>}
print {<div id="logout">}
if @0 {{print {<a class="linkbutton" href="} @10 {"><img src="disconnect.png" alt="Déconnecter" title="Déconnecter">&nbsp;Déconnecter</a>}}}
print {</div></div>}

// end title
print {</div>}

print {<div id="content">}

goto {{if @0 logged not_logged}}

label logged

print {<div id="admintree"><div id="admintree2">}
admintree {<img src="ftv2node.png" />} {<img src="ftv2lastnode.png" />} {<img src="ftv2vertline.png" />} {<img src="empty.png" />} {<div class="visiblesubnode">} {<div class="hiddensubnode">} {<div>} {</div></div>} {<img src="ftv2mnode.png" class="node" />} {<img src="ftv2pnode.png" class="node" />} {<img src="ftv2mlastnode.png" class="node" />} {<img src="ftv2plastnode.png" class="node" />}
print {</div></div>}

print {<div id="admincontent"><div id="admincontent2">} @4 @5 {</div></div>}

goto endif
label not_logged

print {<div id="admintree"><div id="admintree2">}
print {</div></div>}

print {<div id="admincontent"><div id="admincontent2">}

print @6 {<p style="font-size:10pt;"><table style="border:none; margin:50px;"><tr><td>Login</td><td>Mot de passe</td></tr><tr>}
print {<td>} @7 {</td>}
print {<td>} @8 {</td>}
print {<td><input type="submit" value="ok" /></td>}
print {</tr></table></p>} @9

print {</div></div>}

label endif

print {</div>}

print {<div id="footer">}
print {SYNTHESE v} {{version}} { - &copy; 2003-2009 - SYNTHESE est un produit sous <a href="http://www.gnu.org/licenses/gpl.html">licence GPL</a> - Plus d''informations sur <a href="http://synthese.rcsmobility.com/">synthese.rcsmobility.com</a>}
print {</div>}

print {</div>}

if @1 {{print {<script type="text/javascript">alert("} @1 {");</script>}}}
print {</body></html>}
');
