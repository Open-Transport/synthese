DROP FUNCTION IF EXISTS notify_synthese_http;
CREATE FUNCTION notify_synthese_http RETURNS INTEGER SONAME "synthese_mysql_udf.so";

-- test
-- SELECT notify_synthese_http('http://localhost:8080', 'secret_token', 't025_sites', 'insert', 1688849860500940);
