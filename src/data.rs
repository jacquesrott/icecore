use std::io::fs;
use std::io::{File, USER_RWX};
use serialize::hex::ToHex;
use rust_crypto::md5::Md5;
use rust_crypto::digest::Digest;


pub trait DataStore {
    fn write(&self, data: &[u8]) -> String;
    fn read(&self, hash: &String) -> Vec<u8>;
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

    fn get_path(&self, hash: &String) -> Path {
        self.path.clone().join(hash)
    }
}


impl DataStore for FileStore{
    fn write(&self, data: &[u8]) -> String{
        let mut h = Md5::new();
        h.input(data);

        let mut hashbits: [u8, ..16] = [0u8, ..16];
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
        data_hash
    }

    fn read(&self, hash: &String) -> Vec<u8>{
        println!("read {}", hash);
        let path = self.get_path(hash);
        File::open(&path).read_to_end().unwrap()
    }
}
