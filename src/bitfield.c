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
    return parse_uint(payload, start, 10) / 10.0;
}

double parse_heading(const char *payload, int start) {
    return parse_uint(payload, start, 9);
}

char *parse_string(const char *payload, int start, int len) {
    int byte_len = (len + 5) / 6;
    char *out = malloc(byte_len + 1);
    for (int i = 0; i < byte_len; i++) {
        out[i] = sixbit_to_uint(payload[i + start / 6]) + 48;
    }
    out[byte_len] = '\0';
    return out;
}
