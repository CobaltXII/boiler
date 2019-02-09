// Verlet sandbox? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

// When jelly mode is active, shapes use spring constraints instead of rigid
// constraints. Take a look at constraint::do_constraint.

bool jelly = false;
