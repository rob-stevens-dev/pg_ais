-- Load the extension
CREATE EXTENSION pg_ais;

-- Functional test for the ais type
DROP TABLE IF EXISTS test_ais;
CREATE TABLE test_ais (id serial, sentence ais);

INSERT INTO test_ais(sentence) VALUES
('!AIVDM,1,1,,A,15Muq60001G?tTpE>Gbk0?wN0<0,0*7D');

-- Confirm round-trip output
SELECT id, sentence::text FROM test_ais ORDER BY id;

-- Test comparison
SELECT sentence = sentence AS comparison FROM test_ais;

SELECT pg_ais_debug(sentence) FROM test_ais;
