// This file is distributed under the BSD License.
// See "license.txt" for details.
// Copyright 2009-2012, Jonathan Turner (jonathan@emptycrate.com)
// Copyright 2009-2017, Jason Turner (jason@emptycrate.com)
// http://www.chaiscript.com

// This file is used in MomoEngine Project by @MomoDev built for Windows
// It was modified to exclude windows.h header file from ChaiScript library
// All WIN API constants and functions are exported inside WinAPI namespace
// Note that MomoEngine project only supports x64 for Windows
// Correct work of this file is not guaranteed for any other Windows systems
// If you find any bugs or mistakes, please contact me, @MomoDev, NOT Jason Turner

#ifndef CHAISCRIPT_WINDOWS_HPP_
#define CHAISCRIPT_WINDOWS_HPP_

#include <string>

#ifdef CHAISCRIPT_WINDOWS
#define VC_EXTRA_LEAN
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

namespace WinAPI
{
    using DWORD = unsigned long;
    using LPCVOID = const void*;
    using LPSTR = char*;
    using LPCSTR = const char*;
    using LPWSTR = wchar_t*;
    using va_list = char*;
    using HLOCAL = void*;
    using BOOL = int;
    using FARPROC = int(_stdcall*)();

    struct HMODULE__ { int unused; };
    using HMODULE = HMODULE__*;

    extern "C"
    {
        __declspec(dllimport) WinAPI::DWORD _stdcall GetLastError(void);

        __declspec(dllimport) WinAPI::HMODULE _stdcall LoadLibraryA(WinAPI::LPCSTR lpLibFileName);

        __declspec(dllimport) WinAPI::BOOL _stdcall FreeLibrary(WinAPI::HMODULE hLibModule);

        __declspec(dllimport) WinAPI::HLOCAL _stdcall LocalFree(WinAPI::HLOCAL hMem);

        __declspec(dllimport) WinAPI::FARPROC _stdcall GetProcAddress(
            WinAPI::HMODULE hModule,
            WinAPI::LPCSTR lpProcName
        );

        __declspec(dllimport) WinAPI::DWORD _stdcall FormatMessageA(
            WinAPI::DWORD dwFlags,
            WinAPI::LPCVOID lpSource,
            WinAPI::DWORD dwMessageId,
            WinAPI::DWORD dwLanguageId,
            WinAPI::LPSTR lpBuffer,
            WinAPI::DWORD nSize,
            WinAPI::va_list* Arguments
        );

        __declspec(dllimport) WinAPI::DWORD _stdcall FormatMessageW(
            WinAPI::DWORD dwFlags,
            WinAPI::LPCVOID lpSource,
            WinAPI::DWORD dwMessageId,
            WinAPI::DWORD dwLanguageId,
            WinAPI::LPWSTR lpBuffer,
            WinAPI::DWORD nSize,
            WinAPI::va_list* Arguments
        );
    }

    enum Consts
    {
        FORMAT_MESSAGE_ALLOCATE_BUFFER = 0x00000100,
        FORMAT_MESSAGE_FROM_SYSTEM = 0x00001000,
        FORMAT_MESSAGE_IGNORE_INSERTS = 0x00000200,
        LANG_NEUTRAL = 0x00,
        SUBLANG_DEFAULT = 0x01,
    };

    template<typename T>
    constexpr WinAPI::DWORD MAKELANGID(T p, T s)
    {
        return ((WinAPI::DWORD)s << (WinAPI::DWORD)10) | (WinAPI::DWORD)p;
    }
}
#endif

namespace chaiscript
{
    namespace detail
    {
        struct Loadable_Module
        {
            template<typename T>
            static std::wstring to_wstring(const T& t_str)
            {
                return std::wstring(t_str.begin(), t_str.end());
            }

            template<typename T>
            static std::string to_string(const T& t_str)
            {
                return std::string(t_str.begin(), t_str.end());
            }

#if defined(_UNICODE) || defined(UNICODE)
            template<typename T>
            static std::wstring to_proper_string(const T& t_str)
            {
                return to_wstring(t_str);
            }
#else
            template<typename T>
            static std::string to_proper_string(const T& t_str)
            {
                return to_string(t_str);
            }
#endif

            static std::string get_error_message(WinAPI::DWORD t_err)
            {

#if defined(_UNICODE) || defined(UNICODE)
                typedef WinAPI::LPWSTR StringType;
                std::wstring retval = L"Unknown Error";
                StringType lpMsgBuf = nullptr;

                if (WinAPI::FormatMessageW(
                    WinAPI::Consts::FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    WinAPI::Consts::FORMAT_MESSAGE_FROM_SYSTEM |
                    WinAPI::Consts::FORMAT_MESSAGE_IGNORE_INSERTS,
                    nullptr,
                    t_err,
                    WinAPI::MAKELANGID(WinAPI::Consts::LANG_NEUTRAL, WinAPI::Consts::SUBLANG_DEFAULT),
                    reinterpret_cast<StringType>(&lpMsgBuf),
                    0, nullptr) != 0 && lpMsgBuf)
                {
                    retval = lpMsgBuf;
                    LocalFree(lpMsgBuf);
                }

                return to_string(retval);
#else
                typedef WinAPI::LPSTR StringType;
                std::string retval = "Unknown Error";
                StringType lpMsgBuf = nullptr;

                if (WinAPI::FormatMessageA(
                    WinAPI::Consts::FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    WinAPI::Consts::FORMAT_MESSAGE_FROM_SYSTEM |
                    WinAPI::Consts::FORMAT_MESSAGE_IGNORE_INSERTS,
                    nullptr,
                    t_err,
                    WinAPI::MAKELANGID(WinAPI::Consts::LANG_NEUTRAL, WinAPI::Consts::SUBLANG_DEFAULT),
                    reinterpret_cast<StringType>(&lpMsgBuf),
                    0, nullptr) != 0 && lpMsgBuf)
                {
                    retval = lpMsgBuf;
                    WinAPI::LocalFree(lpMsgBuf);
                }

                return to_string(retval);
#endif
            }

            struct DLModule
            {
                explicit DLModule(const std::string& t_filename)
                    : m_data(WinAPI::LoadLibraryA(to_proper_string(t_filename).c_str()))
                {
                    if (!m_data)
                    {
                        throw chaiscript::exception::load_module_error(get_error_message(WinAPI::GetLastError()));
                    }
                }

                DLModule(DLModule&&) = default;
                DLModule& operator=(DLModule&&) = default;
                DLModule(const DLModule&) = delete;
                DLModule& operator=(const DLModule&) = delete;

                ~DLModule()
                {
                    WinAPI::FreeLibrary(m_data);
                }

                WinAPI::HMODULE m_data;
            };

            template<typename T>
            struct DLSym
            {
                DLSym(DLModule& t_mod, const std::string& t_symbol)
                    : m_symbol(reinterpret_cast<T>(WinAPI::GetProcAddress(t_mod.m_data, t_symbol.c_str())))
                {
                    if (!m_symbol)
                    {
                        throw chaiscript::exception::load_module_error(get_error_message(WinAPI::GetLastError()));
                    }
                }

                T m_symbol;
            };

            Loadable_Module(const std::string& t_module_name, const std::string& t_filename)
                : m_dlmodule(t_filename), m_func(m_dlmodule, "create_chaiscript_module_" + t_module_name),
                m_moduleptr(m_func.m_symbol())
            {
            }

            DLModule m_dlmodule;
            DLSym<Create_Module_Func> m_func;
            ModulePtr m_moduleptr;
        };
    }
}
#endif 