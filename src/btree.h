#ifndef DBSCRATCH_BTREE_H
#define DBSCRATCH_BTREE_H

#include "pager.h"

enum NodeType {
    NODE_INTERNAL,
    NODE_LEAF,
};

/* Node Format:
 * |--------------------------------------------------------------------------------------------|
 * | NODE_TYPE | IS_ROOT | PARENT_POINTER | LEAF_NODE_NUM_CELLS | KEY1 | VALUE1 | KEY2 | VALUE2 | ...
 * |--------------------------------------------------------------------------------------------|
 * */
class Node {
public:
    /* Common Node Header Layout */
    const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
    const uint32_t NODE_TYPE_OFFSET = 0;
    const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
    const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
    const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
    const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
    const uint32_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

    /* Leaf Node Header Layout */
    const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
    const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
    const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE;

    /* Leaf Node Body Layout */
    const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
    const uint32_t LEAF_NODE_KEY_OFFSET = 0;
    const uint32_t LEAF_NODE_VALUE_SIZE = 291 + sizeof(uint32_t) + sizeof(uint32_t); // TODO dynamic
    const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
    const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE; // a cell is a k/v pair
    const uint32_t LEAF_NODE_SPACE_FOR_CELLS = Pager::PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
    const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;
};

class BTree {

};

#endif //DBSCRATCH_BTREE_H
