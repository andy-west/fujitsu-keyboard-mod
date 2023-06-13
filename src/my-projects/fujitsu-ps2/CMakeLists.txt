add_executable(fujitsu-ps2
        ps2dev.cpp
        keyboard.cpp
        fujitsu-ps2.cpp
        )

# pull in common dependencies
target_link_libraries(fujitsu-ps2 pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(fujitsu-ps2)

# add url via pico_set_program_url
example_auto_set_url(fujitsu-ps2)
