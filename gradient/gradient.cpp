/*

Gradient

Press E to export a gradient. You can then construct a gradient object using
the exported code. The function for this is gradient_from_string.

A good idea is to use coolors.co to generate a cool palette. Then copy the hex
part of the URL, like "b9d6f2-061a40-0353a4-006daa-003559". Then run this
application like this:

	./gradient.out -H "b9d6f2-061a40-0353a4-006daa-003559"

*/

#include "../boiler/boiler.h"

#include "gradient.h"

#include <vector>
#include <utility>
#include <iostream>
