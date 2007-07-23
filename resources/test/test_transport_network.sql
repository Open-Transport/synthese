BEGIN TRANSACTION;
DELETE FROM t000_environments;
INSERT INTO t000_environments VALUES(1,'Test environment 1');

DELETE FROM t004_axes;
INSERT INTO t004_axes VALUES(1125899906844436,'Test axis 1, non free, allowed',0,1);
INSERT INTO t004_axes VALUES(1125899906844499,'Test axis 2, non free, allowed',0,1);
INSERT INTO t004_axes VALUES(1125899906844501,'Test axis 3, free, allowed',1,1);
INSERT INTO t004_axes VALUES(1125899906844502,'Test axis 4, non free, non allowed',0,0);

DELETE FROM t005_service_dates;

DELETE FROM t006_cities;
INSERT INTO t006_cities VALUES(1688849860508968,'City68');
INSERT INTO t006_cities VALUES(1688849860509006,'City6');
INSERT INTO t006_cities VALUES(1688849860509012,'City12');

DELETE FROM t007_connection_places;
DELETE FROM t012_physical_stops;
DELETE FROM t002_addresses;

INSERT INTO t007_connection_places VALUES(1970324837184593,'93',1688849860508968,0,1,8,'',NULL);
INSERT INTO t012_physical_stops(id,name,place_id,x,y) VALUES(3377699720880573,'93/73',1970324837184593,523265.0,1845342.0);
INSERT INTO t002_addresses(id,place_id,road_id,metric_offset,x,y) VALUES(562949953421372,1970324837184593,523265.0,1845342.0);

INSERT INTO t007_connection_places VALUES(1970324837184594,'94',1688849860508968,3,0,8,'(1:2:11),(2:1:12)',NULL);
INSERT INTO t012_physical_stops(id,name,place_id,x,y) VALUES(3377699720880574,'94/74',1970324837184594,0.0,0.0);
INSERT INTO t012_physical_stops(id,name,place_id,x,y) VALUES(3377699720880575,'94/75',1970324837184594,528671.0,1845899.0);

INSERT INTO t007_connection_places VALUES(1970324837184595,'95',1688849860509006,0,0,8,'',NULL);
INSERT INTO t012_physical_stops(id,name,place_id,x,y) VALUES(3377699720880576,'95/76',1970324837184595,528534.0,1846071.0);

INSERT INTO t007_connection_places VALUES(1970324837184596,'96',1688849860509012,3,0,8,'',NULL);
INSERT INTO t012_physical_stops(id,name,place_id,x,y) VALUES(3377699720880577,'96/77',1970324837184596,528571.0,1845865.0);
INSERT INTO t012_physical_stops(id,name,place_id,x,y) VALUES(3377699720880578,'96/78',1970324837184596,528544.0,1845879.0);

INSERT INTO t007_connection_places VALUES(1970324837184597,'97',1688849860509012,3,1,8,'',NULL);
INSERT INTO t012_physical_stops(id,name,place_id,x,y) VALUES(3377699720880579,'97/79',1970324837184597,0.0,0.0);
INSERT INTO t012_physical_stops(id,name,place_id,x,y) VALUES(3377699720880580,'97/80',1970324837184597,0.0,0.0);

INSERT INTO t007_connection_places VALUES(1970324837184598,'98',1688849860509012,3,0,8,'',NULL);
INSERT INTO t012_physical_stops(id,name,place_id,x,y) VALUES(3377699720880581,'98/81',1970324837184598,528832.0,1845885.0);
INSERT INTO t012_physical_stops(id,name,place_id,x,y) VALUES(3377699720880582,'98/82',1970324837184598,528798.0,1845854.0);

INSERT INTO t007_connection_places VALUES(1970324837184599,'99',1688849860509012,5,1,8,'(1:1:5),(1:2:5),(2:1:5),(2:2:5)',NULL);
INSERT INTO t012_physical_stops(id,name,place_id,x,y) VALUES(3377699720880583,'99/83',1970324837184599,528810.0,1845865.0);
INSERT INTO t012_physical_stops(id,name,place_id,x,y) VALUES(3377699720880584,'99/84',1970324837184599,528820.0,1845874.0);
INSERT INTO t012_physical_stops(id,name,place_id,x,y) VALUES(3377699720880585,'99/85',1970324837184599,528819.0,1845798.0);


DELETE FROM t008_fares;

