-- Load extension and create a table with some AIS messages
DROP TABLE IF EXISTS test_ais_index;
CREATE TABLE test_ais_index (id serial, sentence ais);

-- Sample messages with known MMSI and shipname
INSERT INTO test_ais_index(sentence) VALUES
('!AIVDM,1,1,,A,15Muq60001G?tTpE>Gbk0?wN0<0,0*7D'), -- MMSI 3669703
('!AIVDM,1,1,,A,15N`I80001G?tTpE>Gbk0?wN0<0,0*5A'); -- MMSI 3670984

-- Create indexes on parsed MMSI and shipname fields
CREATE INDEX test_ais_mmsi_idx ON test_ais_index ((pg_ais_get_int_field(sentence, 'mmsi')));
CREATE INDEX test_ais_shipname_idx ON test_ais_index ((pg_ais_get_text_field(sentence, 'shipname')));

-- Force planner to use the index (should not full scan)
SET enable_seqscan = OFF;

-- Run indexed query on MMSI
EXPLAIN SELECT * FROM test_ais_index
WHERE pg_ais_get_int_field(sentence, 'mmsi') = 3669703;

-- Run indexed query on shipname (will be uppercase, trimmed)
EXPLAIN SELECT * FROM test_ais_index
WHERE pg_ais_get_text_field(sentence, 'shipname') = 'MAERSK';

-- Reset planner settings
RESET enable_seqscan;
