#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

struct argz {
    const char *name;
    int set;
};

int argz_mock(int argc, char **argv, struct argz *z) {
    int a = 1;
    while (a < argc) {
        int found = 0;
        for (int i = 0; z[i].name; i++) {
            if (strcmp(argv[a], z[i].name) == 0) {
                z[i].set = 1;
                found = 1;
                a++;
                if (a < argc && strcmp(z[i].name, "dev") == 0) {
                    a++; // dev takes an argument
                }
                break;
            }
        }
        if (!found) break;
    }
    return argc - a;
}

int main() {
    char *argv[] = {"glorytun", "bind", "1.2.3.4", "5000", "dev", "tun0", "chacha", NULL};
    int argc = 7;
    struct argz z[] = {{"dev", 0}, {"chacha", 0}, {NULL, 0}};

    printf("Simulation of gt_bind parsing:\n");
    int a = 2; // argv[2] is "1.2.3.4"
    while (a < argc) {
        int ret = argz_mock(argc - a + 1, argv + a - 1, z);
        int pos = argc - ret;
        printf("a=%d, ret=%d, pos=%d, argv[a]=%s\n", a, ret, pos, argv[a]);
        if (pos > a) {
            a = pos;
            printf("  Consumed by argz, a now %d\n", a);
            continue;
        }

        // Simulating IP parsing
        struct in_addr sa;
        if (inet_pton(AF_INET, argv[a], &sa) == 1) {
            printf("  Positional IP: %s\n", argv[a]);
            a++;
            if (a < argc) {
                char *endptr;
                unsigned long port = strtoul(argv[a], &endptr, 10);
                if (*endptr == '\0') {
                    printf("  Positional Port: %lu\n", port);
                    a++;
                }
            }
        } else {
            printf("  Unknown argument: %s\n", argv[a]);
            break;
        }
    }
    return 0;
}
