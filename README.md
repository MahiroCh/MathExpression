# Сборка и использование

1) Команда сборки проекта: `make`  
2) Команда запуска тестов: `make test`  

После сборки из командной строки доступны следующие команды:  

1) Команда вычисления выражения: `./differentiator --eval "*expression*" *var1*=*value* *var2*=*value* ...`  
2) Команда подсчета частичной производной: `./differentiator --diff "*expression*" --by *var*`  

---

# Особенности имплементации

1) Комплексная единица — это исключительно `"I"`. Строчной символ `"i"` считается за переменную. Если в выражении присутствует `"I"` хотя бы однажды в любом виде, то выражение принимается за комплексное, даже если на деле `"I"` содержится только в названии переменных (это поведение хотелось бы пофиксить, но пока что не придумал адекватное лаконичное решение; да и в ТЗ ничего не прописано, поэтому взятки гладки).  

2) При парсинге выражения комплексные части числа воспринимаются только в виде `"tI"`, `"Ik"` или `"I"` (где `"t"` — рациональное число, `"k"` — положительное рациональное число). Объекты по типу `"t I"`, `"I t"` или `"ti"` недопустимы.  

3) Подстановка значений в выражение изменяет исходное выражение, а не создает новое.  

4) Дифференцирование создает новое выражение, а не изменяет исходное.  

5) При парсинге выражения пропуск операций умножения в сценариях по типу `"tsin"` и `"tx"` (где `"t"` — рациональное число) допустим. В сценариях `"xy"`, `"xsin"` (где `"x"` и `"y"` — переменные) пропуск недопустим (такое будет интерпретироваться как переменная, а в случае с `"sin"` вообще будет undefined behaviour). Также, кроме `"tx"`, допускается любое количество пробелов между `"t"` и `"x"`, что будет восприниматься как `"tx"`  

6) Тригонометрические функции всегда принимают свои аргументы за радианы. Поддержки градусов нет.  

7) Пробелы игнорируются, но следует быть аккуратным с переменными.  

8) За переменные считается все, что начинается с буквы (но не с `"I"`) и содержит исключительно буквы и цифры.  

9) Все выражения, кроме комплексной единицы `"I"`, приводятся к lower-case, поэтому `"X"` и `"x"` — одно и то же.

10) При подсчете производной по определенной переменной все остальные переменные работают как обыкновенные числа, т.е. их производная равна нулю. Например, прозводная `3x*y` по `y` равна `3x`.

11) Даже элементарное упрощение выражений напрочь отсутствует, поэтому они выглядят как помойка (например производная по `y` выражения `1x*3y` выглядит так: `((((((0 * x) + (1 * 0)) * 3) + ((1 * x) * 0)) * y) + (((1 * x) * 3) * 1))`). В ТЗ ничего не сказано. В чате заметили, что некоторые простые эвристики сделать довольно несложно, но не обязали это делать, а значит за полное отсуствие упрощения баллы не снимаются. Для проверки корректности вычислений предлагаю использовать Вольфрам, а для упрощения мерзостей на выходе можно просто копировать и вставлять ее в поисковик Гугла, нажимать enter и получать упрощение легчайше.

12) Операции между выражениями разных типов (complex и long double) не поддерживаются.

---

## Made by Георгий К. БПИ241
