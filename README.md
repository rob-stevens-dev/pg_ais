# pg\_ais

PostgreSQL extension for storing, validating, and eventually parsing and indexing NMEA 0183 v4.0+ AIS messages. Designed for high-performance marine tracking and time-series ingestion in PostgreSQL or TimescaleDB.

---

## 📦 Features

* ✅ Custom PostgreSQL `ais` type
* ✅ Internally stored as VARLENA binary
* ✅ Accepts and validates raw NMEA AIS sentences (must start with `!`)
* ✅ Supports input/output casting and equality comparison
* ✅ CMocka-based unit test coverage
* ✅ SQL regression testing support
* 🗽 Future: full NMEA 0183 message parser + binary generator
* 📍 Future: integration with PostGIS for geospatial indexing
* 📈 Future: support for MMSI/IMO-based indexing and query optimization

---

## 💠 Building (Manual)

If you are not using Docker:

```bash
mkdir build && cd build
cmake ..
make
sudo make install
```

To install the extension in PostgreSQL:

```sql
CREATE EXTENSION pg_ais;
```

---

## 🐳 Docker-Based Development

### Build and Install Extension

```bash
docker-compose build
```

### Start PostgreSQL Dev Container

```bash
make docker-up
```

### Shut Down PostgreSQL Dev Container

```bash
make docker-down
```

---

## 💪 Testing

### Run All Tests (Unit + SQL Regression)

```bash
make test_all
# OR
./scripts/test_all.sh
```

This will:

* Rebuild the extension inside Docker
* Run CMocka unit tests (`pg_ais_tests`)
* Run SQL regression tests and diff output

### Run Unit Tests Only

```bash
make test_phase1
```

### Run SQL Regression Test Only

```bash
make sql_regression
```

---

## 💡 Example Usage: AIS Type

Once the extension is installed:

```sql
-- Load the extension
CREATE EXTENSION pg_ais;

-- Create a table using the `ais` type
CREATE TABLE ais_messages (
    id serial PRIMARY KEY,
    sentence ais
);

-- Insert a valid NMEA 0183 AIS sentence
INSERT INTO ais_messages(sentence)
VALUES ('!AIVDM,1,1,,A,15Muq60001G?tTpE>Gbk0?wN0<0,0*7D');

-- Query the raw sentence
SELECT id, sentence::text FROM ais_messages;
```

---

## 📎 Project Structure

```text
pg_ais/
├── src/                 # C source files (pg_ais.c, pg_ais_core.c)
├── test/                # CMocka and SQL regression tests
├── sql/                 # SQL extension definition (pg_ais--0.2.sql)
├── scripts/             # Test harness and helper scripts
├── docker/              # Dockerfile and config
├── docker-compose.yml   # Local Postgres dev container
├── Makefile             # Project commands
└── README.md
```

---

## 🔮 Roadmap

* [ ] Full AIS sentence parser (type-aware, all message classes)
* [ ] Binary encoder for outbound messages
* [ ] Multi-part AIS support (message types 5, 24, etc.)
* [ ] Geospatial integration with PostGIS
* [ ] Index support for MMSI, IMO, and timestamp fields
* [ ] JSON/JSONB output format support
* [ ] TimescaleDB optimization

---

## 🪪 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
