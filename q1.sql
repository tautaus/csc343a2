-- VoteRange

SET SEARCH_PATH TO parlgov;
drop table if exists q1 cascade;

-- You must not change this table definition.

create table q1(
year INT,
countryName VARCHAR(50),
voteRange VARCHAR(20),
partyName VARCHAR(100)
);


-- You may find it convenient to do this for each of the views
-- total for party
create view total AS
    select party_id, country_id, date_part('year',e_date) as year ,votes/votes_valid as ratio as totalParty
    FROM election join election_result on election_result.election_id = election.id
    where date_part('year',e_date) >= 1996 and date_part('year',e_date) <= 2016
;


create view range AS
    select party_id,country_id,year,avg(ratio) as range
    from total
    GROUP BY party_id, country_id, year
    ;

create view infor AS
    select year, country.name as countryName , range, name_short as partyName
    from party, country, range
    where party.id = party_id and country.id =range.country_id
;

create view five as 
    select year, countryName, '(0,5]' as range, partyName
    from infor
    where range > 0 and range <= 0.05
    ;

create view ten as 
    select year, countryName, '(5,10]' as range, partyName
    from infor
    where range > 0.05 and range <= 0.1
    ;

create view twenty as 
    select year, countryName, '(10,20]' as range, partyName
    from infor
    where range > 0.1 and range <= 0.2
    ;

    create view thirty as 
    select year, countryName, '(20,30]' as range, partyName
    from infor
    where range > 0.2 and range <= 0.3
    ;

    create view forty as 
    select year, countryName, '(30,40]' as range, partyName
    from infor
    where range > 0.3 and range <= 0.4
    ;

    create view more as 
    select year, countryName, '(40,100]' as range, partyName
    from infor
    where range > 0.4
    ;

	 		
    INSERT into q1
    select year,countryName,range,partyName
    from five
    ;
    INSERT into q1
    select year,countryName,range,partyName
    from ten
    ;

    INSERT into q1
    select year,countryName,range,partyName
    from twenty
    ;

    INSERT into q1
    select year,countryName,range,partyName
    from thirty
    ;

    INSERT into q1
    select year,countryName,range,partyName
    from forty
    ;

    INSERT into q1
    select year,countryName,range,partyName
    from more
    ;

    drop view if exists more cascade;