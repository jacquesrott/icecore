extern crate getopts;
extern crate icecore;

use getopts::{optopt, optflag, getopts, usage};
use std::os;

use icecore::data::FileStore;
use icecore::ic::Icecore;
use icecore::api;


fn demo(ic: &mut Icecore) {
    let id = ic.insert(String::from_str("foo"));
    println!("new document: id={}", id);
    println!("get({}, None) -> {}", id, ic.get(id, None));

    ic.update(id, String::from_str("bar"));
    println!("get({}, None) -> {}", id, ic.get(id, None));
    println!("get({}, 1) -> {}", id, ic.get(id, Some(0u64)));

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

    if args.len() < 2 {
        panic!("not enough arguments");
    }

    match args[1].as_slice() {
        "insert" => {
            let opts = [
                optopt("d", "data", "Data to write", "DATA"),
                optflag("h", "help", "print this help message and exit"),
            ];
            let matches = match getopts(args.slice_from(1), &opts) {
                Ok(m) => m,
                Err(_) => panic!(usage("Insert new document", &opts)),
            };

            let data = matches.opt_str("d").unwrap();
            ic.insert(data);
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
        "update" => {
            let opts = [
                optopt("d", "data", "Data to write", "DATA"),
            ];
            let matches = match getopts(args.slice_from(2), &opts) {
                Ok(m) => m,
                Err(_) => panic!(usage("", &opts)),
            };

            let id = from_str::<u64>(args[2].as_slice()).unwrap();
            let data = matches.opt_str("d").unwrap();
            println!("updating document id={}", id);
            ic.update(id, data);
        },
        "demo" => {
            demo(&mut ic);
        },
        "server" => {
            api::run(&mut ic);
        },
        _ => {
            println!("Command not found.");
        }
    }

    ic.dump();
}
