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

-- Optionally reset fragment buffer
-- SELECT pg_ais_reset();


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


-- Test Lat/Lon extraction to point
-- Insert a positional message (type 1) with valid lat/lon
INSERT INTO test_text_field(sentence) VALUES
('!AIVDM,1,1,,B,13aG?P0P00PD;88MD5MTDww@2D0T,0*1C');

-- Validate point extraction
SELECT pg_ais_point(sentence) FROM test_text_field;

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


-- Validate boolean field access
SELECT pg_ais_get_bool_field(sentence, 'raim') FROM test_text_field;
SELECT pg_ais_get_bool_field(sentence, 'accuracy') FROM test_text_field;
SELECT pg_ais_get_bool_field(sentence, 'foobar') FROM test_text_field;