//
// Created by piotr on 08.04.24.
//

/*------- include files:
-------------------------------------------------------------------*/
#include "stmt.hh"
#include "logger.hh"
#include "row.hh"
#include "value.hh"


/*------- forward declarations:
-------------------------------------------------------------------*/
Row fetch_row_data(sqlite3_stmt* stmt, int column_count) noexcept;
bool bind2stmt(sqlite3_stmt* stmt, std::vector<value_t> const& args) noexcept;
bool bind_at(sqlite3_stmt* stmt, int idx, value_t const& v) noexcept;

// When statement not finalized do it.
Stmt::~Stmt() {
    if (stmt_) {
        if (SQLITE_OK == sqlite3_finalize(stmt_)) {
            stmt_ = nullptr;
            return;
        }
        LOG_ERROR(db_);
    }
}

// Execute a query that returns no result.
bool Stmt::exec_without_result(query_t const& query) noexcept {
    if (query.valid())
        if (SQLITE_OK == sqlite3_prepare_v2(db_, query.query().c_str(), -1, &stmt_, nullptr))
            if (bind2stmt(stmt_, query.values()))
                if (SQLITE_DONE == sqlite3_step(stmt_))
                    if (SQLITE_OK == sqlite3_finalize(stmt_)) {
                        stmt_ = nullptr;
                        return true;
                    }

    LOG_ERROR(db_);
    return false;
}

// Execute a query that returns the result
std::optional<Result> Stmt::exec_with_result(query_t const& query) noexcept {
    Result result{};

    if (query.valid())
        if (SQLITE_OK == sqlite3_prepare_v2(db_, query.query().c_str(), -1, &stmt_, nullptr))
            if (bind2stmt(stmt_, query.values()))
                if (auto n = sqlite3_column_count(stmt_); n > 0) {
                    while (SQLITE_ROW == sqlite3_step(stmt_))
                        if (auto row = fetch_row_data(stmt_, n); not row.empty())
                            result.push_back(std::move(row));
                }

    if (SQLITE_DONE == sqlite3_errcode(db_))
        if (SQLITE_OK == sqlite3_finalize(stmt_)) {
            stmt_ = nullptr;
            return std::move(result);
        }

    LOG_ERROR(db_);
    return {};
}

//*******************************************************************
//*                                                                 *
//*                        P R I V A T E                            *
//*                                                                 *
//*******************************************************************

Row fetch_row_data(sqlite3_stmt* const stmt, int const column_count) noexcept {
    Row row{};

    for (auto i = 0; i < column_count; i++) {
        std::string name{sqlite3_column_name(stmt, i)};
        switch (sqlite3_column_type(stmt, i)) {
            case SQLITE_NULL:
                row.add(std::move(name));
                break;
            case SQLITE_INTEGER:
                row.add(std::move(name), sqlite3_column_int64(stmt, i));
                break;
            case SQLITE_FLOAT:
                row.add(std::move(name), sqlite3_column_double(stmt, i));
                break;
            case SQLITE_TEXT: {
                std::string text{reinterpret_cast<const char *>(sqlite3_column_text(stmt, i))};
                row.add(std::move(name), std::move(text));
                break;
            }
            case SQLITE_BLOB: {
                auto const ptr { reinterpret_cast<u8 const*>(sqlite3_column_blob(stmt, i))};
                auto const size{ sqlite3_column_bytes(stmt, i)};
                std::vector<u8> vec{ ptr, ptr + size };
                row.add(std::move(name), std::move(vec));
            }
        }
    }
    return row;
}

bool bind2stmt(sqlite3_stmt* const stmt, std::vector<value_t> const& args) noexcept {
    if (!args.empty()) {
        auto idx = 0;
        for (auto const& v: args)
            if (!bind_at(stmt, ++idx, v))
                return false;
    }
    return true;
}

bool bind_at(sqlite3_stmt* const stmt, int const idx, value_t const& v) noexcept {
    switch (v.index()) {
        case value_t::Monostate:
            return SQLITE_OK == sqlite3_bind_null(stmt, idx);
        case value_t::Integer:
            return SQLITE_OK == sqlite3_bind_int64(stmt, idx, v.int64());
        case value_t::Double:
            return SQLITE_OK == sqlite3_bind_double(stmt, idx, v.float64());
        case value_t::String: {
            auto const text{ v.str() };
            return SQLITE_OK == sqlite3_bind_text(stmt, idx, text.c_str(), -1, SQLITE_TRANSIENT); }
            case value_t::Vector: {
            auto const vec{ v.vec() };
            auto const n{ static_cast<int>(vec.size())};
            return SQLITE_OK == sqlite3_bind_blob(stmt, idx, vec.data(), n, SQLITE_TRANSIENT); }
        default:
            return false;
    }
}
