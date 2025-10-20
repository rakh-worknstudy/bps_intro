#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "parser.h"

/// Вспомогательная функция иницализации атрибута dst по
/// переданному указателю src на базовый атрибут.
/// src.next == NULL не является обязательным, но предполагается.
/// Скидывает предупреждение в stderr.
/// return Указатель на атрибут при успехе, иначе - NULL
static struct attr *init_attr_from_attr(const struct attr *src) {
	if (NULL == src) {
		perror("ERROR: init_attr_from_attr(): src is NULL");
		return NULL;
	}

	struct attr *dst = (struct attr *)malloc(sizeof(struct attr));
	if (NULL == dst) {
		perror("ERROR: init_attr_from_attr(): Failed to init an attribute");
		return NULL;
	}

	do {
		if (NULL == (dst->name = init_str_from_str(src->name, ATTR_NAME_LENGTH_MAX))) {
			perror("ERROR: init_attr_from_attr(): Failed to init a name");
			break;
		}
		if (NULL == (dst->value = init_str_from_str(src->value, ATTR_VALUE_LENGTH_MAX))) {
			perror("ERROR: init_str_from_str(): Failed to init a value");
			break;
		}
		if (NULL != src->next) {
			perror("Warning: init_str_from_str(): next is NOT NULL");
		}
		dst->next = src->next;

		return dst;
	} while(false);

	// On break (fail)
	free_attr(dst);
	return NULL;
}

/// Вспомогательная функция иницализации узла dst по переданному
/// указателю src на базовый узел.
/// src.next == NULL и src.child == NULL  не являются обязательным,
/// но предполагаются. Скидывает предупреждение в stderr.
/// return Указатель на узел при успехе, иначе - NULL
static struct node *init_node_from_node(const struct node *src) {
	if (NULL == src) {
		perror("ERROR: init_node_from_node(): src is NULL");
		return NULL;
	}

	struct node *dst = (struct node *)malloc(sizeof(struct node));
	if (NULL == dst) {
		perror("ERROR: init_node_from_node(): Failed to init a node");
		return NULL;
	}

	do {
		// Node's name
		if (NULL == (dst->name = init_str_from_str(src->name, NODE_NAME_LENGTH_MAX))) {
			perror("ERROR: init_node_from_node(): Failed to init a name");
			break;
		}
		// Node's attributes
		if (NULL != src->attr) {
			if (NULL == (dst->attr = init_attr_from_attr(src->attr))) {
				perror("ERROR: init_node_from_node(): Failed to init attribues");
				break;
			}
		} else {
			dst->attr = NULL;
		}
		// Node's next
		if (NULL != src->next) {
			perror("Warning: init_node_from_node(): src.next is NOT NULL");
			if (NULL == (dst->next = init_node_from_node(src->next))) {
				perror("ERROR: init_node_from_node(): Failed to init next");
			}
		} else {
			dst->next = NULL;
		}
		// Node's children
		if (NULL != src->child) {
			perror("Warning: init_node_from_node(): src.child is NOT NULL");
			if (NULL == (dst->child = init_node_from_node(src->child))) {
				perror("ERROR: init_node_from_node(): Failed to init child");
			}
		} else {
			dst->child = NULL;
		}
		return dst;
	} while(false);

	// On break (fail)
	free_node(dst);
	return NULL;
}

/// Функция инициализации (head) листа.
struct node *init_list(void) {
	struct node *head = (struct node *)malloc(sizeof(struct node));
	if (NULL == head) {
		perror("ERROR: init_list(): Failed to init a list");
		return NULL;
	}

	head->name = NULL;
	head->attr = NULL;
	head->next = NULL;
	head->child = NULL;
	return head;
}


int free_list(struct node *head) {
	if (NULL == head) {
		perror("ERROR: delete_list(): head is NULL");
		return -1;
	}
	free_node(head);
	return 0;
}

void free_attr(struct attr *this) {
	if (NULL == this) {
		return;
	}
	if (NULL != this->next) {
		free_attr(this->next);
	}
	free(this);
}

