#include "parser.h"

#include <stdlib.h>
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


static inline int check_allowed(const char ch) {
	if(('0' <= ch && '9' >= ch) ||
	   ('a' <= ch && 'z' >= ch) ||
	   ('A' <= ch && 'Z' >= ch) ||
	   ('_' == ch)) {
		return R_OK;
	}
	fprintf(stderr, "%s: %s: Found unallowed character \'%c\'\n", "Warning", __func__, ch);
	return R_BAD;
}

static char read_attr(FILE *ofs, struct attr *attr) __attribute__((nonnull));
static char read_node(FILE *ofs, struct node *node) __attribute__((nonnull));

char *init_str_from_str(const char *src, const size_t lmax) {
	char *dst = NULL;
	size_t len;

	if (NULL != src) {
		len = strnlen(src, lmax);
	} else {
		fprintf(stderr, "%s: %s: src is NULL\n", "Warning", __func__);
		len = 0;	
	}

	dst = (char *)malloc(len + 1);
	if (NULL == dst) {
		fprintf(stderr, "%s: %s: Failed to init a string\n", "ERROR", __func__);
		return NULL;
	}
	
	if (NULL != src) {
		strncpy(dst, src, len);
	}
	dst[len] = '\0';
	return dst;
}

int parse_file(const char *filename, struct node *head) {
	if (NULL == filename || '\0' == filename[0]) {
		fprintf(stderr, "%s: %s: No filename given\n", "ERROR", __func__);
		return -1;
	}
	if (NULL == head) {
		fprintf(stderr, "%s: %s: head is NULL\n", "ERROR", __func__);
		return -1;
	}
	FILE *ofs = fopen(filename, "r");
	if (NULL == ofs) {
		fprintf(stderr, "%s: %s: Can't open file \"%s\"\n", "ERROR", __func__, filename);
		return -1;
	}
	
	head->name = NULL;
	head->attr = NULL;
	head->next = NULL;
	head->child = NULL;

	// !valgrind
	struct node *node = NULL;
	char ch;
	while(EOF != (ch = fgetc(ofs))) {
		// valgrind ругается без явной конструкции, на всякий
		if (NULL == node) {
			node = (struct node *)malloc(sizeof(struct node));
			if (NULL == node) {
				fprintf(stderr, "%s: %s: Can't malloc node. Aborting", "CRITICAL", __func__);
				fclose(ofs);
				return -1;
			}
			// !valgrind
			node->name = NULL;
			node->attr = NULL;
			node->next = NULL;
			node->child = NULL;
		}
		if ('[' == ch) {
			if (EOF == read_node(ofs, node)) {
				break;
			}
			if (node->name || node->attr || node->next || node->child) {
				move_next_to_node_end(head, node);
				node = NULL;
			}
		}
	}
	if (node) 
		free_node(node);

	fclose(ofs);
	return 0;
}

static inline char read_attr_name(FILE *ofs, char *name, size_t *len) {
	*len = 0;

	char ch;
	while(EOF != (ch = fgetc(ofs))) {
		if (ch == '=') {
			break;
		} else if (ch == ']' || ch == '[') {
			if (0 != *len) {
				fprintf(stderr, "%s: %s: Got an attribute without a value\n", "Info", __func__);
			}
			break;
		} else if (ch == ' ' || ch == '\t') {
			if (0 < *len) {
				break;	// End of attr.name
			}
		} else if (R_OK == check_allowed(ch)) {
			if (ATTR_NAME_LENGTH_MAX > *len) {
				name[(*len)++] = ch;
			} else {
				fprintf(stderr, "%s: %s: Character \'%c\' exceeds attribute name length. Discaring\n", "Warning", __func__, ch);
			}
		} else {
			fprintf(stderr, "%s: %s: Got a bad character \'%c\'. Discarding\n", "Warning", __func__, ch);
		}	
	}

	name[*len] = '\0';
	return ch;
}

// Only with \' or \"
static inline void check_onescape(const char ch, const char quote) {
	if (0 == quote) {
		if (']' == ch || '[' == ch) {
			return;
		}
	} else if ('\'' == ch || '\"' == ch) {
		if (quote == ch) {
			return;
		}
	} else if ('\\' == ch) {
		return;
	}

	fprintf(stderr, "%s: %s: Excessive backslash\n", "Info", __func__);

}
static inline int check_normal(const char ch, char *quote, const size_t len) {
	if ('\"' == ch || '\'' == ch) {
		if (ch == *quote) {
			return R_END;
		} else if (0 == len) {
			*quote = ch;
			return R_SKIP;
		}
	
	} else if ('\\' == ch) {
		return R_ESC;
	} else if ((' ' == ch || '[' == ch || ']' == ch) && 0 == *quote) {
		return R_END;
	}

	return R_OK;
}

