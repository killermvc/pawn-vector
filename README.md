# pawn-vector

[![sampctl](https://shields.southcla.ws/badge/sampctl-pawn--vector-2f2f2f.svg?style=for-the-badge)](https://github.com/killermvc/pawn-vector)

## Description

Vector data structure implemented using y_malloc

## Installation

Simply install to your project:

```bash
sampctl package install killermvc/pawn-vector
```

Include in your code and begin using the library:

```pawn
#include <pawn-vector>
```

## Usage

### Defines

* `#define VEC_DEFAULT_CAPACITY    (50)`
  * Size (in cells) of the underlying array when creating a vector
* `#define VEC_DEFAULT_INCREASE    (5)`
  * Amount of cells to increase the underlying array if there isn't enough space for an append

You can `#define` both before including the package to change these values

Documentation moved to the [wiki](https://github.com/killermvc/pawn-vector/wiki)

## Testing

To test, simply run the package:

```bash
sampctl package run
```
