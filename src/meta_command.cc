#include "meta_command.h"

MetaCommandResult MetaCommand::execute(const std::string &input, std::shared_ptr<Table> &table) {
    if (input == ".exit") {
        // flush
        // std::shared_ptr<Pager> pager = table->pager;
        // uint32_t num_full_pages = table->num_rows / pager->getRowsPerPage();
        
        // for (uint32_t i = 0; i < num_full_pages; i++) {
        //     std::shared_ptr<Page> page = pager->pages.at(i);
        //     if (page == nullptr) {
        //         continue;
        //     }
        
        //     pager->flush(i, PAGE_SIZE);
        // }
        
        // uint32_t num_additional_rows = table->num_rows % pager->getRowsPerPage();
        // if (num_additional_rows > 0) {
        //     uint32_t page_num = num_full_pages;
        //     if (pager->pages.at(page_num) != nullptr) {
        //         pager->flush(page_num, num_additional_rows * ROW_SIZE);
        //     }
        // }
        
        // close(pager->fd);

        std::exit(EXIT_SUCCESS);
    }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}
