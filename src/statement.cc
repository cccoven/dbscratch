#include "statement.h"
#include "row.h"
#include "cursor.h"

Statement::Statement() {
    row_to_insert = std::make_shared<Row>();
}

Statement::~Statement() {}

bool Statement::beginWith(const std::string &input, const std::string &prefix) {
    return input.compare(0, prefix.length(), prefix) == 0;
}

PrepareResult Statement::prepareInsert(const std::string &input) {
    std::vector<std::string> columns = {"id", "username", "email"};
    std::istringstream assigned_args(input.substr(7));
    std::vector<std::string> values;
    std::string value;

    while (std::getline(assigned_args, value, ' ')) {
        values.push_back(value);
    }

    if (values.size() != columns.size()) {
        return PREPARE_SYNTAX_ERROR;
    }

    if (values.at(0).at(0) == '-') {
        return PREPARE_NEGATIVE_ID;
    }

    std::shared_ptr<User> user = std::make_shared<User>();
    user->id = std::stoul(values.at(0));
    std::string username = values.at(1);
    std::string email = values.at(2);
    if (user->id == 0 || username.empty() || email.empty()) {
        return PREPARE_SYNTAX_ERROR;
    }
    if (username.length() > User::USERNAME_SIZE || email.length() > User::EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    std::strcpy(user->username, username.c_str());
    std::strcpy(user->email, email.c_str());

    row_to_insert->setData(user->serialize()); 
    row_to_insert->setSize(user->size());

    return PREPARE_SUCCESS;
}

PrepareResult Statement::prepareStatement(const std::string &input) {
    if (beginWith(input, "insert")) {
        type = STATEMENT_INSERT;
        return prepareInsert(input);
    }
    if (beginWith(input, "select")) {
        type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult Statement::execute(std::shared_ptr<Table> &table) {
    switch (type) {
        case STATEMENT_INSERT:
            return executeInsert(table);
        case STATEMENT_SELECT:
            return executeSelect(table);
    }

    return EXECUTE_SUCCESS;
}

ExecuteResult Statement::executeInsert(std::shared_ptr<Table> &table) {
    // if (table->num_rows >= TABLE_MAX_ROWS) {
    //     return EXECUTE_TABLE_FULL;
    // }
    
    table->insert(row_to_insert);
    table->num_rows += 1;

    return EXECUTE_SUCCESS;
}

ExecuteResult Statement::executeSelect(std::shared_ptr<Table> &table) {
    if (table->num_rows == 0) {
        return EXECUTE_SUCCESS;
    }

    // Cursor version
    std::shared_ptr<Cursor> cursor = std::make_shared<Cursor>(table, TABLE_START);
    while (!(cursor->end_of_table)) {
        std::shared_ptr<Row> row = cursor->value();
        User user{};
        user.deserialize(row->getData());
        std::cout
                << "("
                << "id: " << user.id
                << " username: " << user.username
                << " email: " << user.email
                << ")"
                << std::endl;
        cursor->advance();
    }

    return EXECUTE_SUCCESS;

    // uint32_t rows_per_page = table->pager->PAGE_SIZE / table->pager->row_size;
    // uint32_t total_pages = (table->num_rows + rows_per_page - 1) / rows_per_page;
    //
    // for (uint32_t i = 0; i < total_pages; i++) {
    //     std::shared_ptr<Page> page = table->pager->getPage(i);
    //      for (const std::shared_ptr<Row> &row: page->rows) {
    //         User user{};
    //         user.deserialize(row->getData());
    //         std::cout
    //             << "("
    //             << "id: " << user.id
    //             << " username: " << user.username
    //             << " email: " << user.email
    //             << ")"
    //             << std::endl;
    //     }
    // }

    return EXECUTE_SUCCESS;
}
