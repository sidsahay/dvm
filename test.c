int foo(int blah) {
    return blah + blah;
}

int bar(int baz, int bleh) {
    bleh = foo(baz) + bleh;
    return foo(bleh);
}

int _start() {
    int x = 2;
    int y = 3;
    int z;
    z = bar(x, y);
    return z;
}
