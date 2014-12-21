#include "_tree.h"


void _tree_node_walk(_Tree* tree, _TreeNode* node, unsigned int depth, TreeWalkCallback callback) {
    if (node == NULL){
        return;
    }
    if (depth == 0){
        for(int i = 0; i < tree->leaf_order; i++){
            void* value = _TREE_CHILD(tree, depth, node, i, void);
            if (value != NULL){
                callback(value);
            }
        }
    }
    else{
        for(unsigned int i = 0; i < tree->order; i++) {
            _tree_node_walk(tree, _TREE_CHILD(tree, depth, node, i, _TreeNode), depth - 1, callback);
        }
    }
}


void _tree_walk(_Tree* tree, TreeWalkCallback callback) {
    _tree_node_walk(tree, tree->root, tree->depth, callback);
}


typedef struct{
    _TreeNode* node;
    size_t index;
} TreeCursorStackFrame;


typedef struct{
    TreeCursorStackFrame* stack;
    TreeCursorStackFrame* stack_end;
    _Tree* tree;
    size_t depth;
} TreeCursorMemo;


void _tree_cursor_next(void* memo, Document** value) {
    TreeCursorMemo* m = (TreeCursorMemo*)memo;
    TreeCursorStackFrame* frame = m->stack;
    
    if (frame->index == m->tree->leaf_order) {
        for(;;) {
            if (frame == m->stack_end) {
                // all nodes have been visited. cleanup
                *value = NULL;
                goto done;
            }
            // move up the stack until we find a node that has remaining child nodes
            frame++;
            if (frame->index < m->tree->leaf_order - 1) {
                // pick the next branch
                frame->index++;
                while (frame != m->stack){
                    // move down the stack until we reach a leaf node
                    _TreeNode* child = _TREE_CHILD(m->tree, 1, frame->node, frame->index, _TreeNode);
                    frame--;
                    frame->node = child;
                    frame->index = 0;
                }
                break;
            }
        }
    }
    *value = _TREE_CHILD(m->tree, 0, frame->node, frame->index++, Document);
    if (*value != NULL) {
        return;
    }
done:
    free(m->stack);
    free(m);
}

Cursor* _tree_cursor(_Tree* tree) {
    TreeCursorMemo* memo = malloc(sizeof(*memo));
    memo->tree = tree;
    memo->stack = calloc(sizeof(TreeCursorStackFrame), tree->depth + 1);
    memo->stack_end = memo->stack + tree->depth;

    _TreeNode* node = tree->root;
    for (TreeCursorStackFrame* frame = memo->stack_end; frame >= memo->stack; frame--) {
        frame->node = node;
        frame->index = 0;
        node = _TREE_CHILD(tree, 0, node, 0, _TreeNode);
    }

    return cursor_create(&_tree_cursor_next, memo);
}
