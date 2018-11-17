-- Winners

SET SEARCH_PATH TO parlgov;
drop table if exists q2 cascade;

-- You must not change this table definition.

create table q2(
countryName VARCHaR(100),
partyName VARCHaR(100),
partyFamily VARCHaR(100),
wonElections INT,
mostRecentlyWonElectionId INT,
mostRecentlyWonElectionYear INT
);

create view winner as 
select election_id, party_id
From election_result
where exists(
    select *
    from election_result er 
    where election_result.votes = max(er.votes) and election_result.election_id = er.election_id
    group by er.election_id
)；

create view winner1 AS
select party_id,count(*) as counts
from winner
group by party_id
;

create view countryavg as 
select country_id, avg(counts) + 3 as average
from winner1 JOIN party on winner1.party_id = party.id
GROUP BY country_id
;

create view countrywin AS
select party.country_id, winner1.party_id,counts
from (winner1 JOIN party on winner1.party_id = party.id) join countryavg on party.country_id = countryavg.country_id
where counts > average
;

create view recent_winner as
select party_id, max(election_id) as election_id
from winner
group by (party_id)
;

create view recent_year AS
select party_id, election_id, date_part('year',e_date) as year
from recent_winner, election
where recent_winner.id = election.id
;

create view party_infor AS
select party.id as party_id, name as partyName, country.name as countryName, party_family.family as partyFamily
from (party join country on party.country_id = country.id) join party_family on party.id = party_family.party_id
;


create view answer AS
select countryName, partyName, partyFamily, counts as wonElections, election_id, year
from (recent_year join countrywin on recent_year.party_id = countrywin.party_id) 
    join party_infor on countrywin.party_id = party_infor.party_id
;

-- You may find it convenient to do this for each of the views
-- that define your intermediate steps.  (But give them better names!)


-- Define views for your intermediate steps here.


-- the answer to the query 
insert into q2 
select countryName, partyName, partyFamily, wonElections,election_id, year
from answer
;

DROP VIEW IF EXISTS answer CASCADE;