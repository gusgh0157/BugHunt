cmake_minimum_required(VERSION 3.21)
project(BugHunt LANGUAGES CXX)

# BugHunt C++판은 표준 C++20만으로 만든다. MSVC, Clang, GCC 어느 것으로 빌드해도
# 같은 랩이 같은 이유로 실패하고, 정답을 넣으면 통과해야 한다.
#   cmake --preset debug && cmake --build --preset debug      (Debug / Release / ASan 프리셋은 CMakePresets.json)
# [JA] BugHunt の C++ 版は標準 C++20 だけで作ります。MSVC、Clang、GCC のどれでビルドしても、
# [JA] 同じラボが同じ理由で失敗し、正解を入れれば合格しなければなりません。
# [JA]   cmake --preset debug && cmake --build --preset debug      (Debug / Release / ASan のプリセットは CMakePresets.json)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUGHUNT_ASAN "AddressSanitizer로 빌드한다 / AddressSanitizer でビルドする" OFF)
set(BUGHUNT_BIN_NAME "${CMAKE_BUILD_TYPE}" CACHE STRING "실행 파일을 둘 bin 아래 폴더 이름 / 実行ファイルを置く bin の下のフォルダー名 (Debug, Release, ASan)")

file(GLOB_RECURSE BUGHUNT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/BugHunt/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BugHunt/src/*.h")
add_executable(BugHunt ${BUGHUNT_SOURCES})

# 직렬화 랩이 쓰는 외부 라이브러리 (third_party, 버전 고정). 랩 코드가 아니라서 경고는 낮춘다.
#   nlohmann/json 3.11.3: 헤더 전용,  pugixml 1.14: 소스 한 개를 정적 라이브러리로
# [JA] シリアライズのラボが使う外部ライブラリ (third_party、バージョン固定)。ラボのコードではないので警告は下げます。
# [JA]   nlohmann/json 3.11.3: ヘッダーのみ、  pugixml 1.14: ソース 1 つを静的ライブラリとして
add_library(bughunt_pugixml STATIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/pugixml/src/pugixml.cpp")
target_include_directories(bughunt_pugixml SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/pugixml/src")
if(MSVC)
    target_compile_options(bughunt_pugixml PRIVATE /W0 /utf-8)
else()
    target_compile_options(bughunt_pugixml PRIVATE -w)
endif()
target_include_directories(BugHunt SYSTEM PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/nlohmann_json/include")
target_link_libraries(BugHunt PRIVATE bughunt_pugixml)

set_target_properties(BugHunt PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${BUGHUNT_BIN_NAME}")
foreach(config IN ITEMS DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(BugHunt PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_CURRENT_SOURCE_DIR}/bin/${BUGHUNT_BIN_NAME}")
endforeach()

if(MSVC)
    # MSVC와 clang-cl
    # [JA] MSVC と clang-cl
    target_compile_options(BugHunt PRIVATE /W4 /permissive- /utf-8 /Zc:__cplusplus /EHsc)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # 위의 SYSTEM include 때문에 CMake가 /external:W0을 붙이면, 표준 헤더 안 템플릿에서 나는 경고도 숨는다.
        # 랩 코드가 일으킨 템플릿 경고(랩 16의 C5205: std::default_delete 안의 delete)는 보이게 한다.
        # [JA] 上の SYSTEM include のせいで CMake が /external:W0 を付けると、標準ヘッダー内のテンプレートで出る警告も隠れます。
        # [JA] ラボのコードが起こしたテンプレート警告(ラボ 16 の C5205: std::default_delete 内の delete)は見えるようにします。
        target_compile_options(BugHunt PRIVATE /external:templates-)
    endif()
else()
    target_compile_options(BugHunt PRIVATE -Wall -Wextra)
    # Debug에서는 표준 라이브러리의 범위 검사를 켠다. MSVC Debug STL의 어설션과 같은 역할이다.
    # 범위 밖 operator[](랩 80 등)가 조용히 틀린 값을 내는 대신 어설션으로 멈춘다.
    #   libstdc++(GCC): _GLIBCXX_ASSERTIONS,  libc++(macOS Clang): 하드닝 모드 debug
    # [JA] Debug では標準ライブラリの範囲チェックを有効にします。MSVC の Debug STL のアサーションと同じ役割です。
    # [JA] 範囲外の operator[](ラボ 80 など)が黙って間違った値を返す代わりに、アサーションで止まります。
    # [JA]   libstdc++(GCC): _GLIBCXX_ASSERTIONS、  libc++(macOS Clang): ハードニングモード debug
    target_compile_definitions(BugHunt PRIVATE
        $<$<CONFIG:Debug>:_GLIBCXX_ASSERTIONS _LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG>)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC는 생성자가 시작될 때 객체 메모리를 "알 수 없는 값"으로 보고 그 전의 저장을 지운다(-flifetime-dse).
        # 초기화하지 않은 멤버를 다루는 랩(12, 40, B-4)의 결과가 최적화에 따라 흔들리지 않도록 끈다.
        # #pragma region은 접기용 표시라 모르는 컴파일러는 무시한다. 그 경고만 끈다.
        # [JA] GCC はコンストラクターの開始時にオブジェクトのメモリを「不定値」とみなし、それ以前の書き込みを消します(-flifetime-dse)。
        # [JA] 初期化していないメンバーを扱うラボ(12、40、B-4)の結果が最適化によって揺れないように、これを無効にします。
        # [JA] #pragma region は折りたたみ用の印なので、知らないコンパイラーは無視します。その警告だけを消します。
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
    # pugixml도 같은 설정으로 컴파일해야 MSVC STL의 ASan 주석(annotate_string 등) 불일치 링크 오류가 안 난다
    # [JA] pugixml も同じ設定でコンパイルしないと、MSVC STL の ASan アノテーション(annotate_string など)の不一致でリンクエラーになります
    if(MSVC AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # MSVC: ASan과 /RTC는 함께 쓸 수 없다
        # [JA] MSVC: ASan と /RTC は一緒に使えません
        string(REGEX REPLACE "/RTC[1csu]+" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
        target_compile_options(BugHunt PRIVATE /fsanitize=address)
        target_compile_options(bughunt_pugixml PRIVATE /fsanitize=address)
    elseif(MSVC)
        # clang-cl
        string(REGEX REPLACE "/RTC[1csu]+" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
        target_compile_options(BugHunt PRIVATE -fsanitize=address)
        target_compile_options(bughunt_pugixml PRIVATE -fsanitize=address)
        target_link_options(BugHunt PRIVATE -fsanitize=address)
    else()
        target_compile_options(BugHunt PRIVATE -fsanitize=address -fno-omit-frame-pointer)
        target_compile_options(bughunt_pugixml PRIVATE -fsanitize=address -fno-omit-frame-pointer)
        target_link_options(BugHunt PRIVATE -fsanitize=address)
    endif()

    if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Windows의 Clang ASan은 Debug CRT를 지원하지 않는다
        # [JA] Windows の Clang ASan は Debug CRT をサポートしていません
        set_property(TARGET BugHunt bughunt_pugixml PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    endif()

    # 명령줄에서 바로 실행할 수 있도록 Windows의 ASan 런타임 DLL을 exe 옆에 복사한다.
    # [JA] コマンドラインからそのまま実行できるように、Windows の ASan ランタイム DLL を exe の隣にコピーします。
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

# 빌드할 때마다 소스 검사: 한글 문자열이 일본어와 짝(i18n::L("ko", "ja") 또는 { "ko", "ja" }) 없이 있거나,
# 한글 주석에 // [JA] 일본어 줄이 없으면 빌드 실패.
# (src/i18n/source_check.cpp) 검사가 통과해야 표시 파일이 생기므로, 실패하면 다음 빌드에서 다시 검사한다.
# 만든 exe를 이 컴퓨터에서 실행할 수 없는 경우(교차 컴파일, 다른 아키텍처)에는 기본으로 끈다.
# [JA] ビルドのたびにソースを検査します: ハングルの文字列が日本語と対(i18n::L("ko", "ja") または { "ko", "ja" })になっていないか、
# [JA] ハングルのコメントに // [JA] の日本語の行がなければ、ビルドが失敗します。
# [JA] (src/i18n/source_check.cpp) 検査に通ったときだけ印のファイルができるので、失敗すると次のビルドでもう一度検査します。
# [JA] 作った exe をこのコンピューターで実行できない場合(クロスコンパイル、別のアーキテクチャー)は、既定でオフにします。
set(BUGHUNT_CHECK_I18N_DEFAULT ON)
if(CMAKE_CROSSCOMPILING)
    set(BUGHUNT_CHECK_I18N_DEFAULT OFF)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^(ARM|arm)" AND NOT CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(ARM|arm|aarch)")
    set(BUGHUNT_CHECK_I18N_DEFAULT OFF)
elseif(APPLE AND CMAKE_OSX_ARCHITECTURES AND NOT CMAKE_OSX_ARCHITECTURES STREQUAL CMAKE_HOST_SYSTEM_PROCESSOR)
    set(BUGHUNT_CHECK_I18N_DEFAULT OFF)
endif()
option(BUGHUNT_CHECK_I18N "빌드 뒤 한글 문자열·주석 짝 검사 / ビルド後のハングル文字列・コメントの対の検査 (BugHunt --check-i18n)" ${BUGHUNT_CHECK_I18N_DEFAULT})
if(BUGHUNT_CHECK_I18N)
    set(BUGHUNT_I18N_STAMP "${CMAKE_CURRENT_BINARY_DIR}/check_i18n.stamp")
    add_custom_command(OUTPUT "${BUGHUNT_I18N_STAMP}"
        COMMAND "$<TARGET_FILE:BugHunt>" --check-i18n "${CMAKE_CURRENT_SOURCE_DIR}/BugHunt/src"
        COMMAND ${CMAKE_COMMAND} -E touch "${BUGHUNT_I18N_STAMP}"
        DEPENDS BugHunt ${BUGHUNT_SOURCES}
        COMMENT "check-i18n"
        VERBATIM)
    add_custom_target(bughunt_check_i18n ALL DEPENDS "${BUGHUNT_I18N_STAMP}")
endif()
