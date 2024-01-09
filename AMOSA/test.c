#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    int* k1;
    int* k2;
} K;

K* asd(int count) {
    if (count >= 3) {
        K* res = malloc(sizeof(K));
        int a[2] = {0, 0};
        int b[2] = {1, 1};

        res->k1 = malloc(2 * sizeof(int));
        res->k2 = malloc(2 * sizeof(int));
        
        res->k1[0] = a[0];
        res->k1[1] = a[1];
        res->k2[0] = b[0];
        res->k2[1] = b[1];

        for (int i = 0; i < 2; i++) {
            printf("%d,%d,%d\n", res->k1[i], res->k2[i], count);
        }
        return res;
    } else {
        count++;
        return asd(count);
    }
}

int main() {
    int count = 0;
    K* result = asd(count);
    
    // 메모리 해제
    free(result->k1);
    free(result->k2);
    free(result);

    return 0;
}
