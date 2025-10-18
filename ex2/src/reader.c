#include "reader.h"

#include <string.h>
#include <stdbool.h>

// Check for "0-9a-zA-Z_"
static inline int check_allowed(const unsigned char questionable) {
	if (('0' <= questionable && '9' >= questionable) ||
	    ('a' <= questionable && 'z' >= questionable) ||
    	    ('A' <= questionable && 'Z' >= questionable) ||
	    ('_' == questionable)) {
		return 0;
	} else {
		fprintf("%s: %s: Bad characer \'%c\' found", "ERROR", __func__, questionable);
		return -1;
	}
}

// check_normal/onescape return values
enum check_rcode {
	OK = 0,
	ESC,
	END,
	BAD = -1
};

// Check symbol when no escape is given
static inline int check_normal(const unsigned char ch, const unsigned char *delim, bool any) {
	// '\' -> escape
	if ('\\' == ch) {
		return ESC;
	}
	if (NULL != strchr(delim, ch)) {
		return END;
	}
	if (any || 0 == check_allowed(ch)) {
		return OK;
	}
	return BAD;
}
// Check symbol after escape
static inline int check_onescape(const unsigned char ch, const unsigned char *delim) {
	if (' ' != ch && NULL != strchr(delim, ch)) {
		return OK;
	}
	// '\\' -> '\'
	if ('\' == ch) {
		return OK;
	}
	fprintf("%s: %s: Bad character \'%c\' after \'\\\'", "ERROR", __func__, ch);
	return BAD;
}

static int read_str_impl(FILE *ofs, unsigned char *buffer[], const size_t size, const unsigned char *delim, bool any) {
	if (NULL == ofs) {
		fprintf("%s: %s: Filestream is NULL", "ERROR", __func__);
		return -1;
	}
	if (NULL == buffer) {
		fprintf("%s: %s: Buffer is NULL", "ERROR", __func__);
		return -1;
	}
	if (size == 0) {
		fprintf("%s: %s: Buffer size is 0. Aborting", "ERROR", __func__);
		return -1;
	}
	if (size == 1) {
		fprintf("%s: %s: Buffer size is 1. Return \"\\0\"", "Warning", __func__);
		(*buffer)[0] = '\0';
		return 0;
	}

	// Use default ' ' delimiter if delim is empty
	const unsigned char *_delim = (0 != strnlen(delim, 2)) ? delim : " ";

	size_t len = 0;
	unsigned char ch;
	while(EOF != (unsigned char ch = fgetc(ofs))) {
		if (has_escape) {
			has_escape = false;
			int res = check_onescape(ch, _delim);
			switch(res) {
			case 'OK':
				(*buffer)[len++] = ch;
				break;
			case 'BAD':
				fprintf("%s: %s: Discarding characters \\%c", "Warning", __func__, ch);
				break;
			default:
				fprintf("%s: %s: Unknown error. Aborting", "CRITICAL", __func__);
				return -1;
			}
		} else {
			int res = check_normal(ch, _delim, any);
			switch(res) {
			case 'OK':
				// Write character only if have space left
				if (size - 1 > len) {
					(*buffer)[len++] = ch;
				} else {
					fprintf("%s: %s: Max length exceeded. Discarding excessive characters", "Warning", __func__, ch);
				}
				break;
			case 'ESC':
				// Escape only if have space left
				if (size - 1 > len) {
					has_escape = true;
				} else {
					fprintf("%s: %s: Max length exceeded. Discarding excessive characters", "Warning", __func__, ch);
				}
				break;
			case 'BAD':
				fprintf("%s: %s: Discarding character %c", "Warning", __func__, ch);
				break;
			case 'END':
				break;
			default:
				fprintf("%s: %s: Unknown error. Aborting", "CRITICAL", __func__);
				return -1;
			}
		}
		if (size - 1 > len) {
			(*buffer)[len] = '\0';
			break;
		}
	}
	
	// Got string
	return 0;
}

int read_str(FILE *ofs, unsigned char *buffer[], const size_t size, const unsigned char *delim) {
	return read_str_impl(ofs, buffer, size, delim, false);
}

int read_str_any(FILE *ofs, unsigned char *buffer[], const size_t size, const unsigned char *delim) {
	return read_str_impl(ofs, buffer, size, delim, true);	
}

