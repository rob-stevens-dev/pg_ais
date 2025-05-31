# Makefile for pg_ais extension

EXT_NAME = pg_ais
EXT_VERSION = 0.3
DOCKER_COMPOSE = docker-compose
PG_DOCKER_SERVICE = pg_ais_dev
PG_USER = postgres
PG_DB = pg_ais

BUILD_DIR = build
SRC_DIR = src
TEST_DIR = test
SQL_DIR = sql

.PHONY: all build install clean test test_phase1 test_phase2 test_all regression sql_regression docker-up docker-down docker-rebuild psql reset-db

all: build

build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make

install:
	cd $(BUILD_DIR) && make install

clean:
	rm -rf $(BUILD_DIR)

test: test_all

test_phase1:
	cd $(BUILD_DIR) && ctest --output-on-failure

sql_regression:
	cd $(BUILD_DIR) && pg_regress \
		--load-extension=$(EXT_NAME) \
		--inputdir=sql --outputdir=sql --dbname=$(PG_DB) sql/$(EXT_NAME)_regression.sql

test_all: test_phase1 sql_regression

docker-up:
	$(DOCKER_COMPOSE) up -d --build

docker-down:
	$(DOCKER_COMPOSE) down --remove-orphans

docker-rebuild: docker-down
	$(DOCKER_COMPOSE) build --no-cache
	$(DOCKER_COMPOSE) up -d

psql:
	$(DOCKER_COMPOSE) exec -T $(PG_DOCKER_SERVICE) psql -U $(PG_USER) -d $(PG_DB)

reset-db:
	$(DOCKER_COMPOSE) exec -T $(PG_DOCKER_SERVICE) dropdb --if-exists $(PG_DB)
	$(DOCKER_COMPOSE) exec -T $(PG_DOCKER_SERVICE) createdb $(PG_DB)

rebuild: clean build
