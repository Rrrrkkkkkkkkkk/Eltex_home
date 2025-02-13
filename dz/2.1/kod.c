#include <stdio.h>

#define TWO 2

int main() {
  int n;

  printf("Введите натуральное число N: ");
  scanf("%d", &n);

  int result = 1;
  for (int i = 0; i < n; i++) {
    result *= TWO;
  }

  printf("2 ^ %d = %d\n", n, result);

  return 0;
}

