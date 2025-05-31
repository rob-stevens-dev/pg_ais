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

.PHONY: docker-up docker-down docker-ps docker-rebuild docker-test docker-regression

prepare-regression:
	mkdir -p sql/sql sql/expected
	cp test/pg_ais_regression.sql sql/sql/
	cp test/pg_ais_regression.out sql/expected/

docker-up:
	docker-compose up -d

docker-down:
	docker-compose down --remove-orphans

docker-ps:
	docker-compose ps

docker-rebuild:
	docker-compose down --remove-orphans
	docker-compose build --no-cache
	docker-compose run --rm postgres make prepare-regression
	docker-compose up -d

docker-test:
	docker-compose exec -T pg_ais_dev sh -c 'cd /app/build && ctest --output-on-failure'

docker-regression:
	docker-compose exec -T pg_ais_dev sh -c 'cd /app/sql && PGUSER=postgres $(PG_REGRESS) --inputdir=. --dbname=regression pg_ais_regression'
