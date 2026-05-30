# КР 1

# Задача: Бинарная матрица — работа с диагоналями

## Общее условие

В бинарном файле хранится **квадратная матрица**, состоящая из байтов (каждый байт — 0 или 1).

Размер матрицы `N × N` заранее неизвестен. 
Его необходимо определить: `N = √(общее количество байтов в файле)`.

### Требования (общие для всех вариантов):

1. Определить размер матрицы `N`, используя `lseek` для получения размера файла.
2. Вычислить сумму элементов согласно одному из вариантов ниже.
3. Записать полученную сумму в выходной файл (в текстовом виде, как число).
4. Для чтения и позиционирования использовать только системные вызовы:  
   `open`, `close`, `read`, `write`, `lseek`.

---

## Варианты задачи

### Вариант 1: Сумма элементов НАД главной диагональю

Найти сумму всех элементов, где `j > i` (строго выше главной диагонали).

**Формула:**  
`sum = Σ matrix[i][j]` для всех `i от 0 до N-1`, `j от i+1 до N-1`

**Пример** (матрица 3×3):

1 0 1
0 1 0
1 0 1

Элементы над диагональю: `[0][1]=0`, `[0][2]=1`, `[1][2]=0` → **сумма = 1**

---

### Вариант 2: Сумма элементов ПОД главной диагональю

Найти сумму всех элементов, где `j < i` (строго ниже главной диагонали).

**Формула:**  
`sum = Σ matrix[i][j]` для всех `i от 0 до N-1`, `j от 0 до i-1`

**Пример** (матрица 3×3):

1 0 1
0 1 0
1 0 1

Элементы под диагональю: `[1][0]=0`, `[2][0]=1`, `[2][1]=0` → **сумма = 1**

---

### Вариант 3: Сумма элементов НА побочной диагонали

Найти сумму всех элементов, где `j = N - 1 - i` (побочная диагональ).

**Формула:**  
`sum = Σ matrix[i][N-1-i]` для всех `i от 0 до N-1`

**Пример** (матрица 3×3):

1 0 1
0 1 0
1 0 1

Побочная диагональ: `[0][2]=1`, `[1][1]=1`, `[2][0]=1` → **сумма = 3**

---

### Вариант 4: Сумма элементов НАД побочной диагональю

Найти сумму всех элементов, где `j > N - 1 - i` (выше побочной диагонали).

**Формула:**  
`sum = Σ matrix[i][j]` для всех `i`, `j` таких, что `j > N-1-i`

**Пример** (матрица 3×3):

1 0 1
0 1 0
1 0 1

Элементы над побочной: `[0][1]=0`, `[1][0]?` нет (0 < 1?), `[1][2]=0` → **сумма = 0**

---

### Вариант 5: Сумма элементов ПОД побочной диагональю

Найти сумму всех элементов, где `j < N - 1 - i` (ниже побочной диагонали).

**Формула:**  
`sum = Σ matrix[i][j]` для всех `i`, `j` таких, что `j < N-1-i`

**Пример** (матрица 3×3):

1 0 1
0 1 0
1 0 1

Элементы под побочной: `[1][0]=0`, `[2][0]=1`, `[2][1]=0` → **сумма = 1**

---

## Формат входного файла

- Бинарный файл, содержащий `N × N` байт, идущих **по строкам**:

[0][0], [0][1], ..., [0][N-1], [1][0], [1][1], ..., [N-1][N-1]




- Каждый байт — либо `0`, либо `1`.

---

## Входные данные (аргументы командной строки)

```bash
./program <input.bin> <output.txt> 
```

# Вариант 1: сумма над главной диагональю


```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) return 1;
    
    int fd = open(argv[1], O_RDONLY);
    int size = lseek(fd, 0, SEEK_END);
    
    int N = 0;
    while (N * N < size) N++;
    if (N * N != size) return 1;
    
    int sum = 0;
    unsigned char byte;
    
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            lseek(fd, i * N + j, SEEK_SET);
            read(fd, &byte, 1);
            sum += byte;
        }
    }
    
    close(fd);
    
    int out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dprintf(out, "%d\n", sum);
    close(out);
    
    return 0;
}

```

# Вариант 2: сумма под главной диагональю

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) return 1;
    
    int fd = open(argv[1], O_RDONLY);
    int size = lseek(fd, 0, SEEK_END);
    
    int N = 0;
    while (N * N < size) N++;
    if (N * N != size) return 1;
    
    int sum = 0;
    unsigned char byte;
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < i; j++) {
            lseek(fd, i * N + j, SEEK_SET);
            read(fd, &byte, 1);
            sum += byte;
        }
    }
    
    close(fd);
    
    int out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dprintf(out, "%d\n", sum);
    close(out);
    
    return 0;
}

