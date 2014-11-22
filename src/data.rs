use std::io::fs;
use std::io::{File, USER_RWX};
use serialize::hex::ToHex;
use rust_crypto::md5::Md5;
use rust_crypto::digest::Digest;


pub trait DataStore {
    fn write(&self, data: &str) -> ();
}


pub struct FileStore{
    root: Path,
}


impl FileStore{
    pub fn new(path: &str) -> FileStore {
        let root = Path::new(path);

        match fs::mkdir_recursive(&root, USER_RWX) {
            Ok(_) => {},
            Err(e) => println!("Folder error: {}", e.kind),
        }

        FileStore {
            root: root,
        }
    }
}


impl DataStore for FileStore{
    fn write(&self, data: &str){
        let data_bytes = data.as_bytes();
        let mut hash = Md5::new();
        hash.input(data_bytes);

        let mut hashbits = [0u8, ..16];
        hash.result(&mut hashbits);

        let path = self.root.join(hashbits.as_slice().to_hex());

        let mut file = match File::create(&path) {
            Ok(f) => f,
            Err(e) => panic!("File error: {}", e),
        };
        println!("Created:\t\"{}\"", path.display());

        match file.write(data_bytes) {
            Ok(_) => {},
            Err(e) => panic!("File error: {}", e),
        };
        println!("Wrote:\t\t\"{}\"", data);
    }
}
