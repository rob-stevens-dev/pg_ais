-- Load the extension
CREATE EXTENSION IF NOT EXISTS pg_ais;

-- Drop and recreate test table
DROP TABLE IF EXISTS test_ais;
CREATE TABLE test_ais (id serial, sentence ais);

-- Insert valid sentence
INSERT INTO test_ais(sentence)
VALUES ('!AIVDM,1,1,,A,15Muq60001G?tTpE>Gbk0?wN0<0,0*7D');

-- Check value and round-trip
SELECT id, sentence::text FROM test_ais;

-- Insert invalid sentence (should error)
-- Uncomment to test error handling
-- INSERT INTO test_ais(sentence) VALUES ('invalid sentence');

-- Comparison test
SELECT sentence = sentence FROM test_ais;

-- Debug output test
SELECT pg_ais_debug(sentence) FROM test_ais;
