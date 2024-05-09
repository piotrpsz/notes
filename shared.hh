//
// Created by piotr on 08.04.24.
//
#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <vector>
#include <string>
#include <string_view>
#include <span>
#include <random>
#include <optional>
#include <charconv>
#include <fmt/core.h>
#include <range/v3/all.hpp>
#include <qglobal.h>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;
using i64 = int64_t;
using uint = unsigned int;
using f32 = float;
using f64 = double;

using qi64 = qint64;
using isize = qsizetype;
using qstr = QString;
using qvar = QVariant;


namespace fs = std::filesystem;

namespace shared {
    namespace rng = ranges;
    namespace vws = rng::views;

    /// Convert span of bytes to string.
    static inline std::string span2str(std::span<u8> data) noexcept {
        return data
               | vws::transform([](u8 const c) { return static_cast<char>(c); })
               | rng::to<std::string>();
    }

    static inline std::string span2str(std::span<u8> data, int const n) noexcept {
        return data
               | vws::take(n)
               | vws::transform([](u8 const c) { return static_cast<char>(c); })
               | rng::to<std::string>();
    }

    static inline bool is_not_spc(char const c) noexcept {
        return !std::isspace(c);
    }

    /// Remove leading whitespaces (left).
    static inline std::string trim_left(std::string s) noexcept {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), is_not_spc));
        return s;
    }

    /// Removing trailing whitespaces (right).
    static inline std::string trim_right(std::string s) noexcept {
        s.erase(std::find_if(s.rbegin(), s.rend(), is_not_spc).base(), s.end());
        return s;
    }

    /// Remove leading and trailing whitespaces (on both sides - left and right).
    static inline std::string trim(std::string s) noexcept {
        return trim_left(trim_right(std::move(s)));
    }

    static inline std::vector<std::string> split(std::string_view sv, char const delimiter) noexcept {
        // It is better to count delimiters than to reallocate the vector later.
        auto const n = std::accumulate(
                sv.begin(),
                sv.end(),
                ssize_t{0},
                [delimiter](ssize_t const count, char const c) {
                    return c == delimiter ? (count + 1) : count;
                }
        );

        // Allocation of a size-matched vector
        std::vector<std::string> tokens{};
        tokens.reserve(n + 1);

        // Save strings found between delimiters.
        auto pos = sv.find(delimiter);
        while (pos != std::string_view::npos) {
            tokens.push_back(trim({sv.data(), sv.data() + pos}));
            sv.remove_prefix(pos + 1);
            pos = sv.find(delimiter);
        }

        // Save the string from the last delimiter to the end of 'sv'.
        if (sv.data() not_eq sv.end())
            tokens.push_back(trim({sv.data(), sv.end()}));

        return std::move(tokens);
    }

    /// Join strings with 'delimiter'.
    /// \param data - source span of strings,
    /// \param delimiter - a character inserted between strings
    /// \param spacer - a character added after delimiter (space as default)
    /// \return connection result as string
    static inline std::string join(std::span<std::string> data, char const delimiter = ',', std::optional<char> const spacer = {}) noexcept {
        auto n = std::accumulate(
                data.begin(),
                data.end(),
                uint{0},
                [](uint const count, std::string const &str) {
                    return count + str.size();
                }
        );
        n += ((data.size() - 1) * 2);   // 2 := delimiter + one space after
        std::string buffer{};
        buffer.reserve(n);

        for (auto it = data.begin(); it not_eq std::prev(data.end()); ++it) {
            buffer += *it;
            buffer += delimiter;
            if (spacer)
                buffer += spacer.value();
        }
        buffer += *std::prev(data.end());

        return std::move(buffer);
    }

    /// Convert string to integer number (if possible).
    /// \param sv - source string (view) to convert,
    /// \param base - base of number represented by string
    /// \return integer if the conversion was successful
    static inline std::optional<int> to_int(std::string_view sv, int const base = 10) noexcept {
        int value{};
        auto [_, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value, base);

        // Success.
        if (ec == std::errc{})
            return value;
        // Error.
        if (ec == std::errc::invalid_argument)
            fmt::print(stderr, "This is not a number ({}).\n", sv);
        else if (ec == std::errc::result_out_of_range)
            fmt::print(stderr, "The number is to big ({}).\n", sv);
        return {};
    }

    /// Generate vector of random bytes.
    /// \param n - number of bytes to generate
    /// \return vector of generated bytes.
    static inline std::vector<u8> random_bytes(int const n) noexcept {
        std::random_device rd;
        std::array<int, std::mt19937::state_size> seed_data{};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        auto mtgen = std::mt19937{seq};
        auto ud = std::uniform_int_distribution{0, 255};

        return vws::iota(0, n)
               | vws::transform([&](u8 _) { return static_cast<u8>(ud(mtgen)); })
               | rng::to<std::vector>();
    }

    enum class BytesFormat {
        Dec, Hex
    };

    /// Generate string representation of vector of bytes.
    /// \param data - span with bytes
    /// \param format - how to convert values [Dec|Hex]
    /// \return string representation of bytes.
    static inline std::string bytes2str(std::span<u8> data, BytesFormat const format = BytesFormat::Hex) noexcept {
        if (format == BytesFormat::Dec) {
            auto components = data
                              | vws::transform([](u8 const c) { return fmt::format("{}", c); })
                              | rng::to<std::vector>();
            return join(components, ',');
        }
        auto components = data
                          | vws::transform([](u8 const c) { return fmt::format("0x{:02x}", c); })
                          | rng::to<std::vector>();
        return join(components, ',');
    }

    /// Generate vector(u8) from string(char)
    /// \param str - source string
    /// \return generated vector
    static inline std::vector<u8> str2vec(std::string_view str) noexcept {
        return str
               | vws::transform([](char const c) { return static_cast<u8>(c); })
               | rng::to<std::vector>();
    }

    /// Generate string(char) from vector(u8).
    /// \param vec - source vector
    /// \return generated string.
    static inline std::string vec2str(std::span<u8> vec) noexcept {
        return vec
               | vws::transform([](u8 const c) { return static_cast<char>(c); })
               | rng::to<std::string>();
    }
}

