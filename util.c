#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


static int is_identifier_char(char c) {
    return isalnum(c) || c == '_';
}

char* fingerprint_string(const char *str) {
    char *fingerprinted = (char *)malloc(strlen(str) + 1);
    if (!fingerprinted) return NULL;

    const char *src = str;
    char *dst = fingerprinted;
    int in_quotes = 0;

    while (*src) {
        // Detect start or end of a quoted string (single or double quotes)
        if (*src == '\'' || *src == '"') {
            if (in_quotes == *src) {
                in_quotes = 0; // End of string
            } else if (in_quotes == 0) {
                in_quotes = *src; // Start of string
                *dst++ = '?'; // Replace entire string with "?"
            }
        }

        // Replace numeric literals with "?"
        else if (!in_quotes && isdigit(*src)) {
            *dst++ = '?';
            while (isdigit(*src)) src++; // Skip entire number
            continue; // Prevent double increment
        }

        // Copy non-literal characters
        else if (!in_quotes || is_identifier_char(*src)) {
            *dst++ = tolower(*src);
        }

        src++;
    }

    *dst = '\0'; // Null-terminate the fingerprinted query
    return fingerprinted;
}

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
