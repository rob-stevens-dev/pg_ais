-- Load the extension
CREATE EXTENSION IF NOT EXISTS pg_ais;

-- Regression test: clean table
DROP TABLE IF EXISTS test_ais;
CREATE TABLE test_ais (id serial, sentence ais);

-- ✅ 1. Valid multi-part message (reassembled)
SELECT pg_ais_parse('!AIVDM,2,1,2,B,53aGowP000001@D;E@E=:qD00000,0*5C');
SELECT pg_ais_parse('!AIVDM,2,2,2,B,00000000000,2*21');

-- ✅ 2. Out-of-order message (discarded)
SELECT pg_ais_parse('!AIVDM,2,2,9,B,00000000000,2*27');  -- arrives second part first
SELECT pg_ais_parse('!AIVDM,2,1,9,B,53aGowP000001@D;E@E=:qD00000,0*5C');

-- ✅ 3. Incomplete message (only one part)
SELECT pg_ais_parse('!AIVDM,2,1,3,B,53aGowP000001@D;E@E=:qD00000,0*5C');

-- ✅ 4. Point output
SELECT pg_ais_point('!AIVDM,1,1,,A,15Muq60001G?tTpE>Gbk0?wN0<0,0*7D');

-- ✅ 5. WKB hex output for point
SELECT encode(pg_ais_point('!AIVDM,1,1,,A,15Muq60001G?tTpE>Gbk0?wN0<0,0*7D'), 'hex');

-- ✅ 6. Round-trip parse-point validation
SELECT pg_ais_point('!AIVDM,1,1,,A,15Muq60001G?tTpE>Gbk0?wN0<0,0*7D') IS NOT NULL AS ok;