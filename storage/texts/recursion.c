// Recursive calls keep their own local variables on the user stack.
int factorial(int n) {
    if (n < 2) return 1;
    return n * factorial(n - 1);
}
int fibonacci(int n) {
    if (n < 2) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}
int main() {
    print("factorial(6), fibonacci(10):");
    print(factorial(6));
    print(fibonacci(10));
    return 0;
}
