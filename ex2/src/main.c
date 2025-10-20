#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "node.h"
#include "parser.h"

int print_list(const struct node *head) __attribute__((nonnull));
int print_layer(const struct node *first, const int layer);

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "No filename");
		return -1;
	}
	const char *filename = argv[1];
	struct node *head = init_list();
	if (NULL == head) {
		fprintf(stderr, "Failed to init a list\n");
		return -1;
	}

	if (0 == parse_file(filename, head)) {
		print_list(head);
	} else {
		fprintf(stderr, "Unable to proceed\n");
	}
	free_list(head);

	return 0;
}

int print_list(const struct node *head) {
	printf("================================\n");
	struct node *first = (struct node *)head->next;
	if (NULL == first) {
		printf("List is empty\n");
		return 0;
	}

	print_layer(first, 0);
	printf("================================\n");
	return 0;
}

static inline int print_attributes(const struct attr *first, const int spaces) {
	static const int attributes_indent_spaces = 2;
	const int spaces_attr = spaces + attributes_indent_spaces;

	printf("%*sAttributes:", spaces, "");
	if (NULL == first) {
		printf(" -\n");
		return 0;
	}

	printf("\n");
	struct attr *at = (struct attr *)first;
	while(NULL != at) {
		printf("%*sName:  %s\n", spaces_attr, "", at->name ? at->name : "");
		printf("%*sValue: %s\n", spaces_attr, "", at->value ? at->value : "");

		if (NULL != at->next) {
			printf("\n");
		}
		at = at->next;
	}

	return 0;
}

static inline int print_children(const struct node *first, const int spaces, const int layer) {
	if (NULL == first) {
		printf("%*sChildren:   -\n", spaces, "");
		return 0;
	}

	printf("%*sChildren:\n", spaces, "");
	return print_layer(first, layer + 1);
}

int print_layer(const struct node *first, const int layer) {
	static const int spaces_per_layer = 4;
	const int spaces = layer * spaces_per_layer;

	struct node *at = (struct node *)first;
	while(NULL != at) {
		printf("%*sNode name:  %s\n", spaces, "", at->name ? at->name : "-");
		print_attributes(at->attr, spaces);
		print_children(at->child, spaces, layer);
		if (NULL != at->next) {
			printf("\n");
		}
		at = at->next;
	}	
	return 0;
}

