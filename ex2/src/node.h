#ifndef NODE_H__
#define NODE_H__

#define ATTR_NAME_LENGTH_MAX 1024
#define ATTR_VALUE_LENGTH_MAX 1024

#define NODE_NAME_LENGTH_MAX 1024

struct attr {
	char *name;
	char *value;
	struct attr *next;
};

struct node {
	char *name;
	struct attr *attr;
	struct node *next;
	struct node *child;
};


// TODO поработать над функциями и именами


/// Функция инициализации (head) листа.
/// По завершению работы с листом необходимо высвободить
/// память с помощью delete_list(struct node *head).
/// return Указатель на head при успехе, иначе - NULL
struct node *init_list(void);

/// Функция деинициализации листа (по head).
/// Используется по завершению работы с листом.
/// return 0 - успех, иначе -1
int free_list(struct node *head);

/// Функция рекурсивного удаления узла.
void free_node(struct node *this);
/// Функция рекурсивного удаления атрибутов.
void free_attr(struct attr *this);

/// Функция добавления атрибута next к атрибуту this.
/// Если this.next != NULL, добавляет в конец this.next.
/// Данная функция создает копию значений next, а
/// не записывает его адрес в this.
int push_next_to_attr(struct attr* this, const struct attr *next);

/// Функция добавления атрибута attr к узлу this.
/// Если this.attr != NULL, добавляет в конец this.attr
/// Данная функция создает копию значений attr, а
/// не записывает его адрес в this.
int push_attr_to_node(struct node *this, const struct attr *attr);
/// Фукнция добавления узла next к узлу this.
/// Если this.next != NULL, добавляет в конец this.next.
/// Данная функция создает копию значений next, а
/// не записывает его адрес в this.
int push_next_to_node(struct node *this, const struct node *next);
/// Функция добавления узла.
/// Если this.child != NULL, добавляет в конец this.child.next.
/// Данная функция создает копию значений child, а
/// не записывает его адрес в this.
int push_child_to_node(struct node *this, const struct node *child);

/// TODO переименовать данную группу фукнций в attach?
/// Функция добавления next к атрибуту this.
/// Не создает копию, записывает переданный узел в конец.
int move_next_to_attr_end(struct attr *this, struct attr *next);

int move_attr_to_node_end(struct node *this, struct attr *next);
/// Функция добавления узла next к узлу this.
/// Не создает копию, записывает переданный узел в конец.
int move_next_to_node_end(struct node *this, struct node *next);

int move_child_to_node_end(struct node *this, struct node *child);

/// Функция получения следующего атрибута у attr.
/// return Указатель на next при наличии, иначе - NULL
struct attr *get_next_from_attr(struct attr *this);
/// Функция получения первого атрибута у node.
/// return Указатель на attr при наличии, иначе - NULL
struct attr *get_attr_from_node(struct node *this);
/// Функция получения следующего узла у node.
/// return Указатель на next при наличии, иначе - NULL
struct node *get_next_from_node(struct node *this);
/// Функция получения первого child у node.
/// return Указатель на child при наличии, иначе - NULL
struct node *get_child_from_node(struct node *this);

#endif  // NODE_H__

