-- TODO: replace these pages with files in pages/ or pages_local/.

-- map package: pages 200-300

-- 200
-- REPLACE INTO "t063_web_pages"
--  (id, site_id, up_id, rank, title, content1, ignore_white_chars, start_time, end_time, mime_type, abstract, image, links, do_not_use_template, has_forum, smart_url_path, smart_url_default_parameter_name, raw_editor)
--   VALUES(17732927827738824, @@site_id@@,0,0,'Menu cartes','<h1>Accès aux cartes de commune</h1>
-- <ul>
-- <?lc&item_page_id=17732927827738825&si=@@site_id@@?>
-- </ul>','2011-06-30 17:02:58',NULL,'','','','',0,0,'','',0,1);
-- 201
-- REPLACE INTO "t063_web_pages"
--  (id, site_id, up_id, rank, title, content1, ignore_white_chars, start_time, end_time, mime_type, abstract, image, links, do_not_use_template, has_forum, smart_url_path, smart_url_default_parameter_name, raw_editor)
--  VALUES(17732927827738825, @@site_id@@,17732927827738824,0,'Commune','<li><a href="/map/cityMap.html?cityName=<@name@>&cityId=<@roid@>&networkId=TODO"><@name@></a></li>','2011-06-30 17:04:10',NULL,'','','','',0,0,'','',0,1);

-- dummy statement to avoid errors during load.
select 1;
