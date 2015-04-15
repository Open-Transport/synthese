/* MySQL and SQLite differ in the way they handle backslashes */
/* This makes MySQL ignore them to have the same behavior between the two */
/*!40101 SET SQL_MODE='NO_BACKSLASH_ESCAPES' */;

/* TODO: rename cient_url to client_url once complex schema updates are in place. */
REPLACE INTO t025_sites(id,name,start_date,end_date,cient_url) VALUES(7036874417766401, 'SYNTHESE3 Administration', NULL, NULL, '/admin/synthese');
