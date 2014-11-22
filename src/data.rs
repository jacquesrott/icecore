use std::io::File;
use serialize::hex::ToHex;
use rust_crypto::md5::Md5;
use rust_crypto::digest::Digest;


pub trait DataStore {
  fn write(&self, data: &[u8]) -> String;
  fn read(&self, hash: &String) -> Vec<u8>;
}


pub struct FileStore{
    path: Path,
}


impl FileStore{
    pub fn new(path: String) -> FileStore {
        FileStore {
            path: Path::new(path.as_slice()),
        }
    }

    fn get_path(&self, hash: &String) -> Path {
        self.path.clone().join(hash)
    }
}


impl DataStore for FileStore{
    fn write(&self, data: &[u8]) -> String{
        println!("write {}", data);
        let mut h = Md5::new();
        h.input(data);
        let mut hashbits: [u8, ..16] = [0u8, ..16];
        h.result(&mut hashbits);
        let data_hash = hashbits.as_slice().to_hex();
        let path = self.get_path(&data_hash);

        let mut file = File::create(&path);
        file.write(data);
        data_hash
    }

    fn read(&self, hash: &String) -> Vec<u8>{
        println!("read {}", hash);
        let path = self.get_path(hash);
        File::open(&path).read_to_end().unwrap()
    }
}


