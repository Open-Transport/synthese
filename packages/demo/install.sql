
REPLACE INTO "t025_sites" VALUES(7036874417766411,'Demo',NULL,NULL,1,0,10,14,'02:30:00|02:30:00|Journée,02:30:00|12:00:00|Matin,11:00:00|15:00:00|Midi,14:00:00|19:00:00|Après midi,18:00:00|02:30:00|Soir',0,'/synthese','17732927827738728');

-- Site template
REPLACE INTO "t063_web_pages" VALUES(17732927827738728,7036874417766411,0,0,'Template','<h1><@title@></h1>

Site Template

<@content@>','2011-07-12 13:04:40',NULL,'','','','',0,0,'','',0,1);
-- Home page
REPLACE INTO "t063_web_pages" VALUES(17732927827738729,7036874417766411,17732927827738728,0,'home','<p>Home Page</p>','2011-07-12 13:11:02',NULL,'','','','',0,0,'','',0,0);
