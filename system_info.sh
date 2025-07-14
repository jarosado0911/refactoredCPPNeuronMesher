#!/bin/bash

# Save output to both terminal and system_info.log
{
    echo "=================== System Information ==================="
    
    echo -n "Linux Terminal Version: "
    uname -a

    echo -n "CMake Version: "
    cmake --version | head -n 1

    echo -n "Make Version: "
    make --version | head -n 1

    echo -n "GCC (C Compiler) Version: "
    gcc --version | head -n 1

    echo -n "G++ (C++ Compiler) Version: "
    g++ --version | head -n 1

    echo -n "GNU Version (coreutils): "
    dpkg -s coreutils 2>/dev/null | grep Version || coreutils --version | head -n 1

	echo "=========================================================="

} | tee logs/system_info.log