void free_node(struct node *this) {
	if (NULL == this) {
		return;
	}
	if (NULL != this->next) {
		free_node(this->next);
	}
	if (NULL != this->child) {
		free_node(this->child);
	}
	free_attr(this->attr);
	free(this);
}


int push_next_to_attr(struct attr *this, const struct attr *next) {
	if (NULL == this) {
		perror("ERROR: push_next_to_attr(): this is NULL");
		return -1;
	}
	if (NULL == next) {
		perror("ERROR: push_next_to_attr(): next is NULL");
		return -1;
	}

	while(NULL != this->next) {
		this = this->next;
	}
	this->next = init_attr_from_attr(next);
	if (NULL == this->next) {
		perror("ERROR: push_next_to_attr(): Failed to add an attribute");
		return -1;
	}
	
	// On SUCCESS
	return 0;
}

int push_attr_to_node(struct node *this, const struct attr *attr) {
	if (NULL == this) {
		perror("ERROR: push_attr_to_node(): this is NULL");
		return -1;
	}
	if (NULL == attr) {
		perror("ERROR: push_attr_to_node(): attr is NULL");
		return -1;
	}

	// No attributes assigned
	if (NULL == this->attr) {
		this->attr = init_attr_from_attr(attr);
		if (NULL == this->attr) {
			perror("ERROR: push_attr_to_node(): Failed to init attributes");
			return -1;
		}
	// Has attributes assigned
	} else {
		struct attr *last = this->attr;
		while(NULL != last->next) {
			last = last->next;
		}
		last->next = init_attr_from_attr(attr);
		if (NULL == last->next) {
			perror("ERROR: push_attr_to_node(): Failed to add an attribute");
			return -1;
		}
	}

	// On SUCCESS
	return 0;
}


int push_next_to_node(struct node *this, const struct node *next) {
	if (NULL == this) {
		perror("ERROR: push_next_to_node(): this is NULL");
		return -1;
	}
	if (NULL == next) {
		perror("ERROR: push_next_to_node(): next is NULL");
		return -1;
	}

	// No next assigned
	if (NULL == this->next) {
		this->next = init_node_from_node(next);
		if (NULL == this->next) {
			perror("ERROR: push_next_to_node(): Failed to init next");
			return -1;
		}
	// Has next assigned
	} else {
		struct node *last = this->next;
		while(NULL != last->next) {
			last = last->next;
		}
		last->next = init_node_from_node(next);
		if (NULL == last->next) {
			perror("ERROR: push_next_to_node(): Failed to add next");
			return -1;
		}
	}

	// On SUCCESS
	return 0;
}

int push_child_to_node(struct node *this, const struct node *child) {
	if (NULL == this) {
		perror("ERROR: push_child_to_node(): this is NULL");
		return -1;
	}
	if (NULL == child) {
		perror("ERROR: push_child_to_node(): next is NULL");
		return -1;
	}

	// No child assigned
	if (NULL == this->child) {
		this->child = init_node_from_node(child);
		if (NULL == this->next) {
			perror("ERROR: push_child_to_node(): Failed to init child");
			return -1;
		}
	// Has child assigned
	} else {
		struct node *last = this->child;
		while(NULL != last->next) {
			last = last->next;
		}
		last->next = init_node_from_node(child);
		if (NULL == last->next) {
			perror("ERROR: push_child_to_node(): Failed to add a child");
			return -1;
		}
		
	}

	// On SUCCESS
	return 0;
}

struct attr *get_next_from_attr(struct attr *this) {
	if (NULL == this) {
		perror("ERROR: get_next_from_attr(): this is NULL");
		return NULL;
	}
	return this->next;
}

struct attr *get_attr_from_node(struct node *this) {
	if (NULL == this) {
		perror("ERROR: get_attr_from_node(): this is NULL");
		return NULL;
	}
	return this->attr;
}

struct node *get_next_from_node(struct node *this) {
	if (NULL == this) {
		perror("ERROR: get_next_from_node(): this is NULL");
		return NULL;
	}
	return this->next;
}

struct node *get_child_from_node(struct node *this) {
	if (NULL == this) {
		perror("ERROR: get_child_from_node(): this is NULL");
		return NULL;
	}
	return this->child;
}

