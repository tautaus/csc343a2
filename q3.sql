-- Participate

SET SEARCH_PATH TO parlgov;
drop table if exists q3 cascade;

-- You must not change this table definition.

create table q3(
        countryName varchar(50),
        year int,
        participationRatio real
);

create view ratio as 
select country_id, year, sum(votes_cast)/sum(electorate) as ratio
from election
group by (country_id, date_part('year',e_date) as year)
;

create view ct as
select DISTINCT(r1.country_id)
from ratio r1, ratio r2
where r1.country_id = r2.country_id and r1.year < r2.year and r1.ratio <= r2.ratio and r1.year >= 2001 and r1.year <= 2016
;

create view ctt as
select ct.country_id, country.name
from ct join country on ct.country_id = country.id
;

create view answer as
select name,year,ratio
from ctt join ratio on ctt.country_id = ratio.country_id
;

-- You may find it convenient to do this for each of the views
-- that define your intermediate steps.  (But give them better names!)
DROP VIEW IF EXISTS intermediate_step CASCADE;

-- Define views for your intermediate steps here.


-- the answer to the query 
insert into q3(countryName,year,participationRatio) 
select name,year,ratio
from answer
;