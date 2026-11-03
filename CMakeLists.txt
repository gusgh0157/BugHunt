cmake_minimum_required(VERSION 3.21)
project(BugHunt LANGUAGES CXX)

# BugHunt C++판은 표준 C++20만으로 만든다. MSVC, Clang, GCC 어느 것으로 빌드해도
# 같은 랩이 같은 이유로 실패하고, 정답을 넣으면 통과해야 한다.
#   cmake --preset debug && cmake --build --preset debug      (Debug / Release / ASan 프리셋은 CMakePresets.json)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUGHUNT_ASAN "AddressSanitizer로 빌드한다" OFF)
set(BUGHUNT_BIN_NAME "${CMAKE_BUILD_TYPE}" CACHE STRING "실행 파일을 둘 bin 아래 폴더 이름 (Debug, Release, ASan)")

file(GLOB_RECURSE BUGHUNT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/BugHunt/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BugHunt/src/*.h")
add_executable(BugHunt ${BUGHUNT_SOURCES})

set_target_properties(BugHunt PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${BUGHUNT_BIN_NAME}")
foreach(config IN ITEMS DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(BugHunt PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_CURRENT_SOURCE_DIR}/bin/${BUGHUNT_BIN_NAME}")
endforeach()

if(MSVC)
    # MSVC와 clang-cl
    target_compile_options(BugHunt PRIVATE /W4 /permissive- /utf-8 /Zc:__cplusplus /EHsc)
else()
    target_compile_options(BugHunt PRIVATE -Wall -Wextra)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC는 생성자가 시작될 때 객체 메모리를 "알 수 없는 값"으로 보고 그 전의 저장을 지운다(-flifetime-dse).
        # 초기화하지 않은 멤버를 다루는 랩(12, 40, B-4)의 결과가 최적화에 따라 흔들리지 않도록 끈다.
        # #pragma region은 접기용 표시라 모르는 컴파일러는 무시한다. 그 경고만 끈다.
        target_compile_options(BugHunt PRIVATE -fno-lifetime-dse -Wno-unknown-pragmas)
    endif()
endif()

if(WIN32)
    target_compile_definitions(BugHunt PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(BugHunt PRIVATE Threads::Threads)
endif()

if(BUGHUNT_ASAN)
    if(MSVC AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # MSVC: ASan과 /RTC는 함께 쓸 수 없다
        string(REGEX REPLACE "/RTC[1csu]+" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
        target_compile_options(BugHunt PRIVATE /fsanitize=address)
    elseif(MSVC)
        # clang-cl
        string(REGEX REPLACE "/RTC[1csu]+" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
        target_compile_options(BugHunt PRIVATE -fsanitize=address)
        target_link_options(BugHunt PRIVATE -fsanitize=address)
    else()
        target_compile_options(BugHunt PRIVATE -fsanitize=address -fno-omit-frame-pointer)
        target_link_options(BugHunt PRIVATE -fsanitize=address)
    endif()

    if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Windows의 Clang ASan은 Debug CRT를 지원하지 않는다
        set_property(TARGET BugHunt PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    endif()

    # 명령줄에서 바로 실행할 수 있도록 Windows의 ASan 런타임 DLL을 exe 옆에 복사한다.
    if(WIN32)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            execute_process(COMMAND "${CMAKE_CXX_COMPILER}" -print-resource-dir
                OUTPUT_VARIABLE BUGHUNT_CLANG_RESOURCE OUTPUT_STRIP_TRAILING_WHITESPACE)
            file(GLOB BUGHUNT_ASAN_DLLS "${BUGHUNT_CLANG_RESOURCE}/lib/windows/clang_rt.asan_dynamic-x86_64.dll")
        else()
            get_filename_component(BUGHUNT_CL_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
            file(GLOB BUGHUNT_ASAN_DLLS "${BUGHUNT_CL_DIR}/clang_rt.asan*_dynamic-x86_64.dll")
        endif()
        foreach(dll IN LISTS BUGHUNT_ASAN_DLLS)
            add_custom_command(TARGET BugHunt POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${dll}" "$<TARGET_FILE_DIR:BugHunt>")
        endforeach()
    endif()
endif()
