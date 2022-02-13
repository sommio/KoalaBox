#include "util.hpp"
#include "../logger/logger.hpp"

namespace koalabox::util {

    [[maybe_unused]]
    void error_box(String title, String message) { // NOLINT(performance-unnecessary-value-param)
        ::MessageBoxW(
            nullptr,
            to_wstring(message).c_str(),
            to_wstring(title).c_str(),
            MB_OK | MB_ICONERROR
        );
    }

    [[maybe_unused]]
    Path get_module_dir(HMODULE& handle) {
        auto file_name = win_util::get_module_file_name(handle);

        auto module_path = Path(file_name);

        return module_path.parent_path();
    }

    [[maybe_unused]]
    bool is_64_bit() {
#ifdef _WIN64
        return true;
#else
        return false;
#endif
    }

    [[maybe_unused]]
    void panic(String title, String message) {
        // Add windows last error to title and message, if necessary.
        auto last_error = ::GetLastError();
        if (last_error != 0) {
            message = fmt::format(
                "{}\n———————— Windows Last Error ————————\nCode: {}\nMessage: {}",
                message,
                last_error,
                win_util::format_message(last_error)
            );
        }

        logger::critical("{}", message);

        error_box(std::move(title), message);

        exit(static_cast<int>(last_error));
    }

    [[maybe_unused]]
    bool strings_are_equal(const String& string1, const String& string2) {
        return _stricmp(string1.c_str(), string2.c_str()) == 0;
    }

    [[maybe_unused]]
    String to_string(const WideString& wstr) {
        if (wstr.empty()) {
            return {};
        }
        int size_needed = WideCharToMultiByte(
            CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr
        );
        String string(size_needed, 0);
        WideCharToMultiByte(
            CP_UTF8, 0, &wstr[0], (int) wstr.size(), &string[0], size_needed, nullptr, nullptr
        );
        return string;
    }


    [[maybe_unused]]
    WideString to_wstring(const String& str) {
        if (str.empty()) {
            return {};
        }
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int) str.size(), nullptr, 0);
        WideString wstring(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int) str.size(), &wstring[0], size_needed);
        return wstring;
    }


    [[maybe_unused]]
    bool is_hook_mode(
        HMODULE module,
        const String orig_module_name // NOLINT(performance-unnecessary-value-param)
    ) {
        const auto module_path = win_util::get_module_file_name(module);
        const auto self_name = Path(module_path).filename().string();

        return not util::strings_are_equal(self_name, orig_module_name + ".dll");
    }
}
