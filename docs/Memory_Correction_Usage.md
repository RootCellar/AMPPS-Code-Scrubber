
# Memory Correction Algorithm Usage

## Performing a scrub

### Define the Pointer list

```c++
char* data_copies[3];
data_copies[0] = (char*) 0x4950;
data_copies[1] = (char*) 0x6100;
data_copies[2] = (char*) 0x10000;
```

### Call `correct_errors`

```c++
runCorrections = correct_errors(data_copies, NUM_CODE_COPIES, TEXT_SIZE);
```

## Extras

### Calling `correct_errors` In a Different Copy of Code

```c++
uintptr_t ptr = (uintptr_t) correct_errors;
ptr -= (uintptr_t) data_copies[2];
ptr += (uintptr_t) data_copies[1];
int (*func)(char**, int, int) = (int (*)(char**,int,int)) ptr;
runCorrections = func(data_copies, NUM_COPIES, TEXT_SIZE);
```