-- Left-right

SET SEARCH_PATH TO parlgov;
drop table if exists q4 cascade;

-- You must not change this table definition.


CREATE TABLE q4(
        countryName VARCHAR(50),
        r0_2 INT,
        r2_4 INT,
        r4_6 INT,
        r6_8 INT,
        r8_10 INT
);

create view infor AS
select country.name as name, party_position.party_id, left_right
from (country join party on country.id = party.country_id) join party_position on party_position.party_id = party.id
;

create view count1 AS
select name, count(*) as c1
from infor
where left_right >= 0 and left_right < 2
group by name
;

create view count2 AS
select name, count(*) as c2
from infor
where left_right >= 2 and left_right < 4
group by name
;

create view count3 AS
select name, count(*) as c3
from infor
where left_right >= 4 and left_right < 6
group by name
;

create view count4 AS
select name, count(*) as c4
from infor
where left_right >= 6 and left_right < 8
group by name
;

create view count5 AS
select name, count(*) as c5
from infor
where left_right >= 8 and left_right < 10
group by name
;

create view answer AS
select count1.name, c1, c2, c3, c4, c5
from count1, count2, count3, count4, count5
where count1.name = count2.name, count2.name = count3.name, count3.name = count4.name , count4.name = count5.name
;

-- You may find it convenient to do this for each of the views
-- that define your intermediate steps.  (But give them better names!)

-- Define views for your intermediate steps here.


-- the answer to the query 
INSERT INTO q4 
SELECT name,c1,c2,c3,c4,c5
from answer
;

DROP VIEW IF EXISTS answer CASCADE;
