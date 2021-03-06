
// MIT License
//
// Copyright (c) 2019, 2020 degski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <Windows.h>

#include <cstdlib>

#include <atomic>
#include <filesystem>
#include <string>
#include <thread>

namespace fs = std::filesystem;

[[nodiscard]] fs::path appDataPath ( std::string && name_ ) {
    char * value;
    std::size_t len;
    _dupenv_s ( &value, &len, "USERPROFILE" );
    fs::path return_value ( std::string ( value ) + std::string ( "\\AppData\\Roaming\\" + name_ ) );
    fs::create_directory ( return_value ); // No error if directory exists.
    return return_value;
}

fs::path app_data_path_          = appDataPath ( "Mado" );
fs::path const & g_app_data_path = app_data_path_;

[[nodiscard]] fs::path getExePath ( ) noexcept {
    TCHAR exename[ 1024 ];
    GetModuleFileName ( NULL, exename, 1024 );
    return fs::path ( exename ).parent_path ( );
}

fs::path app_path_          = getExePath ( );
fs::path const & g_app_path = app_path_;
