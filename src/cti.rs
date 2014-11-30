pub type Version = u64;
pub type Value = u64;
pub type Id = u64;

const LEAF_NODE_SIZE: uint = 16;
const MAX_VERSIONS: uint = 1000;


enum IndexError {
    OK = 0,
    DOCUMENT_DOES_NOT_EXIST,
    VERSION_DOES_NOT_EXIST,
    VERSION_SKIPPED,
}


struct LeafNode {
    version: Version,
    values: [Value, ..LEAF_NODE_SIZE]
}


struct IndexNode<'a> {
    version: Version,
    children: [Option<&'a IndexNode<'a>>, ..2],
}


impl<'a> IndexNode<'a> {
    pub fn new<'a>(version: Version) -> IndexNode<'a> {
        IndexNode {
            version: version,
            children: [None, None],
        }
    }
}


pub struct Tree<'b> {
    version: Version,
    root: &'b IndexNode<'b>,
    depth: uint,
}


struct Index<'c> {
    last_version: Version,
    trees: Vec<Option<&'c Tree<'c>>>,
}


impl<'b> Tree<'b> {
    pub fn new<'b>(version: Version, mut base: &'b Option<Tree<'b>>) -> Tree<'b> {
        let mut root: IndexNode<'b> = IndexNode::new(version);
        let depth = match *base {
            Some(ref b) => {
                root.children[0] = b.root.children[0];
                root.children[1] = b.root.children[1];
                b.depth
            },
            None => 1,
        };

        Tree {
            version: version,
            root: &root,
            depth: depth,
        }
    }

    pub fn capacity(&self) -> uint {
        LEAF_NODE_SIZE * (1 << self.depth)
    }

    pub fn insert<'b>(&'b mut self, id: Id, value: Value) {
        let mut capacity = self.capacity();
        while id >= capacity as u64 {
            let mut new_root: IndexNode<'b> = IndexNode::new(self.version);
            new_root.children[0] = Some(self.root);
            self.root = new_root;
            self.depth += 1;
            capacity *= 2;
        }

        let x: Id = id / LEAF_NODE_SIZE as u64;
        let ref node = self.root;
    }

    pub fn get(&self, id: Id, value: Value) -> int {
        0
    }

    pub fn dump(&self) {

    }
}