DELETE FROM t009_lines;
DELETE FROM t010_line_stops;
DELETE FROM t042_commercial_lines;
DELETE FROM t016_scheduled_services;
DELETE FROM t017_continuous_services;

INSERT INTO t042_commercial_lines VALUES(11821949021891553,6192449487677434,'Network1-CommLine1','92','','vertfonce','vertfonce','');
INSERT INTO t009_lines VALUES(2533274790397692,11821949021891553,1125899906844436,'Network1-Line1',' 1.1','',0,1,1,1,0,2251799814850473,0,0,0,0,0);
INSERT INTO t010_line_stops VALUES(2814749767106560,3377699720880573,2533274790397692,0,1,0,0.0,'');
INSERT INTO t010_line_stops VALUES(2814749767106561,3377699720880575,2533274790397692,1,1,1,5500.0,'');
INSERT INTO t010_line_stops VALUES(2814749767106562,3377699720880579,2533274790397692,2,1,1,6500.0,'');
INSERT INTO t010_line_stops VALUES(2814749767106563,3377699720880585,2533274790397692,3,0,1,6500.0,'');

INSERT INTO t016_scheduled_services VALUES(4785074604183667,'1','00:00:00#00:07:05,00:07:10#00:07:10,00:07:15#00:07:14,00:07:15#00:00:00',2533274790397692,0,0,0,0);
INSERT INTO t005_service_dates VALUES(1407374883553280,4785074604183667,date('now'));
INSERT INTO t005_service_dates VALUES(1407374883553281,4785074604183667,date('now','+1 day'));
INSERT INTO t005_service_dates VALUES(1407374883553282,4785074604183667,date('now','+2 days'));

INSERT INTO t017_continuous_services VALUES(4785074604214097,'','00:00:00#00:19:22,00:19:30#00:19:30,00:19:42#00:19:41,00:19:49#00:00:00',2533274790397692,60,5,0,0,0);
INSERT INTO t005_service_dates VALUES(1407374883553286,4785074604214097,date('now'));
INSERT INTO t005_service_dates VALUES(1407374883553287,4785074604214097,date('now','+1 day'));

INSERT INTO t016_scheduled_services VALUES(4785074604183668,'1','00:00:00#00:22:00,00:22:10#00:22:10,00:22:20#00:22:20,00:22:25#00:00:00',2533274790397692,0,0,0,0);
INSERT INTO t005_service_dates VALUES(1407374883553288,4785074604183668,date('now'));
INSERT INTO t005_service_dates VALUES(1407374883553289,4785074604183668,date('now','+1 day'));


INSERT INTO t042_commercial_lines VALUES(11821949021891554,6192449487677434,'Network1-CommLine2','93','le train (ligne C)','bleuclair','bleuclair','');
INSERT INTO t009_lines VALUES(2533274790397693,11821949021891554,1125899906844436,'Network1-Line2',' 1.2','',0,1,1,1,0,2251799814850473,0,0,0,0,0);
INSERT INTO t010_line_stops VALUES(2814749767106564,3377699720880573,2533274790397693,0,1,0,6500.0,'');
INSERT INTO t010_line_stops VALUES(2814749767106565,3377699720880581,2533274790397693,1,1,1,12141.0,'');
INSERT INTO t010_line_stops VALUES(2814749767106566,3377699720880575,2533274790397693,2,0,1,12141.0,'');

INSERT INTO t016_scheduled_services VALUES(4785074604183676,'7','00:00:00#00:09:00,00:09:10#00:09:11,00:09:20#00:00:00',2533274790397693,0,0,0,0);
INSERT INTO t005_service_dates VALUES(1407374883553285,4785074604183676,date('now'));
INSERT INTO t005_service_dates VALUES(1407374883553284,4785074604183676,date('now','+1 day'));


INSERT INTO t042_commercial_lines VALUES(11821949021891555,6192449487677434,'Network1-CommLine3','94','','orange','orange','');
INSERT INTO t009_lines VALUES(2533274790397694,11821949021891555,1125899906844499,'Network1-Line3',' 1.3','',0,1,1,1,0,2251799814850473,0,0,0,0,0);
INSERT INTO t010_line_stops VALUES(2814749767106567,3377699720880583,2533274790397694,0,1,0,14141.0,'');
INSERT INTO t010_line_stops VALUES(2814749767106568,3377699720880585,2533274790397694,1,1,1,15141.0,'');
INSERT INTO t010_line_stops VALUES(2814749767106569,3377699720880580,2533274790397694,2,1,1,16141.0,'');
INSERT INTO t010_line_stops VALUES(2814749767106570,3377699720880576,2533274790397694,3,1,1,17141.0,'');
INSERT INTO t010_line_stops VALUES(2814749767106571,3377699720880573,2533274790397694,4,0,1,17641.0,'');

