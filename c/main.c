#include <stdio.h>

int main() {
int x = 10;
int *ptr = &x;

printf("Value of x: %d\n", x);
printf("Address of x: %p\n", &x);
printf("Value of ptr parameter: %p\n", ptr);
printf("Value stored in ptr definition: %d\n", *ptr);

scanf("%d", &x);
return 0;
}