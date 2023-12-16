#define XXH_STATIC_LINKING_ONLY /* access advanced declarations */
#define XXH_IMPLEMENTATION      /* access definitions */

#include "xxhash.h"
#include <stdio.h>

uint64_t hash_integer(const void* data, size_t size) {
    return XXH64(data, size, 0);
}

uint64_t test(int number) {
    return XXH64(&number, sizeof(number),0);
}

uint64_t hash(int x, int y){
    uint64_t x_hashed = test(x);
    uint64_t y_hashed = test(y);
    return (x_hashed ^ (y_hashed << 1)); 
}

int main() {
    int yourInteger = 42;
    int intest = 10000000;

    // Calculate the 64-bit hash value for the integer
    uint64_t hashValue = hash_integer(&yourInteger, sizeof(yourInteger));

    printf("xxHash 64-bit hash value for integer %d: %llu\n", yourInteger, (unsigned long long)hashValue);

    hashValue = test(yourInteger);

    printf("xxHash 64-bit hash value for integer %d: %llu\n", yourInteger, (unsigned long long)hashValue);

    hashValue = hash(yourInteger, intest);

    printf("xxHash 64-bit hash value for integer %d: %llu\n", yourInteger, (unsigned long long)hashValue);

    return 0;
}