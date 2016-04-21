/*
pivcon.cpp - Pivionics Console Main Implementation
Copyright (C) 2016 Stephen J Sullivan

This file is a part of Pivionics.

Pivionics is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pivionics is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pivionics.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <csignal>

#include "console.h"

void signal_catcher( int signum ) {
    //do something
}

int main(int argc,char* argv[]) {
    // Uncomment the following to prevent ctrl+c from quitting -- NOT ADVISIBLE AT THIS TIME
    //signal(SIGINT,signal_catcher);
    main_fn();
    return 0;
}
