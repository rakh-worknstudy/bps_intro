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
static int check_instruction(const unsigned char ch, const int *mode) {
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

char *init_str_from_str(const char *src, const size_t lmax) {
	char *dst = NULL;
	size_t len;

	if (NULL != src) {
		len = strnlen(src, lmax);
	} else {
		perror("Warning: init_str_from_str(): src is NULL");
		len = 0;	
	}

	dst = (char *)malloc(len + 1);
	if (NULL == dst) {
		perror("ERROR: init_str_from_str(): Failed to init a string");
		return NULL;
	}
	
	if (NULL != src) {
		strncpy(dst, src, len);
	}
	dst[len] = '\0';
	return dst;
}

struct node *parse_file(const char *filename) {
	if (NULL == filename || '\0' == filename[0]) {
		fprintf("%s: %s: No filename given", "ERROR", __func__);
		return -1;
	}

}

static inline unsigned char read_attr_name(FILE *ofs, unsigned char *name, size_t *len) {
	*len = 0;

	unsigned char ch;
	while(EOF != (ch = fgetc(ofs))) {
		if (R_OK == check_allowed(ch)) {
			if (ATTR_NAME_LENGTH_MAX < *len) {
				name[(*len)++] = ch;
			} else {
				fprintf("%s: %s: Character \'%c\' exceeds attribute name length. Discaring", "Warning", __func__, ch);
			}
		} else if (ch == '=') {
			break;
		} else if (ch == ']' || ch == '[') {
			if (0 == *len) {
				fprintf("%s: %s: Node closed without getting any attributes", "Info", __func__);
			} else {
				fprintf("%s: %s: Got an attribute without a value", "Info", __func__);
			}
			break;
		} else if (ch == ' ' || ch == '\t') {
			if (0 != len) {
				break;	// End of attr.name
			}
		} else {
			fprintf("%s: %s: Got a bad character \'%c\'. Discarding", "Warning", __func__, ch);
		}	
	}

	name[*len] = '\0';
	return ch;
}

static unsigned char read_attr_value(FILE *ofs, unsigned char *value, size_t *len) {
	*len = 0;

	unsigned char ch;
	while(EOF != (ch = fgetc(ofs))) {
		if (R_OK == check_allowed(ch)) {
			if (ATTR_VALUE_LENGTH_MAX < *len) {
				name[(*len)++] = ch;
			} else {
				fprintf("%s: %s: Character \'%c\' exceeds attribute value length. Discarding", "Warning", __func__, ch);
			}
		} else if (ch == '=') {

		}
	}

	return 0;
}
static unsigned char read_attr(FILE *ofs, struct attr *attr) {
	attr->name = NULL;
	attr->value = NULL;
	attr->next = NULL;

	unsigned char name[ATTR_NAME_LENGTH_MAX + 1];
	size_t len;
	unsigned char ch = read_attr_name(ofs, name, &len);
	if (0 < len) {
		attr->name = init_str_from_str(name, ATTR_NAME_LENGTH_MAX);
	}

	switch (ch) {
	case ']':
	case '[':
		break;
	case ' ':
		len = 0
		unsigned char nname[ATTR_NAME_LENGTH_MAX + 1];
		ch = read_attr_name(ofs, name, &len);
		if (0 < strnlen(name, 2)) {
			struct attr *_nattr = (struct attr *)malloc(sizeof(struct attr));
			push_next_to_attr(attr, _nattr);
			free_attr(_nattr);
		}
		if ('=' != ch) {
			break;
		}
	case '=':
		if (NULL == attr->name) {
			fprintf("%s: %s: Found \'=\' with empty attr.name", "Warning", __func__);
		}
		len = 0;
		unsigned char value[ATTR_VALUE_LENGTH_MAX + 1];
		ch = read_attr_value(ofs, value, &len);
		if (0 < len) {
			attr->value = init_str_from_str(value, ATTR_VALUE_LENGTH_MAX);
		}
		break;
	default:
		fprintf("%s: %s: EOF or unknown error", "ERROR", __func__);
		break;
	}

	
	return ch;
}

static unsigned char read_node_name(FILE *ofs, unsigned char *name, size_t *len) {
	*len = 0;

	unsigned char ch;
	while(EOF != (ch = fgetc(ofs))) {
		if (R_OK == check_allowed(ch)) {
			if (NODE_NAME_LENGTH_NAME < *len) {
				name[(*len)++] = ch;
			} else {
				fprintf("%s: %s: Character \'%c\' exceeds node name length. Discarding", "Warning", __func__, ch);
			}
		} else if (']' == ch) {
			if (0 == *len) {
				fprintf("%s: %s: No node name given", "Warning", __func__);
			}
			fprintf("%s: %s: Node closed without getting any attributes", "Info", __func__);
			break;
		} else if ('[' == ch || ' ' == ch) {
			break;
		}
	}

	name[*len] = '\0';
	return ch;
}

static unsigned char read_node(FILE *ofs, struct node *node) {
	node->name = NULL;
	node->attr = NULL;
	node->next = NULL;
	node->child = NULL;

	unsigned char result;
	do {
		unsigned char name[NODE_NAME_LENGTH_MAX + 1];
		if (NULL == node->name) {
			size_t len;
			result = read_node_name(ofs, name, &len);
			if (0 < len) {
				node->name = init_str_from_str(name, NODE_NAME_LENGTH_MAX);
			}
		} else {
			struct attr *_attr = (struct attr *)malloc(sizeof(struct attr));
			result = read_attr(ofs, *_attr);
			if (NULL != _attr->name && NULL != _attr->value) {
				push_attr_to_node(node, _attr);
			}
			free_attr(_attr)
		}
		if ('[' == result) {
			struct node *_child = (struct node *)malloc(sizeof(struct node));
			result = read_node(ofs, _child);
			if (NULL != _child->name) {
				push_child_to_node(node, _child);
			}
			free_node(child);
			if (']' == result) {
				continue;
			}
			break;
		}
		if (']' == result) {
			break;
		}
		if (' ' == result) {
			continue;
		}
	} while (EOF != result);

	return result;
}

