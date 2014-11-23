extern crate getopts;
extern crate icecore;

use icecore::data::{FileStore, DataStore};
use getopts::{optopt, optflag, getopts, usage};
use std::os;


fn insert(data: String) {
    let store = FileStore::new(String::from_str("./_documents"));
    let hash = store.write(data);
    select(&store, hash);
}


fn select(store: &FileStore, hash: String) {
    let data = store.read(hash);
    println!("{}", data);
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
            insert(data);
        },
        _ => {
            println!("Command not found.");
        }
    }
}
