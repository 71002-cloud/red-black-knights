# Red-Black Knights

C++ version of my [red_and_black_knights](https://github.com/71002-cloud/solution/tree/master/projects) Python project.

It places colored knights on a numbered board. Writing the result to
`knights.txt` is enabled in `main()`.

The number of knights and board size are configured when constructing the
`Game` object. The current implementation supports up to eight knights,
using the available colors in order.

## Build and run

Using **MSYS2 UCRT64**:

```bash
g++ main.cpp -o main.exe
./main.exe
```

The example in `main()` uses two knight colors on a board with size `3001`.
This C++ version is approximately five times faster than the Python version.