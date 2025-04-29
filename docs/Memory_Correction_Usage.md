
# Memory Correction Algorithm Usage

## Performing a scrub

### Define the Pointer list

In order to call `correct_errors`, you need to have a list containing the
memory locations of the copies of data. In the list, these must be
IN ORDER of which FRAM memory segment they are in.

For example:

```c++
#pragma PERSISTENT(data_copies)
const char* data_copies[3] = { (char*)0x4950, (char*)0x6100, (char*)0x10000 };
```

1. `0x4950` is where the copy of code in segment 1 is located
2. `0x6100` is where the copy of code in segment 2 is located
3. `0x10000` is where the copy of code in segment 3 is located
4. `#pragma PERSISTENT` is used on `data_copies` to store it in FRAM.

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
Note that `SEEK_TYPE` can be changed. You will probably want to set `SEEK_TYPE` to `int16_t`
if you are using this code on an MSP430FR5969, as the MSP has a 16-bit processor.

Notes:
* You can change the names of these constants
* You can (*and may need to*) define more constants like `DATA_SIZE` that are each used
for different scrubs
* These values do **not** have to be defined as constants, but they probably should be.
* Because of how `environment.h` is implemented for the microcontroller,
`NUM_DATA_COPIES` **can not** be any value other than `3`. However, this can be changed
by modifying `environment.h`

### Call `correct_errors`

Once the required information is known, you can run the algorithm to fix bit flips.

Call `correct_errors:`

```c++
int32_t corrections = correct_errors(data_copies, NUM_DATA_COPIES, DATA_SIZE);
```

Notes:
* `correct_errors` returns how many bit flips it found and fixed.
This return value can be ignored if you have no use for it

## Extras

### Calling `correct_errors` In a Different Copy of Code

Example:

```c++
int booted_copy = 2;
int copy_to_run_from = 1;

uintptr_t ptr = (uintptr_t) correct_errors;
ptr -= (uintptr_t) data_copies[booted_copy];
ptr += (uintptr_t) data_copies[copy_to_run_from];
int32_t (*func)(char**, int, int32_t) = (int32_t (*)(char**,int,int32_t)) ptr;
int32_t corrections = func(data_copies, NUM_DATA_COPIES, DATA_SIZE);
```

Line-by-line Explanation:
1. Get the memory address of the `correct_errors` function and convert it to a number
2. Convert `data_copies[booted_copy]` to a number and subtract from `ptr` to find the offset
of `correct_errors` from the beginning of the program's code
3. Convert `data_copies[copy_to_run_from]` to a number and add, to compute where `correct_errors` is
located in FRAM memory segment 2
4. Convert the `ptr` memory address from a number to a function pointer
5. Call the function and get the return value

### Descriptions of the Functions

#### `correct_errors`

* Seeks through the data copies using `SEEK_TYPE` to determine whether the copies
"agree" about the data at each memory location

#### `correct_bits`

* Should not be used directly
* Called by `correct_errors`
* Fixes the errors found by `correct_errors`
* Operates on a bit-by-bit basis and is unaffected by the `SEEK_TYPE`

### Copying code to other segments

Copying code to other segments is done via `memcpy`. Make sure to unlock the segment of code you are trying to write to before trying to write to it. The actual call to it should look something like this:

```c++
memcpy(DST_pointer, SRC_pointer, num_bytes);
```

* `DST_pointer` is your destination address
* `SRC_pointer` is your source address
* `num_bytes` should be the same as `DATA_SIZE`

## Notes

* The algorithm supports using any number of copies of code, arranged in any way.
However, the `environment.h` implementation for the MSP430 adds a few restrictions
due to how `LOCK_MEMORY_SEGMENT` and `UNLOCK_MEMORY_SEGMENT` were implemented
