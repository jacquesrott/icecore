extern crate getopts;
extern crate icecore;

use getopts::{optopt, optflag, getopts, usage};
use std::os;
use std::str::FromStr;

use icecore::data::FileStore;
use icecore::ic::Icecore;


fn insert(ic: &mut Icecore, data: String) {
    ic.insert(data);
}


fn get(ic: &mut Icecore, id: u64) {
    let data = ic.get(id, None);
    println!("{}", data);
}

fn demo(ic: &mut Icecore) {
    let id = ic.insert(String::from_str("foo"));
    println!("new document: id={}", id);
    println!("get({}, None) -> {}", id, ic.get(id, None));

    ic.update(id, String::from_str("bar"));
    println!("get({}, None) -> {}", id, ic.get(id, None));
    println!("get({}, 1) -> {}", id, ic.get(id, Some(1u64)));

    ic.dump();
}


fn main() {
    let args: Vec<String> = os::args();

    let store = box FileStore::new(String::from_str("_documents"));
    let mut ic = Icecore::new(
        Path::new("_documents"),
        store,
    );

    ic.load();

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
            insert(&mut ic, data);
        },
        "get" => {
            let opts = [
                optopt("v", "version", "Requested document version", "VERSION"),
            ];
            let matches = match getopts(args.slice_from(2), &opts) {
                Ok(m) => m,
                Err(_) => panic!(usage("Get document by id", &opts)),
            };
            let id = from_str::<u64>(args[2].as_slice()).unwrap();
            let version: Option<u64> = match matches.opt_str("v") {
                Some(v) => Some(from_str::<u64>(v.as_slice()).unwrap()),
                None => None,
            };
            println!("getting document id={} version={}", id, version);
            let data = ic.get(id, version);
            println!("{}", data);
        },
        "demo" => {
            demo(&mut ic);
        },
        _ => {
            println!("Command not found.");
        }
    }

    ic.dump();
}
