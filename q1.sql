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




create VIEW zeroToFive AS
    SELECT date_part('year',e_date) as year, country.name as countryName, '(0,5]' as voteRange, party.name as partyName

    FROM ((party join election_result on party.id = election_result.party_id) 
                join election on election_result.election_id = election.id)
                join country on country.id = election.country_id
    WHERE election_result.votes/election.votes_valid > 0 and 
        election_result.votes/election.votes_valid <= 0.05

-- that define your intermediate steps.  (But give them better names!)
-- DROP VIEW IF EXISTS intermediate_step CASCADE;

-- Define views for your intermediate steps here.


-- the answer to the query 




-- insert into q1 


