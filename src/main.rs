extern crate getopts;
extern crate icecore;

use icecore::data::{FileStore, DataStore};
use getopts::{optopt, optflag, getopts, usage};
use std::os;


fn insert(data: &str) {
    let store = FileStore::new("./_documents");
    store.write(data.as_bytes());
}


fn main() {
    let args: Vec<String> = os::args();

    match args[1].as_slice() {
        "insert" => {
            let opts = [
                optopt("d", "data", "Data to insert into the database", "DATA"),
                optflag("h", "help", "print this help message and exit"),
            ];

            let matches = match getopts(args.tail(), &opts) {
                Ok(m) => m,
                Err(_) => panic!("{}", usage("Insert data in the data store", &opts)),
            };

            if matches.opt_present("h") {
                println!("{}", usage("Insert data in the data store", &opts));
                return;
            }

            let data = matches.opt_str("d").unwrap();
            insert(data.as_slice());
        },
        _ => {
            println!("Command not found.");
        }
    }
}
