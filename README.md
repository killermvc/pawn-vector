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


### Functions

* `Vec:Vec_CreateVector`
Creates a new vector.

  **parameters:**
  
  * `capacity = VEC_DEFAULT_CAPACITY`
    Size (in cells) of the underlying array
  * `bool:ordered = false`
    whether the vector should remain ordered when removing a value

* `Vec:Vec_CreateVectorFromArray`
Creates a new vector and initializes it with the values stored in an array.

  **parameters:**
  * `const arr[]`
    the array to copy the data from
  * `arrSize`
    the size of the array, the array will be copies from the start to this size
  * `capacity = VEC_DEFAULT_CAPACITY`
    Size (in cells) of the underlying array.
  * `bool:ordered = false`
    whether the vector should remain ordered when removing a value.

* `Vec_GetLength`
  Returns the number of elements appended to the vector.
  
  **parameters:**
  * `Vec:vec`
    the vector to get length from.

* `Vec_GetCapacity`
  Returns the capacity of the vector.
  
  **parameters:**
  * `Vec:vec`
    the vector to get length from.

* `bool:Vec_IsOrdered`
  Returns if the vector is ordered or not.
  
  **parameters:**
  * `Vec:vec`
    the vector to get length from.

* `Vec_Resize`
  changes the capacity of the vector.
  
  **parameters:**
  * `Vec:vec`
    the vector to resize.
  * `newSize`
    the new size (in cells).

* `Vec_Append`
  appends a value to the end of the vector.
  
  **parameters:**
  * `Vec:vec`
    the vector to append to.
  * `value`
    the value to append.

* `Vec_AppendArray`
  appends all the values in the array to the end of the vector.
  
  **parameters:**
  * `Vec:vec`
    the vector to append to.
  * `const arr[]`
    the array to append.
  * `arrSize = sizeof arr`
    the size of the array.

* `Vec_GetValue`
  reads a value from the vector.
  
  **parameters:**
  * `Vec:vec`
    the vector to get the value from.
  * `index`
    the index to read from.

* `Vec_SetValue`
  reads a value from the vector
  
  **parameters:**
  * `Vec:vec`
    the vector to set the value to.
  * `index`
    the index to set to.

* `Vec_Remove`
    removes a value from the vector
  * if the vector isn't ordered the function copies the last element to the specified index and decreases the length of the vector
  * if the vector is ordered it copies every element past the specified index to the preceding index.

  **parameters:**
  * `Vec:vec`
    the vector to remove from.
  * `index`
    the index of the element to remove.

* `Vec_Delete`
    deletes a vector and frees the memory
    
    **parameters:**
  * `Vec:vec`
    the vector to delete.

## Testing

To test, simply run the package:

```bash
sampctl package run
```