static char read_attr_value(FILE *ofs, char *value, size_t *len) {
	*len = 0;

	bool has_escape = false;
	char quote = 0;

	char ch;
	while(EOF != (ch = fgetc(ofs))) {
		// Determine action
		int action;
		if (has_escape) {
			check_onescape(ch, quote);
			has_escape = false;
			action = R_OK;
		} else {
			action = check_normal(ch, &quote, *len);
		}
		// Act
		if (R_OK == action) {
			if (ATTR_VALUE_LENGTH_MAX > *len) {
				value[(*len)++] = ch;
			} else {
				fprintf(stderr, "%s: %s: Character \'%c\' exceeded attribute value length. Discarding\n", "Warning", __func__, ch);
			}
		} else if (R_ESC == action) {
			has_escape = true;
		} else if (R_END == action) {
			break;
		}
	}

	value[*len] = '\0';
	return ch;
}
static char read_attr(FILE *ofs, struct attr *attr) {
	attr->name = NULL;
	attr->value = NULL;
	attr->next = NULL;

	char name[ATTR_NAME_LENGTH_MAX + 1];
	char value[ATTR_VALUE_LENGTH_MAX + 1];

	size_t len = 0;
	char ch;

	ch = read_attr_name(ofs, name, &len);
	if (0 < len) {
		attr->name = init_str_from_str(name, ATTR_NAME_LENGTH_MAX);
	}

	switch (ch) {
	case ']':
	case '[':
		break;
	case ' ':
		ch = read_attr_name(ofs, name, &len);
		if (0 < len) {
			struct attr *_nattr = (struct attr *)malloc(sizeof(struct attr));
			if (NULL == _nattr) {
				return -1;
			}
			_nattr->name = init_str_from_str(name, ATTR_NAME_LENGTH_MAX);
			move_next_to_attr_end(attr, _nattr);
			attr = attr->next; // Предыдущий атрибут имел пустое value, пришел новый
		}
		if ('=' != ch) {
			break;
		}
		// fallthrough
	case '=':
		if (NULL == attr->name) {
			fprintf(stderr, "%s: %s: Found \'=\' with empty attr.name\n", "Warning", __func__);
		}
		len = 0;
		ch = read_attr_value(ofs, value, &len);
		if (0 < len) {
			attr->value = init_str_from_str(value, ATTR_VALUE_LENGTH_MAX);
		}
		break;
	default:
		fprintf(stderr, "%s: %s: EOF or unknown error\n", "ERROR", __func__);
		break;
	}
	
	return ch;
}

static char read_node_name(FILE *ofs, char *name, size_t *len) {
	*len = 0;

	char ch;
	while(EOF != (ch = fgetc(ofs))) {
		if (']' == ch) {
			if (0 == *len) {
				fprintf(stderr, "%s: %s: No node name given\n", "Warning", __func__);
			}
			break;
		} else if ('[' == ch || ' ' == ch) {
			break;
		} else if (R_OK == check_allowed(ch)) {
			if (NODE_NAME_LENGTH_MAX > *len) {
				name[(*len)++] = ch;
			} else {
				fprintf(stderr, "%s: %s: Character \'%c\' exceeds node name length. Discarding\n", "Warning", __func__, ch);
			}
		}
	}

	name[*len] = '\0';
	return ch;
}

static char read_node(FILE *ofs, struct node *node) {
	node->name = NULL;
	node->attr = NULL;
	node->next = NULL;
	node->child = NULL;

	char result;
	do {
		char name[NODE_NAME_LENGTH_MAX + 1];
		if (NULL == node->name) {
			size_t len;
			result = read_node_name(ofs, name, &len);
			if (0 < len) {
				node->name = init_str_from_str(name, NODE_NAME_LENGTH_MAX);
			}
		} else {
			struct attr *_attr = (struct attr *)malloc(sizeof(struct attr));
			if (NULL == _attr) {
				return EOF;
			}

			result = read_attr(ofs, _attr);

			if (NULL != _attr->name || NULL != _attr->value) {
				move_attr_to_node_end(node, _attr);
			} else {
				free_attr(_attr);
			}
		}

		if ('[' == result) {
			struct node *_child = (struct node *)malloc(sizeof(struct node));
			if (NULL == _child) {
				return EOF;	
			}

			result = read_node(ofs, _child);
			if (NULL != _child->name) {
				move_child_to_node_end(node, _child);
			} else {
				free_node(_child);
			}
			if (']' != result) {
				break;
			}
		} else if (']' == result) {
			break;
		}
	} while (EOF != result);

	if (EOF == result) {
		fprintf(stderr, "%s: %s: Unexpected EOF\n", "Warning", __func__);
	}
	if (NULL == node->attr) {
		fprintf(stderr, "%s: %s: Node \"%s\" closed without getting any attributes\n", "Info", __func__, node->name ? node->name : "");
	}
	return result;
}

