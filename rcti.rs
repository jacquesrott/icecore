extern crate libc;


#[repr(C)]
struct NativeTree;


#[link(name="cti")]
extern {
    fn tree_create(version: u64, base: Option<*mut NativeTree>) -> *mut NativeTree;
    fn tree_insert(tree: *mut NativeTree, id: u64, value: u64);
    fn tree_get(tree: *mut NativeTree, id: u64, value: &mut u64);
    fn tree_delete(tree: *mut NativeTree) -> ();
    fn tree_size(tree: *mut NativeTree) -> libc::size_t;
    fn tree_dump(tree: *mut NativeTree) -> ();
}


pub struct Tree{
    native_tree: *mut NativeTree,
}


impl Tree{
    pub fn new(version: u64) -> Tree{
        println!(".");
        unsafe{
            let t: *mut NativeTree;
            println!(".");
            t = tree_create(version, None);
            Tree{
                native_tree: t,
            }
        }
    }
    
    pub fn insert(&mut self, id: u64, value: u64) {
        unsafe{
            tree_insert(self.native_tree, id, value);
        }
    }
    
    pub fn get(&self, id: u64) -> u64 {
        let mut value = 0u64;
        unsafe{
            tree_get(self.native_tree, id, &mut value);
        }
        value
    }
    
    pub fn size(&self) -> uint {
        unsafe{
            tree_size(self.native_tree) as uint
        }
    }
    
    pub fn dump(&self) -> () {
        unsafe{
            tree_dump(self.native_tree);
        }
    }
}

impl Drop for Tree{
    fn drop(&mut self) -> () {
        unsafe{
            tree_delete(self.native_tree);
        }
    }
}


fn main(){
    let mut tree = Tree::new(1);
    for i in range(0, 50u){
        tree.insert(i as u64, i as u64);
    }
    tree.dump();
    println!("size = {}", tree.size());
}