// MIT License
//
// Copyright (c) 2023 Piotr Pszczółkowski
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
#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <sstream>
#include <date/date.h>
#include <date/tz.h>
#include <fmt/core.h>
#include <chrono>
//#include <fmt/chrono.h>

using i64 = int64_t;
using zoned_time_t = date::zoned_time<std::chrono::seconds>;

struct Date {
    int y{}, m{}, d{};
    bool operator==(Date const& rhs) const noexcept {
        return y == rhs.y && m == rhs.m && d == rhs.d;
    }
    bool operator!=(Date const& rhs) const noexcept {
        return !operator==(rhs);
    }
    bool operator<(Date const& rhs) const noexcept {
        if (y < rhs.y) return true;
        if (m < rhs.m) return true;
        if (d < rhs.d) return true;
        return false;
    }
    bool operator>(Date const& rhs) const noexcept {
        if (y > rhs.y) return true;
        if (m > rhs.m) return true;
        if (d > rhs.d) return true;
        return false;
    }

};
struct Time {
    int h{}, m{}, s{};
};

class DateTime final {
    date::time_zone const* zone = date::locate_zone("Europe/Warsaw");
    zoned_time_t tp_{};
public:
    /// Data-czas teraz (now).
    DateTime() {
        auto const now = std::chrono::system_clock::now();
        tp_ = date::make_zoned(zone, std::chrono::floor<std::chrono::seconds>(now));
    }
    /// Data-czas z timestampu (liczba sekund od początku epoki).
    /// \param timestamp - liczba sekund od początku epoki.
    explicit DateTime(i64 const timestamp) {
        auto const tp = std::chrono::system_clock::from_time_t(timestamp);
        tp_ = date::make_zoned(zone, std::chrono::floor<std::chrono::seconds>(tp));
    }
    explicit DateTime(zoned_time_t const tp) : tp_{tp} {}

    /// Data-czas z tekstu (np. 2023-10-23 11:06:21).
    /// \param str - string z datą i godziną
    explicit DateTime(std::string const& str) {
        std::stringstream in{str};
        date::local_time<std::chrono::seconds> tp;
        in >> date::parse("%F %T", tp);
//        date::from_stream(in, "%F %X", tp);
        tp_ = make_zoned(zone, tp);
    }

    /// Data-czas z komponentów.
    explicit DateTime(Date const dt, Time const tm)
            : tp_{from_components(dt, tm)}
    {}

    // Kopiowanie i przekazywanie - domyślne
    DateTime(DateTime const&) = default;
    DateTime& operator=(DateTime const&) = default;
    DateTime(DateTime&&) = default;
    DateTime& operator=(DateTime&&) = default;
    ~DateTime() = default;

    /// Sprawdza czy wskazany obiekt określa ten sam punkt w czasie.
    /// \return True jeśli punkty w czasie są takie same, False w przeciwnym przepadku.
    bool operator==(DateTime const &rhs) const noexcept {
        return timestamp() == rhs.timestamp();
    }
    /// Sprawdza czy wskazany obiekt określa inny punkt w czasie
    /// \return True jeśli punkty w czasie są różne, False w przeciwnym przypadku.
    bool operator!=(DateTime const &rhs) const noexcept {
        return !operator==(rhs);
    }
    /// Sprawdza czy wskazana data-czas (rhs) jest późniejsza.
    /// \return True jeśli rhs jest późniejszy, False w przeciwnym przypadku,
    bool operator<(DateTime const& rhs) const noexcept {
        return timestamp() < rhs.timestamp();
    }
    /// Sprawdza czy wskazana data-czas (rhs) jest wcześniejsza.
    /// \return True jeśli rhs jest wcześniejszy, False w przeciwnym przypadku.
    bool operator>(DateTime const& rhs) const noexcept {
        return timestamp() > rhs.timestamp();
    }

    /// Zwraca różnicę jako liczbę minut.
    [[nodiscard]] i64
    minutes_from(DateTime const& rhs) const noexcept {
        auto const a = date::floor<std::chrono::minutes>(tp_.get_sys_time());
        auto const b = date::floor<std::chrono::minutes>(rhs.tp_.get_sys_time());
        return (b - a).count();
    }
    /// Obliczenie timestampu (liczba sekund od początku epoki).
    /// \return timestamp
    [[nodiscard]] i64
    timestamp() const noexcept {
        auto const seconds = tp_.get_sys_time().time_since_epoch().count();
        return static_cast<i64>(seconds);
    }

