# bloom-lookup-table

TODO: 
1. Добавить fault tolerance для пар (ключ1, значение1) и (ключ1, значение2).
2. Собрать табличку с зависимостью n, m, k, сравнить результаты со статьей.

Результаты тестов на запаковку распаковку множеств в фильтры, где ratio - отношение размера фильтра к количеству пар, а количество хеш-функций всегда 3:

```
List entries (1000 tests with 1000 pairs):
Percentage of successes for ratio 1 is 0.0
Percentage of successes for ratio 1.1 is 0.0
Percentage of successes for ratio 1.2 is 5.4
Percentage of successes for ratio 1.3 is 99.4
Percentage of successes for ratio 1.4 is 99.1
Percentage of successes for ratio 1.5 is 98.9
Percentage of successes for ratio 1.6 is 99.7
Percentage of successes for ratio 1.7 is 99.6
Percentage of successes for ratio 1.8 is 100.0
Percentage of successes for ratio 1.9 is 100.0
Percentage of successes for ratio 2 is 99.8

List entries (1000 tests with 100 pairs):
Percentage of successes for ratio 1 is 0.0
Percentage of successes for ratio 1.1 is 0.0
Percentage of successes for ratio 1.2 is 9.0
Percentage of successes for ratio 1.3 is 52.6
Percentage of successes for ratio 1.4 is 85.6
Percentage of successes for ratio 1.5 is 91.8
Percentage of successes for ratio 1.6 is 95.1
Percentage of successes for ratio 1.7 is 95.9
Percentage of successes for ratio 1.8 is 96.3
Percentage of successes for ratio 1.9 is 97.2
Percentage of successes for ratio 2 is 97.5
```
Предварительные результаты экспериментов с разностью фильтров:
```
STRESS TESTING OF LIST-ENTRIES OF SET DIFFERENCE
------------------------------------------------
Number of TESTS:
100
Number of PAIRS per filter:
1000
Number of CELLS per filter:
2000
Number of INTERSECTING PAIRS for two sets:
500
List entries (100 tests with 1000 pairs):
Percentege of successes is 99
```
