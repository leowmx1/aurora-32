// Arrays decay to word pointers when passed to functions.
int sort(int *values, int n) {
    int i = 0;
    while (i < n) {
        int j = 0;
        while (j + 1 < n - i) {
            if (values[j] > values[j + 1]) {
                int saved = values[j];
                values[j] = values[j + 1];
                values[j + 1] = saved;
            }
            j = j + 1;
        }
        i = i + 1;
    }
    return 0;
}
int main() {
    int values[5];
    values[0] = 5;
    values[1] = 2;
    values[2] = 4;
    values[3] = 1;
    values[4] = 3;
    sort(values, 5);
    print("Sorted:");
    int i = 0;
    while (i < 5) {
        print(values[i]);
        i = i + 1;
    }
    return 0;
}
