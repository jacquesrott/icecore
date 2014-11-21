extern crate icecore;

use icecore::data::FileStore;
use icecore::data::DataStore;


fn main() {
  let foo = "bar";
  let store = FileStore::new(String::from_str("_documents"));
  store.write(foo.as_bytes());
}
