# `dat2header`

This applications translate a `.dat` file from `hls4ml` in a header file (`.h`).

## Quick Test

```
cd sim
make run
```

Look at the generated `sim/data.h` file and compare it with `test/data.dat`.

## Command Line Parameters

The application takes 4 parameters
- The input data file (`<path_to>/<data>.dat`)
- The output header file (`<path_to>/<data>.h`)
- The name of the array to where to store data (e.g. `input`)
- The maximum number of feature (lines) to process from the data file (`3 `)

```
dat2header path_to/data.dat path_to/data.h input 3 
```
