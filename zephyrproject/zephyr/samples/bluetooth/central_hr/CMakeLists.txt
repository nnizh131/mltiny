# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(central_hr)




# add_subdirectory(src/CMSIS/NN)

# target_link_libraries(app PRIVATE cmsis-nn)

include_directories(src/CMSIS/Core/Include)
add_subdirectory(src/CMSIS/DSP)
target_link_libraries(app PRIVATE CMSISDSP)
target_link_directories(app PUBLIC src/CMSIS/DSP)












# target_link_libraries(app PRIVATE cmsis-dsp)

# target_link_directories(app PUBLIC src/CMSIS/DSP)

# set(DSP src/CMSIS/DSP)

### Add DSP folder to module path
# list(APPEND CMAKE_MODULE_PATH ${DSP})


### Load CMSIS-DSP definitions. Libraries will be built in bin_dsp
# add_subdirectory(${DSP}/Source bin_dsp)


# list(APPEND CMAKE_MODULE_PATH ${DSP}) 



# add_subdirectory(src/CMSIS/DSP/Source)

# target_link_libraries(app PRIVATE cmsis-dsp)




file(GLOB all_INCS
        # "src/codegen/Include"
        # "src/TinyEngine/include"
        # "src/nnom/inc"
        # "src/nnom/port"
        # "src/nnom/inc/layers"
        "src/pdda"
        )



target_include_directories(app PUBLIC ${all_INCS})

include_directories(src/CMSIS/Core/Include)
# add_subdirectory(src/CMSIS/DSP)
# target_include_directories(app PUBLIC ${all_INCS})
# include_directories(src/CMSIS/Core/Include)
# add_subdirectory(src/CMSIS/DSP)

file(GLOB all_SRCS
        # "src/codegen/Source/*.c"
        # "src/TinyEngine/Source/*.c"
        # "src/nnom/src/core/*.c"
        # "src/nnom/src/layers/*.c"
        # "src/nnom/src/backends/*.c"
        "src/pdda/*.c"
        "src/*.c"
        )

        
# add_executable(app ${all_SRCS})

# target_include_directories(app PRIVATE ${app_includes})
target_sources(app PRIVATE ${all_SRCS})

zephyr_library_include_directories(${ZEPHYR_BASE}/samples/bluetooth)



