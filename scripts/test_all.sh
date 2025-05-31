#!/bin/bash
set -euo pipefail

echo "🔧 Rebuilding extension inside container..."
docker-compose run --rm postgres bash -c "
  cmake -S . -B build &&
  cmake --build build &&
  cd build && ctest --output-on-failure
"

UNIT_EXIT=$?

if [ $UNIT_EXIT -eq 0 ]; then
  echo -e '\033[1;32m✅ Unit tests passed\033[0m'
else
  echo -e '\033[1;31m❌ Unit tests failed\033[0m'
  exit 1
fi

echo "🧪 Running SQL regression test..."
docker-compose exec -T postgres psql -U postgres -c "DROP DATABASE IF EXISTS pg_ais_test_db; CREATE DATABASE pg_ais_test_db;"
docker-compose exec -T postgres psql -U postgres -d pg_ais_test_db -c "CREATE EXTENSION pg_ais;"
docker-compose exec -T postgres psql -U postgres -d pg_ais_test_db -f /app/test/pg_ais_regression.sql > /tmp/pg_ais_regression.out || true

if diff -u test/pg_ais_regression.out /tmp/pg_ais_regression.out; then
  echo -e '\033[1;32m✅ SQL regression test passed\033[0m'
else
  echo -e '\033[1;31m❌ SQL regression test failed\033[0m'
  exit 1
fi

echo -e '\033[1;34m🎉 All tests passed!\033[0m'
