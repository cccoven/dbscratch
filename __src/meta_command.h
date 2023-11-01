#ifndef DBSCRATCH_META_COMMAND_H
#define DBSCRATCH_META_COMMAND_H

#include <string>
#include <memory>
#include <unistd.h>

#include "table.h"

enum MetaCommandResult {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND,
};

template<typename T>
class MetaCommand {
public:
    MetaCommand() = default;

    MetaCommandResult execute(const std::string &input, std::shared_ptr<Table<T>> &table);
};

template<typename T>
MetaCommandResult MetaCommand<T>::execute(const std::string &input, std::shared_ptr<Table<T>> &table) {
    if (input == ".exit") {
        // flush
        // std::shared_ptr<Pager<T>> pager = table->pager;
        // uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;
        //
        // for (uint32_t i = 0; i < num_full_pages; i++) {
        //     Page<T> *page = pager->pages.at(i);
        //     if (page == nullptr) {
        //         continue;
        //     }
        //
        //     pager->flush(i, PAGE_SIZE);
        // }
        //
        // uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
        // if (num_additional_rows > 0) {
        //     uint32_t page_num = num_full_pages;
        //     if (pager->pages[page_num] != nullptr) {
        //         pager->flush(page_num, num_additional_rows * ROW_SIZE);
        //     }
        // }
        //
        // close(pager->fd);

        std::exit(EXIT_SUCCESS);
    }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}

#endif // DBSCRATCH_META_COMMAND_H
