# Boost Software License http://www.boost.org/LICENSE_1_0.txt
# Copyright (c) 2010 The Luabind Authors

cmake_minimum_required(VERSION 3.16)
project(luabind)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(LUABIND_BUILD_SHARED "Build shared library" ON)
option(LUABIND_NO_EXCEPTIONS "Disable exeptions" ON)
option(LUABIND_CUSTOM_ALLOCATOR "Use user provided allocator" OFF)
option(LUABIND_NUMBER_TO_STRING_CONVERSION "Allow native converter from number to string\\char*" ON)
option(LUABIND_ALLOW_MISSING_ARGUMENTS "Function calls with missing arguments are not treated as errors" OFF)
option(LUABIND_CLASS_IN_NAMESPACE "Try to add class to the namespace if local "this" exists" ON)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /permissive-")
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

set(LUABIND_INC_API
	luabind/back_reference_fwd.hpp
	luabind/back_reference.hpp
	luabind/class.hpp
	luabind/class_info.hpp
	luabind/config.hpp
	luabind/error.hpp
	luabind/error_callback_fun.hpp
	luabind/exception_handler.hpp
	luabind/from_stack.hpp
	luabind/function.hpp
	luabind/function_introspection.hpp
	luabind/get_main_thread.hpp	
	luabind/handle.hpp
	luabind/lua_argument_proxy.hpp
	luabind/lua_extensions.hpp
	luabind/lua_include.hpp
	luabind/lua_index_proxy.hpp
	luabind/lua_iterator_proxy.hpp
	luabind/lua_proxy.hpp
	luabind/lua_proxy_interface.hpp
	luabind/lua_stack.hpp
	luabind/lua_state_fwd.hpp
	luabind/luabind.hpp
	luabind/make_function.hpp
	luabind/memory.hpp
	luabind/memory_allocator.hpp
	luabind/nil.hpp
	luabind/object.hpp
	luabind/open.hpp
	luabind/operator.hpp
	luabind/pointer_traits.hpp
	luabind/prefix.hpp
	luabind/scope.hpp
	luabind/set_package_preload.hpp
	luabind/shared_ptr_converter.hpp
	luabind/typeid.hpp
	luabind/types.hpp	
	luabind/version.hpp
	luabind/weak_ref.hpp
	luabind/wrapper_base.hpp
	CACHE INTERNAL "LUABIND_INC_API" FORCE
)
source_group("include\\api"
	FILES ${LUABIND_INC_API})

set(LUABIND_INC_DETAIL
	luabind/detail/call.hpp
	luabind/detail/call_function.hpp
	luabind/detail/call_member.hpp
	luabind/detail/call_shared.hpp
	luabind/detail/call_traits.hpp
	luabind/detail/class_registry.hpp
	luabind/detail/class_rep.hpp
	luabind/detail/constructor.hpp
	luabind/detail/debug.hpp
	luabind/detail/decorate_type.hpp
	luabind/detail/enum_maker.hpp
	luabind/detail/format_signature.hpp
	luabind/detail/garbage_collector.hpp
	luabind/detail/inheritance.hpp
	luabind/detail/instance_holder.hpp
	luabind/detail/make_instance.hpp
	luabind/detail/meta.hpp
	luabind/detail/object.hpp
	luabind/detail/object_rep.hpp
	luabind/detail/operator_id.hpp
	luabind/detail/other.hpp
	luabind/detail/pcall.hpp
	luabind/detail/policy.hpp
	luabind/detail/primitives.hpp
	luabind/detail/property.hpp
	luabind/detail/stack_utils.hpp
	luabind/detail/type_traits.hpp
	CACHE INTERNAL "LUABIND_INC_DETAIL" FORCE
)
source_group("include\\detail"
	FILES ${LUABIND_INC_DETAIL})

