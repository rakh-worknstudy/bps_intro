#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "node.h"
#include "reader.h"

struct rres {
	enum rtype {
		END = 0,
		NODE,
		ATTR,
		BAD = -1
	} type;
	union rbuff {
		struct node _node;
		struct attr _attr;
	} buff;
};

int read_node(FILE *ofs, struct node **result);
struct node *read_layer();
int parse(struct node *list, char *filename);
int init();

int main(void) {
	return init();
}

int read_next(struct rres *result) {
	return -1;
}

int parse(struct node *list, char *filename) {
	struct rres result;
	while(0 == read_next(&result)) {
		switch(result.type) {
		case NODE:
			break;
		case ATTR:
			break;
		case END:
			break;
		case BAD:
			fprintf(stderr, "%s: %s(): Bad string given", "ERROR", __func__);
			return -1;
		}
	}

	result.buff._node.name = "TEST";
	result.buff._node.attr = NULL;
	result.buff._node.next = NULL;
	result.buff._node.child = NULL;
	push_next_to_node(list, &result.buff._node);

	printf("PUSHED NEXT");

	// On FAIL
	return -1;
}

int init(void) {
	struct node *list = init_list();
	if (NULL == list) {
		fprintf(stderr, "%s: %s(): Failed to init list", "ERROR", __func__);
		return -1;
	}

	int rcode = parse(list, "");

	free_list(list);
	printf("%d", rcode);
	return rcode;
}

// Read attribute (on 'a')
int read_attr(FILE *ofs, struct attr **result) {
	bool has_name = false;
	bool 
}

// Read node (on '[')
int read_node(FILE *ofs, struct node **result) {
	if (NULL == ofs) {
		fprintf(stderr, "%s: %s(): filestream is NULL", "ERROR", __func__);
		return -1;
	}
	if (NULL == result) {
		fprintf(stderr, "%s: %s(): result buffer is NULL", "ERROR", __func__);
		return -1;
	}

	*result = init_list();
	bool has_name = false;	// Warning on empty name
	bool has_attr = false;	// Warning on no attributes
	while(']' != (unsigned char ch = (unsigned char)fgetc(ofc))) {
		switch (ch) {
		// Haven't closed with ], assuming new child
		case '[':
			struct node *child = NULL;
			if (0 != read_node(ofc, child)) {
				fprintf(stderr, "%s: %s(): Child parse error, discarding", "Warning", __func__);
			} else {
				(*result)->child = child;
			}
			break;
		case 'n':
			if (has_name) {
				fprintf(stderr, "%s: %s(): Node already had a name, overriding", "Warning", __func__);
			}

		}
	}

	return 0;
}