    /// Zmiana czasu na podany.
    DateTime& set_time(Time const tm) noexcept {
        namespace chrono = std::chrono;
        auto t = date::floor<chrono::days>(tp_.get_local_time())
                 + chrono::hours(tm.h)
                 + chrono::minutes(tm.m)
                 + chrono::seconds(tm.s);
        tp_ = make_zoned(zone, t);
        return *this;
    }
    /// Wyzerowanie sekund z ewentualnym zaokrągleniem minut.
    DateTime& clear_seconds() noexcept {
        namespace chrono = std::chrono;
        auto const days = date::floor<chrono::days>(tp_.get_local_time());
        date::hh_mm_ss hms{tp_.get_local_time() - days};
        auto t = date::floor<chrono::days>(tp_.get_local_time())
                 + hms.hours()
                 + hms.minutes()
                 + chrono::seconds(hms.seconds().count() >= 30 ? 60 : 0);
        tp_ = make_zoned(zone, t);
        return *this;
    }
    /// Wyzerowanie czasu.
    DateTime& clear_time() noexcept {
        namespace chrono = std::chrono;
        auto t = date::floor<chrono::days>(tp_.get_local_time())
                 + chrono::hours(0)
                 + chrono::minutes(0)
                 + chrono::seconds(0);
        tp_ = make_zoned(zone, t);
        return *this;
    }
    /// Początek dnia dla daty.
    DateTime& beginning_day() noexcept {
        return clear_time();
    }
    /// Koniec dnia dla daty.
    DateTime& end_day() noexcept {
        return set_time({23,59,59});
    }
    /// Wyznaczenie składników daty (bez czasu).
    [[nodiscard]] Date
    date_components() const noexcept {
        namespace chrono = std::chrono;
        auto days = date::floor<chrono::days>(tp_.get_local_time());
        date::year_month_day ymd{days};
        auto const year = static_cast<int>(ymd.year());
        auto const month = static_cast<int>(static_cast<unsigned>(ymd.month()));
        auto const day = static_cast<int>(static_cast<unsigned>(ymd.day()));
        return {year, month, day};
    }
    /// Sprawdzenie czy to ten sam dzień.
    [[nodiscard]] bool
    is_same_day(DateTime const& rhs) const noexcept {
        return date_components() == rhs.date_components();
    }
    /// Wyznaczenie składników czasu (bez daty).
    [[nodiscard]] Time
    time_components() const noexcept {
        namespace chrono = std::chrono;
        auto const days = date::floor<chrono::days>(tp_.get_local_time());
        date::hh_mm_ss const hms{tp_.get_local_time() - days};
        auto hour = static_cast<int>(hms.hours().count());
        auto min = static_cast<int>(hms.minutes().count());
        auto sec = static_cast<int>(hms.seconds().count());
        return {hour, min, sec};
    }
    /// Wyznaczenie składników daty i czasu.
    [[nodiscard]] std::tuple<Date, Time>
    components() const noexcept {
        namespace chrono = std::chrono;
        auto const days = date::floor<chrono::days>(tp_.get_local_time());

        date::year_month_day const ymd{days};
        auto const year = static_cast<int>(ymd.year());
        auto const month = static_cast<int>(static_cast<unsigned>(ymd.month()));
        auto const day = static_cast<int>(static_cast<unsigned>(ymd.day()));
        Date const dt{year, month, day};

        date::hh_mm_ss const hms{tp_.get_local_time() - days};
        auto hour = static_cast<int>(hms.hours().count());
        auto min = static_cast<int>(hms.minutes().count());
        auto sec = static_cast<int>(hms.seconds().count());
        Time const tm{hour, min, sec};

        return {dt, tm};
    }

    /// Utworzenie nowej data-czasu przez dodanie wskazanej liczby dni.
    /// \param n - liczba dnie do dodania (może być ujemna)
    /// \return nowa data-czas
    [[nodiscard]] DateTime
    add_days(int const n) const noexcept {
        namespace chrono = std::chrono;
        auto const days = date::floor<chrono::days>(tp_.get_local_time());
        date::hh_mm_ss const hms{tp_.get_local_time() - days};
        auto const added = days + chrono::days(n);
        auto const secs = chrono::floor<chrono::seconds>(added)
                          + hms.hours()
                          + hms.minutes()
                          + hms.seconds();
        return DateTime(make_zoned(zone, secs));
    }

    [[nodiscard]] DateTime
    next_day() const noexcept {
        return add_days(1);
    }
    [[nodiscard]] DateTime
    prev_day() const noexcept {
        return add_days(-1);
    }
    [[nodiscard]] int
    week_day() const noexcept {
        namespace chrono = std::chrono;
        auto const wd = date::weekday(chrono::floor<chrono::days>(tp_.get_local_time()));
        return int(wd.iso_encoding());
    }
    [[nodiscard]] std::pair<DateTime, DateTime>
    week_range() const noexcept {
        auto const today_idx = week_day();
        return {add_days(-today_idx + 1), add_days(7 - today_idx)};
    }

    /// Data-czas w postaci tekstu (LOCAL)
    /// \return string z datą-czasem.
    [[nodiscard]] std::string
    str() const noexcept {
        std::stringstream ss{};
        ss << tp_.get_local_time();
        return ss.str();
    }
private:
    [[nodiscard]] zoned_time_t
    from_components(Date const dt, Time const tm) noexcept {
        namespace chrono = std::chrono;
        date::year_month_day const ymd = date::year(dt.y) / dt.m / dt.d;
        auto const days = static_cast<date::local_days>(ymd);
        auto t = days + chrono::hours(tm.h) + chrono::minutes(tm.m) + chrono::seconds(tm.s);
        return make_zoned(zone, t);
    }
};