```


# Сумма на побочной

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) return 1;
    
    int fd = open(argv[1], O_RDONLY);
    int size = lseek(fd, 0, SEEK_END);
    
    int N = 0;
    while (N * N < size) N++;
    if (N * N != size) return 1;
    
    int sum = 0;
    unsigned char byte;
    
    for (int i = 0; i < N; i++) {
        int j = N - 1 - i;
        lseek(fd, i * N + j, SEEK_SET);
        read(fd, &byte, 1);
        sum += byte;
    }
    
    close(fd);
    
    int out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dprintf(out, "%d\n", sum);
    close(out);
    
    return 0;
}

```

# сумма над побочной
```c

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) return 1;
    
    int fd = open(argv[1], O_RDONLY);
    int size = lseek(fd, 0, SEEK_END);
    
    int N = 0;
    while (N * N < size) N++;
    if (N * N != size) return 1;
    
    int sum = 0;
    unsigned char byte;
    
    for (int i = 0; i < N; i++) {
        for (int j = N - i; j < N; j++) {
            lseek(fd, i * N + j, SEEK_SET);
            read(fd, &byte, 1);
            sum += byte;
        }
    }
    
    close(fd);
    
    int out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dprintf(out, "%d\n", sum);
    close(out);
    
    return 0;
}
```


# Сумма под побочной диагональю
```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) return 1;
    
    int fd = open(argv[1], O_RDONLY);
    int size = lseek(fd, 0, SEEK_END);
    
    int N = 0;
    while (N * N < size) N++;
    if (N * N != size) return 1;
    
    int sum = 0;
    unsigned char byte;
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N - 1 - i; j++) {
            lseek(fd, i * N + j, SEEK_SET);
            read(fd, &byte, 1);
            sum += byte;
        }
    }
    
    close(fd);
    
    int out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dprintf(out, "%d\n", sum);
    close(out);
    
    return 0;
}
```




















# КР 2

# АКОС - Неделя 7: Межпроцессное взаимодействие через pipes

## Условие

На вход программе передаётся имя файла, содержащего пары **32-битных знаковых целых чисел** (диапазон: -2,147,483,648 до 2,147,483,647).

Числа в файле разделены пробелами, количество чисел гарантированно чётное.

### Требуется создать три процесса:

1. **Процесс Родитель (Parent)**
   - Открывает файл
   - Читает пары чисел (a, b)
   - Передаёт каждую пару процессу внука через первый pipe

2. **Процесс Внук (Grandchild)**
   - Получает пары (a, b) от родителя
   - **Меняет местами:** (a, b) → (b, a)
   - Передаёт изменённые пары процессу сына через второй pipe

3. **Процесс Сын (Child)**
   - Получает пары (b, a) от внука
   - Вычисляет разницу: **diff = b - a**
     - где b — второе число исходной пары
     - где a — первое число исходной пары
   - Находит **максимальную разницу**
   - Выводит одно число: max_diff

## Входные данные

```
5 10  2 8
3 9
```

## Вычисления

| Пара из файла | После swap | Разница (b - a) |
|---|---|---|
| (5, 10) | (10, 5) | 10 - 5 = 5 |
| (2, 8) | (8, 2) | 8 - 2 = 6 |
| (3, 9) | (9, 3) | 9 - 3 = 6 |

## Вывод

```
6
```

(максимум из 5, 6, 6)

## Требования

- Использовать **int32_t** для 32-битных знаковых чисел
- Взаимодействие только через pipes
- Корректная обработка ошибок
- Закрывать неиспользуемые концы pipes в каждом процессе


## Решение

```c
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int p1[2], p2[2];
    pipe(p1); pipe(p2);
    
    if (fork() == 0) {  // Сын (считает максимум)
        close(p1[0]); close(p1[1]); close(p2[1]);
        int32_t a, b, max = -2147483648;
        while (read(p2[0], &a, 8) == 8) {
            int32_t diff = b - a;  // b и a приходят в порядке (b, a) от внука
            if (diff > max) max = diff;
        }
        printf("%d\n", max);
        return 0;
    }
    
    if (fork() == 0) {  // Внук (меняет местами)
        close(p1[1]); close(p2[0]);
        int32_t a, b;
        while (read(p1[0], &a, 8) == 8) {
            write(p2[1], &b, 4);
            write(p2[1], &a, 4);
        }
        return 0;
    }
    
    // Родитель (читает файл)
    close(p1[0]); close(p2[0]); close(p2[1]);
    FILE *f = fopen(argv[1], "r");
    int32_t a, b;
    while (fscanf(f, "%d %d", &a, &b) == 2) {
        write(p1[1], &a, 4);
        write(p1[1], &b, 4);
    }
    wait(NULL); wait(NULL);
}
```


