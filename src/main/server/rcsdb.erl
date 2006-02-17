-module(rcsdb).


-export([start/0,stop/0]).
-export([import/0,install/0,backup/1,restore/1]).
%-export([putrec/1,getrec/1,delrec/1,getkey/1,delkey/1,match/3]).
-export([select/3,insert/2,update/3,delete/2]).
-export([count/2,max/3,nextid/1]).

-export([sql_query/1,sql_where/1]).

%-include_lib("stdlib/include/qlc.hrl").
-include("rcsdb.hrl").

-define(NODES,[db0@iBook,db1@iBook,db2@iBook]).
-define(TABLES,[personnes,profils,resa]).

% todo:
% faire system event -> bakup disque
% install(node) -> ajouter un noeud


%% service functions

start() ->
	mnesia:start(),
	mnesia:wait_for_tables(?TABLES,5000).

stop() ->
	mnesia:dump_tables(?TABLES),
	mnesia:stop().

%% maintainance functions

install() ->
	mnesia:create_schema(?NODES),
	lists:foreach(
		fun(N) -> 
			io:format("starting Mnesia on node ~s~n",[N]),
			rpc:call(N,mnesia,start,[]) 
		end,
		?NODES),
	lists:foreach(
		fun(Table) ->
			Record = case Table of
				% record_info is compile time only !
				% todo: generate dynamic tuple
				personnes -> record_info(fields,personnes);
				profils -> record_info(fields,profils);
				resa -> record_info(fields,resa)
			end,
			io:format("create table ~s~n",[Table]),
			{atomic,ok} = mnesia:create_table([
				{name,Table},
				{attributes,Record},
				{disc_copies,?NODES} ])
		end,
		?TABLES),
	ok.

import() ->
	not_implemented.

backup(File) ->
	mnesia:dump_to_textfile(File++".txt"),
	mnesia:backup(File).

restore(File) ->
	mnesia:restore(File,[]).

%% database actions

% add a record
putrec(Record) ->
	Trans = fun() ->
		mnesia:write(Record)
	end,
	{atomic,Status} = mnesia:transaction(Trans),
	Status.

% remove a key
delkey(Key) ->
	Trans = fun() ->
		mnesia:delete(Key)
	end,
	{atomic,Status} = mnesia:transaction(Trans),
	Status.

% remove a record
delrec(Record) ->
	Trans = fun() ->
		mnesia:delete_object(Record)
	end,
	{atomic,Status} = mnesia:transaction(Trans),
	Status.

% get a key
getkey(Key) ->
	Trans = fun() ->
		mnesia:read(Key)
	end,
	{atomic,Result} = mnesia:transaction(Trans),
	Result.

% get a record
getrec(Record) ->
	Trans = fun() ->
		mnesia:match_object(Record)
	end,
	{atomic,Result} = mnesia:transaction(Trans),
	Result.

% match a record with guards
match(Match,Guards,Get) ->	
	Trans = fun() ->
		mnesia:select(element(1,Match),[{Match,Guards,Get}])
	end,
	{atomic,Result} = mnesia:transaction(Trans),
	Result.

%% database record acces functions

% Alias position into List
aliaspos(Alias,List) -> aliaspos(Alias,1,List).
aliaspos(Alias,Acc,[Alias|_]) -> Acc;
aliaspos(Alias,Acc,[_|T]) -> aliaspos(Alias,Acc+1,T);
aliaspos(_,_,[]) -> 0.

% name of Alias into List
aliasname(N) when N < 10 -> lists:nth(N+1,['_','$1','$2','$3','$4','$5','$6','$7','$8','$9']);
aliasname(_) -> '_'.

% undefined record for Table
emptyrecord(Table) ->
	Size = mnesia:table_info(Table,arity) - 1,
	List = lists:duplicate(Size,undefined),
	list_to_tuple([Table]++List).

% full mask record for Table
tablerecord(Table) ->
	Size = mnesia:table_info(Table,arity) - 1,
	List = lists:map(fun aliasname/1,lists:seq(1,Size)),
	list_to_tuple([Table]++List).

% add a matching constraint to a table record
recordmatch({Field,'==',Value},Record,List) -> 
	Pos = aliaspos(Field,List) + 1,
	setelement(Pos,Record,Value);
recordmatch(_,Record,_) -> Record.

% full mask record for Table with constraint
tablematch(Table,Where) ->
	Record = tablerecord(Table),
	List = mnesia:table_info(Table,attributes),
	lists:foldl(fun(W,R) -> recordmatch(W,R,List) end,Record,Where).

