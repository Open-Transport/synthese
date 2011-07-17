-- map package: pages 200-300

-- 200
-- FIXME: having site_id == 0 crashes synthese.
-- http://localhost:8085/synthese?SERVICE=page&p=17732927827738824
REPLACE INTO "t063_web_pages" VALUES(17732927827738824, @@site_id@@,0,0,'Menu cartes','<h1>Accès aux cartes de commune</h1>
<ul>
<?lc&item_page_id=17732927827738825&si=@@site_id@@?>
</ul>','2011-06-30 17:02:58',NULL,'','','','',0,0,'','',0,1);
-- 201
REPLACE INTO "t063_web_pages" VALUES(17732927827738825, @@site_id@@,17732927827738824,0,'Commune','<li><a href="/map/cityMap.html?cityName=<@name@>&cityId=<@roid@>&networkId=TODO"><@name@></a></li>','2011-06-30 17:04:10',NULL,'','','','',0,0,'','',0,1);

