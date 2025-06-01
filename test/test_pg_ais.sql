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


-- Text field extraction testing.
DROP TABLE IF EXISTS test_text_field;
CREATE TABLE test_text_field (id serial, sentence ais);

-- Insert a sample AIS message with shipname
INSERT INTO test_text_field(sentence) VALUES
('!AIVDM,1,1,,B,H42Owk@2BDPh3@<T4pM00000000,0*6D');

-- Validate access to the shipname field
SELECT id, pg_ais_get_text_field(sentence, 'shipname') AS shipname FROM test_text_field;

-- Validate access to an unsupported field (expect NULL)
SELECT pg_ais_get_text_field(sentence, 'foobar') AS should_be_null FROM test_text_field;


-- pg_get_int_field tests
-- Insert a type 1 message (positional report with int fields)
INSERT INTO test_text_field(sentence) VALUES
('!AIVDM,1,1,,B,13aG?P0P00PD;88MD5MTDww@2D0T,0*1C');

-- Validate integer field access
SELECT pg_ais_get_int_field(sentence, 'mmsi') FROM test_text_field;
SELECT pg_ais_get_int_field(sentence, 'heading') FROM test_text_field;
SELECT pg_ais_get_int_field(sentence, 'nav_status') FROM test_text_field;
SELECT pg_ais_get_int_field(sentence, 'foobar') FROM test_text_field;


-- Validate float field access
SELECT pg_ais_get_float_field(sentence, 'speed') FROM test_text_field;
SELECT pg_ais_get_float_field(sentence, 'course') FROM test_text_field;
SELECT pg_ais_get_float_field(sentence, 'foobar') FROM test_text_field;