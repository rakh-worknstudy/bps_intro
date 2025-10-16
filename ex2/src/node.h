#ifndef NODE_H__
#define NODE_H__

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

/// @brief Функция инициализации (head) листа.
/// @note По завершению работы с листом необходимо высвободить
/// память с помощью delete_list(struct node *head)
/// @return Указатель на head при успехе, иначе - NULL
struct node *init_list(void);

/// @brief Функция деинициализации листа (по head).
/// @note Используется по завершению работы с листом
/// @return 0 - успех, иначе -1
int free_list(struct node *head);

/// @brief Функция рекурсивного удаления узла
void free_node(struct node *this);
/// @brief Функция рекурсивного удаления атрибутов
void free_attr(struct attr *this);

/// @brief Функция добавления атрибута next к атрибуту this
/// @note Если this.next != NULL, добавляет в конец this.next
/// @warning Данная функция создает копию значений next, а
/// не записывает его адрес в this.
int add_next_to_attr(struct attr* this, struct attr *next);

/// @brief Функция добавления атрибута attr к узлу this
/// @note Если this.attr != NULL, добавляет в конец this.attr
/// @warning Данная функция создает копию значений attr, а
/// не записывает его адрес в this.
int add_attr_to_node(struct node *this, struct attr *attr);
/// @brief Фукнция добавления узла next к узлу this
/// @note Если this.next != NULL, добавляет в конец this.next
/// @warning Данная функция создает копию значений next, а
/// не записывает его адрес в this.
int add_next_to_node(struct node *this, struct node *next);
/// @brief Функция добавления узла
/// @note Если this.child != NULL, добавляет в конец this.child.next
/// @warning Данная функция создает копию значений child, а
/// не записывает его адрес в this.
int add_child_to_node(struct node *this, struct node *child):

#endif  // NODE_H__
