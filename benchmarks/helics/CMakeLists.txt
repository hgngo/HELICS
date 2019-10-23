# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Copyright (c) 2017-2019, Battelle Memorial Institute; Lawrence Livermore
# National Security, LLC; Alliance for Sustainable Energy, LLC.
# See the top-level NOTICE for additional details.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

set(
    HELICS_BENCHMARKS
    ActionMessageBenchmarks
    echoBenchmarks
    ringBenchmarks
    messageLookupBenchmarks
    conversionBenchmarks
	echoMessageBenchmarks
)

# Only affects current directory, so safe
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

foreach(T ${HELICS_BENCHMARKS})

    add_executable(${T} ${T}.cpp)
    target_link_libraries(${T} PUBLIC helics-static)
    add_benchmark(${T})
    set_target_properties(${T} PROPERTIES FOLDER benchmarks)

endforeach()

copy_key_files_to_target_location(ActionMessageBenchmarks)