-- Dummy page used to set the next page identifier to some high value.
-- It allows us to use the identifier gap to add our own pages from packages without collisions.
-- To be removed once we have a better mechanism to handle that.

-- Note: 17732927827738624 = 63 << 48 | 1 << 32 (63 = table id, 1 = node id)
-- REPLACE INTO t063_web_pages(id,site_id,up_id,rank,title,start_time,end_time,mime_type,do_not_use_template,content1) VALUES(17732927827738624 | 2000, 7036874417766401, 0, 0, 'Maxid allocation page',NULL,NULL,'text/html',1,'Dummy page to allocate reserved ids');

-- XXX: executescript on linux fails if there's no statements to run in this file.
select 1;
