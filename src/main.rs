extern crate icecore;

use icecore::data::FileStore;
use icecore::ic::Icecore;


fn main() {
    let store = box FileStore::new(String::from_str("_documents"));
    let mut ic = Icecore::new(store);
    let id = ic.insert("foo");
    println!("new document: id={}", id);
    println!("get({}, None) -> {}", id, ic.get(id, None));

    ic.update(id, "bar");
    println!("get({}, None) -> {}", id, ic.get(id, None));
    println!("get({}, 1) -> {}", id, ic.get(id, Some(1u64)));

}
