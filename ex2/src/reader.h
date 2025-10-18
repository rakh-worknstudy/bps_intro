#ifndef READER_H__
#define READER_H__

// Функции для посимвольного чтения строки из файла в буффер размера size
// В двух вариантах: всех ASCII и [0-9a-zA-z_]. Можно заменить разделитель

#include <stdio.h>

// size is max length + 1 for '\0'
int read_str(FILE *ofs, unsigned char *buffer[], const size_t size, const unsigned char *delim);
int read_str_any(FILE *ofs, unsigned char *buffer[], const size_t size, const unsigned char *delim);

#endif  // READER_H__

