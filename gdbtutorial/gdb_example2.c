#include <stdio.h>
#include <stdlib.h>

void setint(int* ip, int i) {
    *ip = i;
} 


int main() {
    int a;
    int* b;

    setint(&a, 10);
    printf("%d\n", a);

    setint(b, 20);
    printf("%ld\n", *b);
    return 0;
}
