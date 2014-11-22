use std::io::fs;
use std::io::{File, USER_RWX};
use serialize::hex::ToHex;
use rust_crypto::md5::Md5;
use rust_crypto::digest::Digest;


pub trait DataStore {
    fn write(&self, data: &[u8]) -> ();
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
    fn write(&self, data: &[u8]){
        let mut hash = Md5::new();
        hash.input(data);

        let mut hashbits = [0u8, ..16];
        hash.result(&mut hashbits);

        let path = self.root.join(hashbits.as_slice().to_hex());

        let mut file = match File::create(&path) {
            Ok(f) => f,
            Err(e) => panic!("File error: {}", e),
        };

        match file.write(data) {
            Ok(_) => {},
            Err(e) => panic!("File error: {}", e),
        };
    }
}