set(LUABIND_INC_CONV_POLICIES
	luabind/detail/conversion_policies/conversion_base.hpp
	luabind/detail/conversion_policies/conversion_policies.hpp
	luabind/detail/conversion_policies/enum_converter.hpp
	luabind/detail/conversion_policies/function_converter.hpp
	luabind/detail/conversion_policies/lua_proxy_converter.hpp
	luabind/detail/conversion_policies/native_converter.hpp
	luabind/detail/conversion_policies/pointer_converter.hpp
	luabind/detail/conversion_policies/reference_converter.hpp
	luabind/detail/conversion_policies/value_converter.hpp
	CACHE INTERNAL "LUABIND_INC_CONV_POLICIES" FORCE
)
source_group("include\\detail\\conversion_policies"
	FILES ${LUABIND_INC_CONV_POLICIES})

set(LUABIND_INC_USER_POLICIES
	luabind/adopt_policy.hpp
	luabind/container_policy.hpp
	luabind/copy_policy.hpp
	luabind/dependency_policy.hpp
	luabind/discard_result_policy.hpp
	luabind/iterator_policy.hpp
	luabind/no_dependency.hpp
	luabind/out_value_policy.hpp
	luabind/raw_policy.hpp
	luabind/return_reference_to_policy.hpp
	luabind/yield_policy.hpp
	CACHE INTERNAL "LUABIND_INC_USER_POLICIES" FORCE
)
source_group("include\\api\\user_policies"
	FILES ${LUABIND_INC_USER_POLICIES})

set(LUABIND_SRC
	src/class.cpp
	src/class_info.cpp
	src/class_registry.cpp
	src/class_rep.cpp
	src/create_class.cpp
	src/error.cpp
	src/exception_handler.cpp
	src/function.cpp
	src/function_introspection.cpp
	src/inheritance.cpp
	src/lua_extensions.cpp
	src/memory.cpp
	src/nil_conversion.cpp
	src/object_rep.cpp
	src/open.cpp
	src/operator.cpp
	src/pcall.cpp
	src/scope.cpp
	src/set_package_preload.cpp
	src/stack_content_by_name.cpp
	src/weak_ref.cpp
	src/wrapper_base.cpp
)
source_group("src" FILES
	${LUABIND_SRC}
)

if(LUABIND_BUILD_SHARED)
	add_library(luabind SHARED)
else()
	add_library(luabind STATIC)
endif()

set_target_properties(luabind PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS OFF
	CXX_VISIBILITY_PRESET hidden
)
if(MSVC)
	set_property(TARGET luabind APPEND PROPERTY CMAKE_CXX_FLAGS "/wd4251")
endif()

if(LUABIND_NUMBER_TO_STRING_CONVERSION)
	add_compile_definitions(LUABIND_NUMBER_TO_STRING_CONVERSION)
endif()

if(LUABIND_CLASS_IN_NAMESPACE)
	add_compile_definitions(LUABIND_CLASS_IN_NAMESPACE)
endif()

if(LUABIND_ALLOW_MISSING_ARGUMENTS)
	add_compile_definitions(LUABIND_ALLOW_MISSING_ARGUMENTS)
endif()

if(LUABIND_NO_EXCEPTIONS)
	add_compile_definitions(LUABIND_NO_EXCEPTIONS)
	target_compile_definitions(luabind PUBLIC LUABIND_NO_EXCEPTIONS)
endif()

if(LUABIND_CUSTOM_ALLOCATOR)
	add_compile_definitions(LUABIND_CUSTOM_ALLOCATOR)
	target_compile_definitions(luabind PUBLIC LUABIND_CUSTOM_ALLOCATOR)
endif()

if(LUABIND_BUILD_SHARED)
	add_compile_definitions(LUABIND_DYNAMIC_LINK)
	target_compile_definitions(luabind PUBLIC LUABIND_DYNAMIC_LINK)
	target_compile_definitions(luabind PRIVATE LUABIND_BUILDING=1)
endif()

target_include_directories(luabind PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(luabind PRIVATE
	${LUABIND_INC_API}
	${LUABIND_INC_DETAIL}
	${LUABIND_INC_CONV_POLICIES}
	${LUABIND_INC_USER_POLICIES}
	${LUABIND_SRC}
)

target_link_libraries(luabind PUBLIC luajit)
