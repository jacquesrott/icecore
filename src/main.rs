extern crate icecore;

use icecore::data::{FileStore, DataStore};
use getopts::{optopt, getopts};
use std::os;


fn insert(data: &str) {
    let store = FileStore::new("./_documents");
    store.write(foo.as_bytes());
}


fn main() {
    let args: Vec<String> = os::args();

    match args[1].as_slice() {
        "insert" => {
            let opts = [
                optopt("d", "data", "Data to insert into the database", "DATA"),
            ];
            let matches = match getopts(args.tail(), opts) {
                Ok(m) => m,
                Err(e) => panic!(e.to_string()),
            };

            let data = if matches.opt_present("d") {
                matches.opt_str("d").unwrap()
            };

            insert(data);
        },
        _ => {
            println!("Command not found.");
        }
    }
}
