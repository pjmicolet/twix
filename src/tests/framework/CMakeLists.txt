add_library(test_framework INTERFACE
   testing.hpp
   result_format.hpp
   assertions.hpp
)

#this is messed up I should probably move it back one ?
target_include_directories(test_framework INTERFACE ..)
