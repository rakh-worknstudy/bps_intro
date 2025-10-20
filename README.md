# Задания БПС
На 20.10: Смотреть в Ex2, прочие не трогались
##
### Ex1
-
##
### Ex2

+ Чтение реализовано посимвольно
+ [ - начало узла
+ ] - конец узла
+ Название узла пишется в начале его определения
+ attr.value может быть пустым

  
Часть конструкций кода добавлялась, чтобы "успокоить" valgrind в отчете  
Часть введенных функций оказались неиспользованы (push_) в связи со сроками и избыточностью перевыделения памяти  
В папке ex2/build лежит тестовый файл **text.txt** со строкой-примером  
Более подробно с процессом работы можно ознакамиться через git log  

**Результат + отчет Valgrind (по файлу в build/text.txt):**  
```
==3089== Memcheck, a memory error detector  
==3089== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.  
==3089== Using Valgrind-3.16.1 and LibVEX; rerun with -h for copyright info  
==3089== Command: ../build/ex2 ../build/text.txt  
==3089== 
Info: read_attr_name: Got an attribute without a value
================================
Node name:  node1
Attributes:
  Name:  attr1
  Value: "test attr name"

  Name:  attr2
  Value: 

  Name:  attr3
  Value: test
Children:
    Node name:  child1
    Attributes:
      Name:  chattr1
      Value: 
    Children:   -
================================
==3089== 
==3089== HEAP SUMMARY:
==3089==     in use at exit: 0 bytes in 0 blocks
==3089==   total heap usage: 19 allocs, 19 frees, 5,877 bytes allocated
==3089== 
==3089== All heap blocks were freed -- no leaks are possible
==3089== 
==3089== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
##
### Ex3
