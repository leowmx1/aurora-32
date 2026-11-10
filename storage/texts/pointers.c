int swap(int *a, int *b) {
    int saved = *a;
    *a = *b;
    *b = saved;
    return 0;
}
int length(char *text) {
    int n = 0;
    while (text[n]) n = n + 1;
    return n;
}
int main() {
    int a = -7;
    int b = 42;
    swap(&a, &b);
    print("Swapped (42, -7):");
    print(a);
    print(b);
    print("Word string length (6):");
    print(length("Aurora"));
    char chars[2];
    chars[0] = 1000;
    print("char stores a full 32-bit word:");
    print(chars[0]);
    return 0;
}
