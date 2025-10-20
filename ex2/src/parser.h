#ifndef PARSER_H__
#define PARSER_H__

#include <stdio.h>
#include "node.h"

/// Вспомогательная функция инициализации строки char *dst
/// по переданной строке char *src и максимальному размеру lmax.
/// src == NULL не является обязателньым, но предполагается.
/// Скидывает предупреждение в stderr.
/// Инициализирует строгий размер, чанки показались сомнительными
/// с указанными в задании значениями.
/// return Указатель на строку при успехе, иначе - NULL
char *init_str_from_str(const char *src, const size_t lmax);


struct node *parse_file(const char *filename);

#endif  // PARSER_H__

