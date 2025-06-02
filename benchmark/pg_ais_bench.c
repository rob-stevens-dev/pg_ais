#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pg_ais.h"
#include "ais_core.h"
#include "parse_ais_msg.h"
#include "pg_ais_core.h"
#include "pg_ais_metrics.h"


#define MAX_LINE_LEN 1024


/**
 * @brief Benchmark parser performance using a file of AIS sentences.
 *
 * Reads newline-delimited AIS messages from a text file and parses each using
 * the full pg_ais extension pipeline. Tracks total messages parsed, elapsed time,
 * and throughput, and prints internal parse/reassembly metrics.
 *
 * This benchmark reflects real-world use cases such as COPY-based bulk ingest
 * and can be used to measure parser throughput under high-load conditions.
 *
 * @param filepath Path to the input file containing one AIS sentence per line
 */
static void benchmark_parse_file(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open file: %s\n", filepath);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LEN];
    size_t count = 0;

    clock_t start = clock();

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || strlen(line) < 6)
            continue;

        // Strip newline
        line[strcspn(line, "\r\n")] = 0;

        ais *datum = ais_from_cstring_external(line);
        if (!datum)
            continue;

        AISMessage msg = {0};
        pg_ais_parse(datum, &msg);
        free_ais_message(&msg);
        AIS_FREE(datum);

        count++;
    }

    clock_t end = clock();
    fclose(fp);

    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    double rate = (elapsed > 0) ? (count / elapsed) : 0;

    printf("Parsed %zu messages in %.2f sec (%.0f msg/sec)\n", count, elapsed, rate);
    printf("--- Internal Metrics ---\n");
    printf("%-30s %lu\n", "total_messages_parsed:", total_messages_parsed);
    printf("%-30s %lu\n", "total_parse_failures:", total_parse_failures);
    printf("%-30s %lu\n", "total_reassembly_attempts:", total_reassembly_attempts);
    printf("%-30s %lu\n", "total_reassembly_success:", total_reassembly_success);
}


/**
 * @brief Benchmark parser performance using a file of AIS sentences.
 *
 * Reads newline-delimited AIS messages from a text file and parses each using
 * the full pg_ais extension pipeline. Tracks total messages parsed, elapsed time,
 * and throughput, and prints internal parse/reassembly metrics.
 *
 * This benchmark reflects real-world use cases such as COPY-based bulk ingest
 * and can be used to measure parser throughput under high-load conditions.
 *
 * @param filepath Path to the input file containing one AIS sentence per line
 */
int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: %s <ais_file.txt>\n", argv[0]);
        printf("  Each line should be a full !AIVDM sentence.\n");
        return EXIT_SUCCESS;
    }
    
    benchmark_parse_file(argv[1]);
    return EXIT_SUCCESS;
}
