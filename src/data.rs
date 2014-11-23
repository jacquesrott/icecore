use std::io::fs;
use std::io::{File, USER_RWX};
use serialize::hex::ToHex;
use rust_crypto::md5::Md5;
use rust_crypto::digest::Digest;


pub trait DataStore {
    fn write(&self, data: String) -> String;
    fn read(&self, hash: String) -> String;
}


pub struct FileStore{
    root: Path,
}


impl FileStore{
    pub fn new(path: String) -> FileStore {
        let root = Path::new(path.as_slice());

        match fs::mkdir_recursive(&root, USER_RWX) {
            Ok(_) => {},
            Err(e) => println!("Folder error: {}", e.kind),
        }

        FileStore {
            root: root,
        }
    }

    fn get_path(&self, hash: String) -> Path {
        self.root.clone().join(hash)
    }
}


impl DataStore for FileStore{
    fn write(&self, data: String) -> String {
        let data_bytes = data.as_bytes();
        let mut h = Md5::new();
        h.input(data_bytes);

        let mut hashbits = [0u8, ..16];
        h.result(&mut hashbits);

        let data_hash = hashbits.as_slice().to_hex();
        let path = self.root.join(data_hash);

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
        hashbits.as_slice().to_hex()
    }

    fn read(&self, hash: String) -> String {
        println!("Read:\t\t\"{}\"", hash);
        let path = self.get_path(hash);
        match File::open(&path).read_to_string() {
            Ok(s) => s,
            Err(e) => panic!("File error: {}", e),
        }
    }
}
