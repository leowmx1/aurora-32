int main() {
    print("Type keys; q exits. Input uses callbacks + WaitEvent.");
    int c = 0;
    while (c != 'q') {
        c = getchar();
        if (c != 'q') {
            putchar(c);
        }
    }
    print("\nBye!");
    return 0;
}
