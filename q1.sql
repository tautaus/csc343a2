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
create view total1 AS
    select party_id, country_id, date_part('year',e_date) as year ,sum(votes) as totalParty
    FROM election, election_result
    GROUP BY party_id, country_id, date_part('year',e_date)
;

-- total for country votes
create view total2 AS
    select party_id, country_id, date_part('year',e_date) as year, sum(votes_valid) as total
    From election join election_result on election_result.election_id = election.id
    group by party_id, country_id, date_part('year',e_date)
;

create view range AS
    select total1.party_id, total1.country_id,total1.year, totalParty/total as range
    From total1 join total2 on (total1.party_id = total2.party_id and total1.country_id = total2.country_id
    and total1.year = total2.year)
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
    drop view if exists total1 cascade;	    
