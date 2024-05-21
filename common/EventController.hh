// MIT License
//
// Copyright (c) 2024 Piotr Pszczółkowski
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
//
// Created by Piotr Pszczółkowski on 24.04.2024.
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include "Event.hh"
#include <QHash>
#include <QSet>
#include <QList>
#include <QApplication>
#include <mutex>

class EventController : public QObject {
    using EventStore = QHash<int, QSet<QObject*>>;
    std::mutex mutex_;
    EventStore store_{};
public:
    static EventController& instance() noexcept {
        static EventController controller;
        return controller;
    }

    /// no copy, no move
    EventController(EventController const&) = delete;
    EventController(EventController&&) = delete;
    EventController& operator=(EventController const&) = delete;
    EventController& operator=(EventController&&) = delete;
    ~EventController() override = default;

    /// Add a subscriber that is interested in receiving events with the given IDs
    /// \param subscriber - subscriber to append,
    /// \param ids - event IDs.
    template<typename... T> void append(QObject* subscriber, T... ids) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);

        auto add = [&](QObject* const s, int const id) {
            auto const exists = store_.contains(id) and store_.value(id).contains(subscriber);
            if (not exists)
                store_[id] << s;
        };

        // iterate over ids
        (..., add(subscriber, ids));
    }

    /// The specified subscriber no longer wants to follow the events.
    /// \param subscriber - subscriber to remove.
    void remove(QObject* const subscriber) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);

        auto const ids = store_.keys();
        for (int id : ids)
            store_[id].remove(subscriber);
        store_.squeeze();
    }

    /// Dispatch of an event with the given ID and the given arguments.
    /// \param id - events ID,
    /// \param args - arguments of the event.
    template<typename... T>
    void send(int const id, T... args) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);

        if (store_.contains(id)) {
            auto const& subsribers = store_[id];
            for (auto const& receiver : subsribers)
                QApplication::postEvent(receiver, new Event(id, args...));
        }
    }

private:
    EventController();
};
