#include "bitfield.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static uint8_t sixbit_to_uint(char c) {
    if (c < 48 || c > 119) return 0;
    c -= 48;
    if (c > 40) c -= 8;
    return c;
}

uint32_t parse_uint(const char *payload, int start, int len) {
    uint32_t value = 0;
    for (int i = 0; i < len; i++) {
        int bit_idx = start + i;
        int byte_idx = bit_idx / 6;
        int bit_off = 5 - (bit_idx % 6);
        uint8_t b = sixbit_to_uint(payload[byte_idx]);
        value <<= 1;
        value |= (b >> bit_off) & 1;
    }
    return value;
}

double parse_lat(const char *payload, int start) {
    int val = (int) parse_uint(payload, start, 27);
    if (val & (1 << 26)) val -= (1 << 27);
    return val / 600000.0;
}

double parse_lon(const char *payload, int start) {
    int val = (int) parse_uint(payload, start, 28);
    if (val & (1 << 27)) val -= (1 << 28);
    return val / 600000.0;
}

double parse_speed(const char *payload, int start) {
    uint32_t raw = parse_uint(payload, start, 10);
    if (raw == 1023) return -1.0;
    return raw / 10.0;
}

double parse_heading(const char *payload, int start) {
    uint32_t raw = parse_uint(payload, start, 9);
    if (raw == 511) return -1.0;
    return raw;
}

char *parse_string(const char *payload, int start, int len) {
    int byte_len = (len + 5) / 6;
    char *out = malloc(byte_len + 1);
    if (!out) return NULL;

    for (int i = 0; i < byte_len; i++) {
        int sixbit = parse_uint(payload, start + i * 6, 6);
        char decoded = (sixbit == 0) ? ' ' : sixbit_ascii[sixbit & 0x3F];
        out[i] = decoded;
    }
    out[byte_len] = '\0';

    // Trim trailing spaces
    for (int i = byte_len - 1; i >= 0; i--) {
        if (out[i] == ' ') {
            out[i] = '\0';
        } else {
            break;
        }
    }

    return out;
}
