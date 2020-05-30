# Boost Software License http://www.boost.org/LICENSE_1_0.txt
# Copyright (c) 2010 The Luabind Authors

# ls test_*.cpp | sort | sed s/test_// | sed s/.cpp//
# remove typetraits lua_proxy and has_get_pointer because they are
# compile-only tests
set(LUABIND_SRC_TESTS
    abstract_base
    adopt
    adopt_wrapper
    attributes
    automatic_smart_ptr
    back_reference
    builtin_converters
    class_info
    collapse_converter
    const
    construction
    create_in_thread
    def_from_base
    destruction
    dynamic_type
    exception_handlers
    exceptions
    extend_class_in_lua
    free_functions
    function_converter
    function_introspection
    function_overload_overflow
    held_type
    implicit_cast
    implicit_raw
    index
    iterator
    lambda
    lua_classes
    null_pointer
    object
    operators
    package_preload
    policies
    private_destructors
    properties
    scope
    separation
    set_instance_value
    shadow
    shared_ptr
    lua_stack
    simple_class
    smart_ptr_attributes
    static_member
    super_leak
    table
    tag_function
    unchecked
    unsigned_int
    user_defined_converter
    vector_of_object
    virtual_inheritance
    yield
)

function(setup_compiler _target)
    set_target_properties(${_target} PROPERTIES
        CXX_STANDARD 17
        CXX_EXTENSIONS OFF
        CXX_VISIBILITY_PRESET hidden
    )
endfunction()

function(set_target_folder _target _folder)
    set_property(TARGET ${_target} APPEND PROPERTY FOLDER ${_folder})
endfunction()

enable_testing()

if(LUABIND_BUILD_TESTS)
    include("${CONAN_DOCTEST_ROOT}/lib/cmake/doctest.cmake")
    add_library(test_main STATIC main.cpp)
    set_target_folder(test_main "tests")
    setup_compiler(test_main)
    target_include_directories(test_main PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    )
    target_link_libraries(test_main PUBLIC
        luabind
        CONAN_PKG::doctest
    )
    foreach(test ${LUABIND_SRC_TESTS})
        set(test_name test_${test})
        add_executable(${test_name} ${test_name}.cpp)
        set_target_folder(${test_name} "tests")
        setup_compiler(${test_name})
        target_include_directories(${test_name} PRIVATE
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
        )
        target_link_libraries(${test_name} PRIVATE
            test_main
        )
        doctest_discover_tests(${test_name})
    endforeach()
endif()

if(LUABIND_HEADER_TEST)
    set(LUABIND_HEADERS
        ${LUABIND_INC_API}
        ${LUABIND_INC_DETAIL}
        ${LUABIND_INC_CONV_POLICIES}
        ${LUABIND_INC_USER_POLICIES}
    )
    set(LUABIND_HC_SRC
        test_headercompile.cpp
    )
    foreach(header ${LUABIND_HEADERS})
        set(header_short_name "${header}")
        string(REPLACE "/" "_" header_short_name "${header_short_name}")
        string(REPLACE ".hpp" "" header_short_name "${header_short_name}")
        set(test_cpp "${CMAKE_CURRENT_BINARY_DIR}/test_hdr_${header_short_name}.cpp")
        configure_file(test_hdr.cpp.in "${test_cpp}" @ONLY)
        set(LUABIND_HC_SRC ${LUABIND_HC_SRC} "${test_cpp}")
    endforeach()
    add_executable(test_headercompile ${LUABIND_HC_SRC})
    set_target_folder(test_headercompile "tests")
    setup_compiler(test_headercompile)
    target_include_directories(test_headercompile PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    )
    target_link_libraries(test_headercompile PRIVATE
        luabind
    )
endif()
