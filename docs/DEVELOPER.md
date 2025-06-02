# Developer Guide

- All parsing logic in `parse_ais_msg.c`
- Enum decoding in `ais_core.c`
- Safe parsing via `ParseResult` pattern
- Use `pg_ais_bench` to measure performance

## Run Tests

```bash
make docker-test
make docker-regression
```

## Benchmark

```bash
make benchmark
./pg_ais_bench test/ais_test_payloads.txt
```
