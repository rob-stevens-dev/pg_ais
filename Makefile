EXTENSION = pg_ais
DATA = sql/pg_ais--0.1.sql
MODULE_big = pg_ais
OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))

PG_CONFIG := pg_config
PG_VERSION := $(shell $(PG_CONFIG) --version | awk '{print $$2}' | cut -d. -f1)
PG_REGRESS := /usr/lib/postgresql/$(PG_VERSION)/lib/pgxs/src/test/regress/pg_regress

PG_CPPFLAGS = -I$(srcdir)/src
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

.PHONY: docker-up docker-down docker-ps docker-rebuild docker-test docker-regression prepare-regression test-indexes docker-install docker-clean

# Prepare regression test structure
prepare-regression:
	mkdir -p sql/sql sql/expected
	cp test/pg_ais_regression.sql sql/sql/
	cp test/pg_ais_regression.out sql/expected/

# Bring up Docker environment
docker-up:
	docker-compose up -d

# Tear down containers
docker-down:
	docker-compose down --remove-orphans

# Show container status
docker-ps:
	docker-compose ps

# Clean rebuild, copy regression inputs, then start fresh container
docker-rebuild:
	docker-compose down --remove-orphans
	docker-compose build --no-cache
	docker-compose run --rm postgres make prepare-regression
	docker-compose up -d

# Run CMake/C unit tests
docker-test:
	docker-compose exec -T pg_ais_dev sh -c 'cd /app/build && ctest --output-on-failure'

# Run both SQL regression and index tests
docker-regression:
	docker-compose exec -T pg_ais_dev sh -c 'cd /app/sql && PGUSER=postgres $(PG_REGRESS) --inputdir=. --dbname=regression pg_ais_regression'
	docker-compose exec -T pg_ais_dev sh -c 'cd /app/test && PGUSER=postgres $(PG_REGRESS) --inputdir=. --dbname=regression pg_ais_indexes'

# Run indexing-specific SQL test only
test-indexes:
	docker-compose exec -T pg_ais_dev sh -c 'cd /app/test && PGUSER=postgres $(PG_REGRESS) --inputdir=. --dbname=regression pg_ais_indexes'

# Install the extension manually inside the container (dev use)
docker-install:
	docker-compose exec -T pg_ais_dev sh -c 'cd /app/build && make install && cp pg_ais.so /usr/lib/postgresql/$(PG_VERSION)/lib/'

# Drop the extension and remove build artifacts from container
docker-clean:
	docker-compose exec -T pg_ais_dev sh -c 'cd /app/build && make clean && rm -f /usr/lib/postgresql/$(PG_VERSION)/lib/pg_ais.so'