# КР 3

# Задача: Подсчёт пар сигналов USR1 → USR2

**Проект:** spring/kr03-1-1-KP 3 (пара 1), задача 1

**Оценка:** 100 баллов  
**Штраф за запуск:** 2  
**Лимит времени:** 200 мс  
**Лимит реального времени:** 1 с  
**RSS лимит памяти:** 128M  
**Лимит стека:** 8M  

**Дата открытия:** 2026-04-20 14:40:00  
**Дедлайн:** 2026-04-20 15:50:00

---

## Условие

Программа должна напечатать в стандартный поток вывода свой **pid**.

После этого программе будут отправляться сигналы `USR1` и `USR2`. 

Программа должна подсчитывать количество ситуаций, когда **следующим после сигнала USR1 пришёл сигнал USR2**.

При получении сигнала `TERM` программа должна:
- Вывести в стандартный поток вывода значение счётчика
- Завершить работу с кодом 0

---

## Технические требования

- Для работы с сигналами использовать **только `sigaction`**
- Обработчик сигнала должен использовать **только безопасные (async-signal-safe) функции**

---

## Язык реализации

**GCC - GNU C 15.2.0**

---

## Решение



```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t need = 0;
volatile sig_atomic_t cnt = 0;

void handle(int sig) {
    if (sig == SIGUSR1) need = 1;
    if (sig == SIGUSR2 && need) { cnt++; need = 0; }
    if (sig == SIGTERM) {
        // безопасный вывод: пишем число по цифрам
        char buf[16];
        int n = cnt;
        int len = 0;
        
        if (n == 0) {
            write(1, "0\n", 2);
        } else {
            char temp[16];
            int i = 0;
            while (n > 0) {
                temp[i++] = '0' + (n % 10);
                n /= 10;
            }
            for (int j = i - 1; j >= 0; j--) {
                buf[len++] = temp[j];
            }
            buf[len++] = '\n';
            write(1, buf, len);
        }
        _exit(0);
    }
}

int main() {
    printf("%d\n", getpid());
    fflush(stdout);
    
    struct sigaction sa = {0};
    sa.sa_handler = handle;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    
    while (1) pause();
}
```

# КР 4

# Задача: Потоки и зависимости (КР №4)

## Условие

Дано 8 потоков с индексами от 0 до 7. Каждый поток должен вывести свой индекс в **stdout** в определённом порядке, используя механизм **pthread_join** для синхронизации.

Требуется построить цепочку зависимостей между потоками такую, чтобы вывод происходил в следующем порядке:

7
0
6
1
5
2
4
3


## Правила

1. Каждый поток представлен функцией `void *f(void *a)`, где `a` — указатель на индекс потока.
2. Внутри функции поток может вызывать `pthread_join` для ожидания завершения **других** потоков.
3. Поток выводит свой индекс **только после того**, как все потоки, от которых он зависит, завершились.
4. Запрещено использовать другие механизмы синхронизации (мьютексы, семафоры, условные переменные и т.д.).
5. Разрешены только `pthread_create`, `pthread_join` и `printf`.

## Решение


```c
#include <stdio.h>
#include <pthread.h>

pthread_t t[8];

void *f(void *a) {
    int i = *(int*)a;
    
    // Выстраиваем цепочку: 7 -> 0 -> 6 -> 1 -> 5 -> 2 -> 4 -> 3
    if (i == 0) pthread_join(t[7], NULL);
    if (i == 6) pthread_join(t[0], NULL);
    if (i == 1) pthread_join(t[6], NULL);
    if (i == 5) pthread_join(t[1], NULL);
    if (i == 2) pthread_join(t[5], NULL);
    if (i == 4) pthread_join(t[2], NULL);
    if (i == 3) pthread_join(t[4], NULL);
    // i == 7 не ждёт никого
    
    printf("%d\n", i);
    return NULL;
}

int main() {
    int id[8];
    for (int i = 0; i < 8; i++) {
        id[i] = i;
        pthread_create(&t[i], NULL, f, &id[i]);
    }
    pthread_join(t[3], NULL);  // ждём последнюю в цепочке
    return 0;
}
```          