INSERT INTO t016_scheduled_services VALUES(4785074604183677,'9','00:00:00#00:10:05,00:10:10#00:00:00',2533274790397695,0,0,0,0);
INSERT INTO t005_service_dates VALUES(1407374883553290,4785074604183677,date('now'));
INSERT INTO t005_service_dates VALUES(1407374883553293,4785074604183677,date('now','+1 day'));


INSERT INTO t042_commercial_lines VALUES(11821949021891562,6192449487677437,'Network2-CommLine1','95','','rouge','rouge','');
INSERT INTO t009_lines VALUES(2533274790397695,11821949021891562,1125899906844501,'Network2-Line1',' 2.1','',0,1,1,1,0,2251799814850473,0,0,0,0,0);
INSERT INTO t010_line_stops VALUES(2814749767106572,3377699720880575,2533274790397695,0,1,0,17641.0,'');
INSERT INTO t010_line_stops VALUES(2814749767106573,3377699720880584,2533274790397695,1,0,1,17641.0,'');

INSERT INTO t016_scheduled_services VALUES(4785074604183678,'11','00:00:00#00:20:00,00:20:05#00:20:06,00:20:10#00:20:11,00:20:15#00:20:15,00:20:20#00:00:00',2533274790397694,0,0,0,0);
INSERT INTO t005_service_dates VALUES(1407374883553291,4785074604183678,date('now'));
INSERT INTO t005_service_dates VALUES(1407374883553292,4785074604183678,date('now','+1 day'));


INSERT INTO t042_commercial_lines VALUES(11821949021891563,6192449487677437,'Network2-CommLine2','96','','jaune','jaune','');
INSERT INTO t009_lines VALUES(2533274790397696,11821949021891563,1125899906844501,'Network2-Line2',' 96','',0,1,1,1,0,2251799814850473,0,0,0,0,0);
INSERT INTO t010_line_stops VALUES(2814749767106574,3377699720880579,2533274790397696,0,1,0,17641.0,'');
INSERT INTO t010_line_stops VALUES(2814749767106575,3377699720880585,2533274790397696,1,0,1,17641.0,'');

INSERT INTO t016_scheduled_services VALUES(4785074604183679,'13','00:00:00#00:19:55,00:20:05#00:00:00',2533274790397696,0,0,0,0);
INSERT INTO t005_service_dates VALUES(1407374883553297,4785074604183679,date('now'));
INSERT INTO t005_service_dates VALUES(1407374883553294,4785074604183679,date('now','+1 day'));

INSERT INTO t016_scheduled_services VALUES(4785074604183680,'14','00:00:00#00:20:00,00:20:25#00:00:00',2533274790397696,0,0,0,0);
INSERT INTO t005_service_dates VALUES(1407374883553295,4785074604183680,date('now'));
INSERT INTO t005_service_dates VALUES(1407374883553296,4785074604183680,date('now','+1 day'));

DELETE FROM t015_roads;
DELETE FROM t014_road_chunks;
INSERT INTO t015_roads(id,name,city_id) VALUES(4222124650659840,'40',1688849860508968);
INSERT INTO t015_roads(id,name,city_id) VALUES(4222124650659841,'41',1688849860508968);

DELETE FROM t011_place_aliases;


DELETE FROM t013_public_places;





DELETE FROM t018_pedestrian_compliances;

DELETE FROM t019_handicapped_compliances;

DELETE FROM t020_bike_compliances;

DELETE FROM t021_reservation_rules;

DELETE FROM t022_transport_networks;
INSERT INTO t022_transport_networks VALUES(6192449487677434,'Network1');
INSERT INTO t022_transport_networks VALUES(6192449487677437,'Network2');

DELETE FROM t025_sites;
INSERT INTO t025_sites(id,name,interface_id,start_date,end_date,online_booking,use_old_data,max_connections) VALUES(1,'test',2,'','',1,0,10);

COMMIT;
