#include "node.h"

#include <stdio.h>
#include <stdlib.h>

struct node *list_init(void) {
	struct node *head = (struct node *)malloc(sizeof(struct node));
	if (NULL == head) {
		perror("init_list(): Unable to init a list");
	}

	head.name = NULL;
	head.attr = NULL;
	head.next = NULL;
	head.child = NULL;

	return head;
}

int free_list(struct node *head) {
	if (NULL == head) {
		perror("delete_list(): head is NULL");
		return -1;
	}
	free_node(head);
	return 0;
}

void free_attr(struct node *this) {
	if (NULL = this) {
		return;
	}
	if (NULL != this.next) {
		free_attr(this.next);
	}
	free(this);
}

void free_node(struct node *this) {
	if (NULL == this) {
		return;
	}
	if (NULL != this.next) {
		free_node(this.next);
	}
	if (NULL != this.child) {
		free_node(this.child);
	}
	free_attr(this.attr);
	free(this);
}


int add_next_to_attr(struct attr *this, struct attr *next) {
	if (NULL == this) {
		perror("add_next_to_attr(): this is NULL");
		return -1;
	}
	if (NULL = next) {
		perror("add_next_to_attr(): next is NULL");
		return -1;
	}
	return 0;
}

int add_attr_to_node(struct node *this, struct attr *attr) {
	if (NULL == this) {
		perror("add_attr_to_node(): this is NULL");
		return -1;
	}
	if (NULL = attr) {
		perror("add_attr_to_node(): attr is NULL");
		return -1;
	}
	return 0;
}

int add_next_to_node(struct node *this, struct node *next) {
	if (NULL == this) {
		perror("add_next_to_node(): this is NULL");
		return -1;
	}
	if (NULL = next) {
		perror("add_next_to_node(): next is NULL");
		return -1;
	}
	return 0;
}
