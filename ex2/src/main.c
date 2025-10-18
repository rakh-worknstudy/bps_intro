#include <stdio.h>

#include "node.h"

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

int read_next(struct rres *result);
int parse(struct node *list, char *filename);
int init();

int main(void) {
	return init();
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
			perror("ERROR: parser(): Bad string given");
			return -1;
		}
	}

	// On FAIL
	return -1;
}

int init(void) {
	struct node *list = init_list();
	if (NULL == list) {
		perror("ERROR: init(): Failed to init list");
		return -1;
	}

	int rcode = parse(list);

	free_list(list);
	return rcode;
}

