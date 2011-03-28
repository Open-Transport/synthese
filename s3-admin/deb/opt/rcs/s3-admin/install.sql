/** Common SYNTHESE3 admin interface.
	@file install.sql

	Defines :
		- simple login screen
		- generic admin screen
		- html redirection page

	Used IDs :
		- site : 7036874417766401 (1)
		- pages : 177329235327713281 (1) to 177329235327713283 (3)
*/

/* MySQL and SQLite differ in the way they handle backslashes */
/* This makes MySQL ignore them to have the same behavior between the two */
/*!40101 SET SQL_MODE='NO_BACKSLASH_ESCAPES' */;

/* Site */

/* TODO: rename cient_url to client_url once complex schema updates are in place. */
REPLACE INTO t025_sites(id,name,start_date,end_date,cient_url) VALUES(7036874417766401, 'SYNTHESE3 Administration',NULL,NULL, '/synthese3/admin');

/* Pages */

REPLACE INTO t063_web_pages(id,site_id,up_id,rank,title,start_time,end_time,mime_type,do_not_use_template,content1) VALUES(177329235327713281, 7036874417766401, 0, 0, 'Admin',NULL,NULL,'text/html',1,'
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="fr" xml:lang="fr">
<head>
<meta charset="utf-8">
<title><@title@> - SYNTHESE3 Admin</title>
<link rel="icon" type="image/png" href="<@icon@>" />
<style type="text/css">@import url(colors.css); @import url(admin.css); @import url(calendar-win2k-1.css);</style>
<script type="text/javascript" src="calendar.js"></script><script type="text/javascript" src="lang/calendar-fr.js"></script><script type="text/javascript" src="calendar-setup.js"></script>
<script type="text/javascript" src="treeview.js"></script>
<script type="text/javascript" src="tiny_mce/tiny_mce_src.js"></script>
<script type="text/javascript" src="ajax_forms.js"></script>
</head>
<body>
<div id="wrapall">
<div id="title">

<div id="logo">
<img src="synthese.png" alt="SYNTHESE" />
<!--
<div id="calls"><div id="call" if {{callduration}} { class="incall"}
print {>}
if {{callduration}} {EN APPEL}
if {{callcustomer}} {{print { avec } {{callcustomer}}}}
if {{callduration}} {{print {<span id="calldurationspan"></span><script type="text/javascript>durationSinceCallStart=} {{callduration}} {; updateCallStopwatch()</script>}}}
print {</div>}
-->

<?if&cond=<@switch_call_url@>&then=<a class="linkbutton" onmouseover="this.className=''activatedlinkbutton'';" onmouseout="this.className=''linkbutton'';" onmousedown="this.className=''clickedlinkbutton'';" onmouseup="this.className=''activatedlinkbutton'';" href="<@switch_call_url@>"><img src="phone.png" alt="P" /> <?if&cond=<@current_call_id@>&then=Stopper l''appel&else=Prendre un appel?></a>?>

</div>

<div id="adminpos">&nbsp;<@position@></div>

<div id="user">
<?if&cond=<@user_full_name@>&then=<div id="username"><img src="user.png" alt="Utilisateur" /><@user_full_name@></div>?>
<div id="logout"><?if&cond=<@user_full_name@>&then=<a class="linkbutton" href="<@logout_url@>"><img src="disconnect.png" alt="Déconnecter" title="Déconnecter">\&nbsp;Déconnecter</a>?>
</div></div>

<div id="pagename"><img src="<@icon@>" /><@title@></div>

</div>

<div id="content">
  <div id="admintree"><div id="admintree2"><?if&cond=<@user_full_name@>&then=<@tree@>?></div></div>
  <div id="admincontent"><div id="admincontent2"><@tabs@><@content@></div></div>
</div>

<div id="footer">
SYNTHESE v<@version@> - &copy; 2003-2010 - SYNTHESE est un produit sous <a href="http://www.gnu.org/licenses/gpl.html">licence GPL</a> - Plus d''informations sur <a href="http://synthese.rcsmobility.com/">synthese.rcsmobility.com</a>
</div>

</div>

<?if&cond=<@error_message@>&then=<script type="text/javascript">alert("<@error_message@>");</script>?>

</body></html>
');



REPLACE INTO t063_web_pages(id,site_id,up_id,rank,title,start_time,end_time,mime_type,do_not_use_template,content1) VALUES(177329235327713282, 7036874417766401, 177329235327713281, 0, 'Tree node',NULL,NULL,'text/html',1,'
<div class="<?if&cond=<@is_last@>&then=lastblock&else=notlastblock?>">
<div class="tree_icon<?if&cond=<@depth@>&then=s?>"><?if&cond=<@depth@>&then=<?if&cond=<@subtree@>&then=<img class="node" src="<?if&cond=<@is_opened@>&then=ftv2mlastnode&else=ftv2plastnode?>.png" />&else=<img src="ftv2lastnode.png" />?>?><img src="<@icon@>" class="icon" /></div>

<div class="text level<@depth@>"><?if&cond=<@is_current@>&else=<a href="<@url@>">?><@title@><?if&cond=<@is_current@>&else=</a>?></div>
<div <?if&cond=<@is_opened@>&else=style="display:none" ?> class="subtree subtree<@depth@>"><@subtree@></div>
</div>
');



REPLACE INTO t063_web_pages(id,site_id,up_id,rank,title,start_time,end_time,mime_type,do_not_use_template,content1) VALUES(177329235327713283, 7036874417766401, 177329235327713281, 1, 'Position element',NULL,NULL,'text/html',1,'
<?if&cond=<@is_last@>&else=<?if&cond=<@depth@>&then=\&nbsp;/\&nbsp;?><a href="<@url@>"><img src="<@icon@>" /> <@title@></a>?>
');