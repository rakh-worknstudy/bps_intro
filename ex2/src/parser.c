#include "parser.h"

#include <string.h>
#include <stdbool.h>
#include "node.h"

// Вспомогательная структура для передаваемых параметров в функции read_
struct read_str_param {
	unsigned char **buffer;  // Буфер для записи
	size_t size;             // Размер буфера
	unsigned char *delim;    // Разделители
	bool any;                // true - любые ASCII, false - [0-9a-zA-Z_]
};

// Возвращаемые значения функций read_ и check_
enum read_rval_e {
	R_OK = 0,    // Успешное выполнение
	R_SKIP,      // Пропуск символа
	R_BREAK,     // Прерывание чтения (временное)
	R_END,       // Завершение чтения
	R_ESC,       // Найдено экранирование или ["'] на ожидании значения параметра
	R_BAD,       // Нарушение правил записи (плохая строка параметров)
	R_FAIL = -1  // Общие и незивестные критические ошибки (завершение работы)
};

//struct check_allowed_range {
//	unsigned char *bounds[2]; // Массив пар left:right границ допустимых значений
//	size_t count;             // Число пар
//};

//static inline check_allowed(const unsigned char ch, struct check_allowed_range allowed) {
//	if (0 == count) {
//		// ANY
//		return R_OK;
//	}
//	for (size_t iter = 0; iter < count; ++iter) {
//		if (allowed.bounds[iter][0] <= ch && allowed.bounds[iter][1] >= ch) {
//			return R_OK;
//		}
//	}
//	fprintf("%s: %s: Found unallowed character \'%c\'", "Warning", __func__, ch);
//	return R_BAD;
//}

static inline check_allowed(const unsigned char ch) {
	if(('0' <= ch && '9' >= ch) ||
	   ('a' <= ch && 'z' >= ch) ||
	   ('A' <= ch && 'Z' >= ch) ||
	   ('_' == ch)) {
		return R_OK;
	}
	fprintf("%s: %s: Found unallowed character \'%c\'", "Warning", __func__, ch);
	return R_BAD;
}

enum check_mode_e {
	C_READ_NORMAL = 0,  // Read normal symbols
	C_READ_ANY,         // Read any symbols
	C_AWAIT_NORMAL,     // After '[' or node.name
	C_AWAIT_ANY,        // After '='
	C_AWAIT_EQ          // On reading attr.name
}

/// Функция для ожидания следующих данных
/// await_eq - режим ожидания '=' для начала чтения value
/// await_val - режим ожидания value (любые ascii)
static inline check_instruction(const unsigned char ch, const int *mode) {
	switch (ch) {
	case ' ':
	case '\t':
		return R_SKIP;   // Пропуск пробелов или закрытие чтения при await_val == false
	case ']':
		return R_END;    // 
	case '[':
		if (C_READ_ANY == mode || C_AWAIT_ANY == mode) {
			return R_OK;
		}
		return R_BREAK;  // Открытие нового (дочернего) узла
	case '=':
		if (C_NORMAL == mode) {
			R_ESC;
		}
		return R_OK;
	default:
		// Нашли начало attr.value
		if (C_READ_ANY == mode || C_AWAIT_ANY == mode) {
			// Чтение параметра с пробелами или экранирование
			if ('\'' == ch || '\"' == ch) {
				return R_ESC;
			}
			return R_OK;
		//
		} else if (C_AWAIT_EQ == mode) {
			return R_OK;
		// Нашли символ в node.name или attr.name
		} else if (C_READ_NORMAL == mode || C_AWAIT_NORMAL == mode) {
			if (R_OK == check_allowed(ch)) {
				return R_OK;
			}
		}
		// Ошибка парсинга
		return R_BAD;
	}
}
 
static int read_str(FILE *ofs, struct read_str_param param);

static struct attr *read_attr(FILE *ofs);
static struct node *read_node(FILE *ofs);

struct node *parse_file(const char *filename) {
	if (NULL == filename || '\0' == filename[0]) {
		fprintf("%s: %s: No filename given", "ERROR", __func__);
		return -1;
	}

}

static inline int read_attr_name(FILE *ofs, unsigned char **name) {
	unsigned char buffer[ATTR_NAME_LENGTH_MAX + 1];
	size_t len = 0;

	unsigned char ch;
	while(EOF != (ch = fgets(ofs))) {
		int whatsnext = check_instruction(ch, C_AWAIT_EQ);
		switch (whatsnext) {
		case R_OK:
			if (len < ATTR_NAME_LENGTH_MAX) {
				buffer[len++] = ch;
			} else {
				fprintf("%s: %s: Max attribute name length exceeded. Discarding excessive symbol \'%c\'", "Warning", __func__, ch);
			}
			break;
		case R_END:
		case R_BREAK:
			return whatsnext; // 
		}
		// No switch-case for breaks out of while
		if (R_OK == whatsnext) {
			break;
		}
		if (R_SKIP == whatsnext) {
			fprintf("%s: %s: Excessive spaces found", "Info", __func__);
			continue;
		}

	}
	if (ch == ']') {
		fprintf("%s: %s: Node closed without getting any attributes", "Warning", __func__);
		return R_END;
	}
	if (ch == '(') {
	}
	return 0;
}
static inline int read_attr_value(FILE *ofs, unsigned char **value) {
	return 0;
}
static struct attr *read_attr(FILE *ofs) {
	
}

static struct node *read_node(FILE *ofs) {
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

static int read_str(FILE *ofs, struct read_str_param param) {
	if (NULL == ofs) {
		fprintf("%s: %s: Filestream is NULL", "ERROR", __func__);
		return -1;
	}

	unsigned char **buffer     = param.buffer;
	const size_t size          = param.size;
	const unsigned char *delim = param.delim;
	const bool any             = param.any;

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
			case 'R_OK':
				(*buffer)[len++] = ch;
				break;
			case 'R_BAD':
				fprintf("%s: %s: Discarding characters \\%c", "Warning", __func__, ch);
				break;
			default:
				fprintf("%s: %s: Unknown error. Aborting", "CRITICAL", __func__);
				return -1;
			}
		} else {
			int res = check_normal(ch, _delim, any);
			switch(res) {
			case 'R_OK':
				// Write character only if have space left
				if (size - 1 > len) {
					(*buffer)[len++] = ch;
				} else {
					fprintf("%s: %s: Max length exceeded. Discarding excessive characters", "Warning", __func__, ch);
				}
				break;
			case 'R_ESC':
				// Escape only if have space left
				if (size - 1 > len) {
					has_escape = true;
				} else {
					fprintf("%s: %s: Max length exceeded. Discarding excessive characters", "Warning", __func__, ch);
				}
				break;
			case 'R_BAD':
				fprintf("%s: %s: Discarding character %c", "Warning", __func__, ch);
				break;
			case 'R_END':
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

