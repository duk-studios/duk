# enable parallel build for msvc
add_compile_options(
        /MP     # parallel builds
        /W4     # warning level 4
        /WX     # warnings as errors

        # it would be nice to not disable any warnings, but some of them easily
        # become noise and hide relevant errors in a flood of warnings.
        # therefore we disable them for now
        /wd4267 # 'var' : conversion from 'size_t' to 'type', possible loss of data
        /wd4244 # 'argument' : conversion from 'type1' to 'type2', possible loss of data
        /wd4100 # 'identifier' : unreferenced formal parameter
        /wd4018 # 'token' : signed/unsigned mismatch
        /wd4245 # 'conversion' : conversion from 'type1' to 'type2', signed/unsigned mismatch
        /wd4702 # unreachable code
)