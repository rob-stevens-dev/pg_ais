EXTENSION = pg_ais
DATA = sql/pg_ais--0.2.sql
PG_CONFIG = pg_config
MODULES = pg_ais
REGRESS =

build:
	cmake -S . -B build
	cmake --build build

install:
	make -C build install

docker-up:
	docker-compose -f docker-compose.yml up -d

docker-down:
	docker-compose -f docker-compose.yml down

test_phase1:
	cd build && ctest --output-on-failure

sql_regression:
	docker-compose exec -T postgres psql -U postgres -c "DROP DATABASE IF EXISTS pg_ais_test_db; CREATE DATABASE pg_ais_test_db;"
	docker-compose exec -T postgres psql -U postgres -d pg_ais_test_db -c "CREATE EXTENSION pg_ais;"
	docker-compose exec -T postgres psql -U postgres -d pg_ais_test_db -f /app/test/pg_ais_regression.sql > /tmp/pg_ais_regression.out
	diff -u test/pg_ais_regression.out /tmp/pg_ais_regression.out || (echo "SQL regression test failed." && exit 1)

test_all:
	./scripts/test_all.sh

psql:
	docker-compose exec -it postgres psql -U postgres -d pg_ais_dev

clean:
	rm -rf build
