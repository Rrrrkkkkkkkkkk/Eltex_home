#include <stdio.h>

#define TWO 2

int main() {
  int n;
  int result = 1;
  int *result_2 = &result;

  printf("Натуральное число N: ");
  if (scanf("%d", &n) != 1) {
    fprintf(stderr, "Ошибка ввода.\n");
    return 1;
  }

  for (int i = 0; i < n; i++) {
      *result_2 *= TWO;
  }

  printf("2 ^ %d = %d\n", n, result);
  return 0;
}

