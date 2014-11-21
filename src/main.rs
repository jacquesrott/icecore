extern crate icecore;

use icecore::data;

fn main() {
  let foo = "foo";
  data::write_data(foo.as_bytes());
}
