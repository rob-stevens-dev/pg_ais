// File: src/bitfield.h
#ifndef BITFIELD_H
#define BITFIELD_H

#include <stdint.h>

uint32_t parse_uint(const char *payload, int start, int len);
double parse_lat(const char *payload, int start);
double parse_lon(const char *payload, int start);
double parse_speed(const char *payload, int start);
double parse_heading(const char *payload, int start);
char *parse_string(const char *payload, int start, int len);

#endif