% build alias list
fieldlist(Table,Fields) ->
	List = mnesia:table_info(Table,attributes),
	case length(Fields) of
		0 -> ['$_'];
		_ ->
			PosList = lists:map(fun(I) -> aliaspos(I,List) end,Fields),
			[lists:map(fun aliasname/1,PosList)]
	end.

% add a guard constraint
tableguard({Field,Op,Value},List) ->
	case lists:member(Op,['==','<','>']) of
		true -> {Op,aliasname(aliaspos(Field,List)),Value};
		false -> []
	end.

% build guard spec
guardlist(Table,Where) ->
	List = mnesia:table_info(Table,attributes), 
	lists:map(fun(W) -> tableguard(W,List) end,Where).

%% program actions

select(Table,Fields,Where) ->
	Match = tablerecord(Table),
	Get = fieldlist(Table,Fields),
	Guards = guardlist(Table,Where),
	match(Match,Guards,Get).

insert(Table,Values) ->
	List = mnesia:table_info(Table,attributes),
	New = lists:foldl(fun({Field,Value},Rec) -> 
		Pos = aliaspos(Field,List) + 1,
		setelement(Pos,Rec,Value)
		end,emptyrecord(Table),Values),
	putrec(New).

update(Table,Values,Where) ->
	List = mnesia:table_info(Table,attributes),
	From = select(Table,[],Where),
	lists:foreach(fun(Record) ->
		New = lists:foldl(fun({Field,Value},Rec) -> 
			Pos = aliaspos(Field,List) + 1,
			setelement(Pos,Rec,Value)
			end,Record,Values),
		putrec(New)
		end,From),
	length(From).

delete(Table,Where) ->
	%List = select(Table,[],Where),
	%lists:foreach(fun delrec/1,List).
	Match = tablematch(Table,Where), % works only for where '=='
	delrec(Match).

count(Table,Where) -> 
	length(select(Table,[id],Where)).

max(Table,Field,Where) -> 
	lists:max(lists:flatten(select(Table,[Field],Where))).

nextid(Table) -> 
	max(Table,id,[]) + 1.


%% misc SQL interpretation

locase_char(X) when $A =< X, X =< $Z -> X - $A + $a;
locase_char(X) -> X.
locase_word(X) -> lists:map(fun locase_char/1, X).
upcase_char(X) when $a =< X, X =< $z -> X + $A - $a;
upcase_char(X) -> X.
upcase_word(X) -> lists:map(fun upcase_char/1, X).

% attention au atom id leak !
list_to_record(Table,Values) ->
	{_,Record} = lists:foldl(fun(Value,{Pos,Rec}) ->
		{Int,_} = string:to_integer(Value),
		Real = case Int of
			error -> 
				Stripped = string:strip(Value,both,$'), % pour vim! '
				case length(Stripped) of
					0 -> undefined;
					_ -> Stripped
				end;
			_ -> Int
		end,
		{Pos+1,setelement(Pos,Rec,Real)}
		end,{2,emptyrecord(list_to_atom(Table))},Values),
	Record.

sql_query(SqlStr) ->
	Tokens = string:tokens(locase_word(SqlStr)," (),;"),
	case lists:nth(1,Tokens) of
	"insert" ->
		T = lists:nth(3,Tokens),
		S = string:substr(SqlStr,string:chr(SqlStr,$()+1),
		V = string:tokens(S,"(),;"),
		putrec(list_to_record(T,V));
	"update" ->
		T = lists:nth(2,Tokens),
		%% format à definir ici
		S = string:substr(SqlStr,string:chr(SqlStr,$()+1),
		V = string:tokens(S," =;"),
		putrec(list_to_record(T,V));
	_ ->
		badcommand
	end.

% ne marche que sur requete avec espaces et un seul where
% OK: where nom = "romain"
% KO: where nom="romain"
sql_where(SqlStr) ->
	Tokens = string:tokens(locase_word(SqlStr)," (),;"),
	case lists:nth(1,Tokens) of
	"where" ->
		[F,O,V] = lists:sublist(Tokens,2,3),
		Op = case O of
			"=" -> "==";
			_ -> 0
		end,
		"[{"++F++",'"++Op++"',"++V++"}]";
	_ ->
		"[]"
	end.

%% test injection function

%putrec(sql("INSERT INTO client VALUES(1,'romain','christophe','','','');")),
%putrec(sql("INSERT INTO client VALUES(2,'romain','hugues','','','');")),
%putrec(sql("INSERT INTO client VALUES(3,'toto','un test','','','');")),
%putrec(sql("INSERT INTO client VALUES(4,'match','un test','','','');")),
%putrec(sql("INSERT INTO client VALUES(5,'','','','','test@acme.com');")),
%delrec(#client{prenom="christophe",_='_'}),
%getrec(#client{nom="romain",_='_'}).


