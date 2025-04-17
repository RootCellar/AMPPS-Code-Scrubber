
# Memory Correction Algorithm Usage

## Performing a scrub

### Define the Pointer list

In order to call `correct_errors`, you need to have a list containing the
memory locations of the copies of data. In the list, these must be
in order of which FRAM memory segment they are in.

For example:

```c++
const char* data_copies[3] = { (char*)0x4950, (char*)0x6100, (char*)0x10000 };
```

1. `0x4950` is where the copy of code in segment 1 is located
2. `0x6100` is where the copy of code in segment 2 is located
3. `0x10000` is where the copy of code in segment 3 is located

Change these values as appropriate.

### Define Data Size and Number of Copies

Before calling `correct_errors`, you should define constants for
how many copies of data there are, and how large the data is:

```c++
#define NUM_DATA_COPIES (3)
#define DATA_SIZE       (4096) // Bytes
```

Change the value of `DATA_SIZE` to the size of the data segment that will be scrubbed, in bytes.
This value **must** be evenly divisible by the number of bytes in `SEEK_TYPE` in `memory_correction.h`.

For example, if `SEEK_TYPE` is `int16_t`, then `DATA_SIZE` must be evenly divisible by `2`.

Notes:
* You can change the names of these constants
* You can (*and may need to*) define more constants like `DATA_SIZE` that are each used
for different scrubs
* These values do **not** have to be defined as constants, but they probably should be.
* Because of how `environment.h` is implemented for the microcontroller,
`NUM_DATA_COPIES` **can not** be any value other than `3`. This can be changed
by modifying `environment.h`

### Call `correct_errors`

Once the required information is known, you can run the algorithm to fix bit flips.

Call `correct_errors:`

```c++
int32_t corrections = correct_errors(data_copies, NUM_CODE_COPIES, DATA_SIZE);
```

Notes:
* `correct_errors` returns how many bit flips it found and fixed.
This return value can be ignored if you have no use for it

## Extras

### Calling `correct_errors` In a Different Copy of Code

```c++
uintptr_t ptr = (uintptr_t) correct_errors;
ptr -= (uintptr_t) data_copies[2];
ptr += (uintptr_t) data_copies[1];
int (*func)(char**, int, int) = (int (*)(char**,int,int)) ptr;
runCorrections = func(data_copies, NUM_COPIES, TEXT_SIZE);
```

## Notes

* The algorithm supports using any number of copies of code, arranged in any way.
However, the `environment.h` implementation for the MSP430 adds a few restrictions
due to how `LOCK_MEMORY_SEGMENT` and `UNLOCK_MEMORY_SEGMENT` were implemented