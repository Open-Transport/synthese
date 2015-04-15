-- Site for read-only pages.
-- TODO: rename cient_url to client_url once complex schema updates are in place.
REPLACE INTO t025_sites(id,name,start_date,end_date,cient_url) VALUES(7036878712733796, 'Shared Pages (read-only)', NULL, NULL, '/synthese');

-- Dummy page used to set the next page identifier to some high value.
-- It allows us to use the identifier gap to add our own pages from packages without collisions.
-- Note: page id equals to: 63 << 48 | 1 << 32 | 0x80000000 (63 = table id, 1 = node id)
REPLACE INTO t063_web_pages(id,site_id,up_id,rank,title,start_time,end_time,mime_type,do_not_use_template,content1) VALUES(17732925680254976, 7036874417766500, 0, 0, 'Maxid allocation page',NULL,NULL,'text/html',1,'Dummy page to allocate reserved ids');
