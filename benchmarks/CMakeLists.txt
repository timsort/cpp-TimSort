
foreach(filename bench.cpp)
    get_filename_component(name ${filename} NAME_WE)
    add_executable(${name} ${filename})
    target_link_libraries(${name} PRIVATE gfx::timsort)
endforeach()
