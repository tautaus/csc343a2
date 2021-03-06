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

DROP VIEW IF EXISTS winline,winner,winner1,partynum,countrytotal,countryavg,countrywin,winner_election,recent_winner,recent_year,party_infor,answer CASCADE;
create view winline as
select election_id, max(votes) as line
from election_result
group by election_id
; 

create view winner as 
select election_result.election_id, party_id
From election_result, winline
where election_result.election_id = winline.election_id and votes = line
;

create view winner1 AS
select party_id,count(*) as counts
from winner
group by party_id
;

create view partynum as
select country_id, count(*) as num
from party
group by country_id
;

create view countrytotal as 
select party.country_id, sum(counts) as total
from winner1 join party on winner1.party_id = party.id
GROUP BY party.country_id
;

create view countryavg as
select partynum.country_id, 3*cast(total as real)/num as average
from partynum join countrytotal on partynum.country_id = countrytotal.country_id
;

create view countrywin AS
select party.country_id, winner1.party_id,counts,party.name
from (winner1 JOIN party on winner1.party_id = party.id) join countryavg on party.country_id = countryavg.country_id
where counts > average
;


create view winner_election as
select party_id, winner.election_id, e_date
from winner join election on winner.election_id = election.id
;

create view recent_winner AS
select party_id, max(e_date) as date
from winner_election
group by party_id
;

create view recent_year AS
select recent_winner.party_id, winner_election.election_id,date
from recent_winner join winner_election on recent_winner.party_id = winner_election.party_id and recent_winner.date = winner_election.e_date
;


create view party_infor AS
select countrywin.party_id as party_id, countrywin.name as partyName, country.name as countryName, party_family.family as partyFamily,counts
from (countrywin join country on countrywin.country_id = country.id) left join party_family on countrywin.party_id = party_family.party_id
;

 
create view answer AS
select countryName, partyName, partyFamily, counts as wonElections, election_id, date_part('year',date) as year
from recent_year join party_infor on recent_year.party_id = party_infor.party_id
;

-- You may find it convenient to do this for each of the views
-- that define your intermediate steps.  (But give them better names!)


-- Define views for your intermediate steps here.


-- the answer to the query 
insert into q2 
select countryName, partyName, partyFamily, wonElections,election_id, year
from answer
;
