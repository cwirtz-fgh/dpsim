# cmake/FindFilesystem.cmake
include(CheckCXXSourceCompiles)
# Try to compile a simple filesystem snippet
check_cxx_source_compiles("
    #include <filesystem>
    int main() {
        std::filesystem::path p = \"test\";
        return 0;
    }
" HAS_STD_FILESYSTEM)

if (HAS_STD_FILESYSTEM)
    set(FILESYSTEM_LIBRARY "")
else()
    if (MSVC)
        # MSVC always has <filesystem> in the standard library (since VS2019)
        set(FILESYSTEM_LIBRARY "")
    else()
        # GCC/Clang < 9 may require explicit linking
        set(FILESYSTEM_LIBRARY stdc++fs)
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Filesystem
    DEFAULT_MSG
)

mark_as_advanced(FILESYSTEM_LIBRARY)