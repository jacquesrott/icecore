use std::io::File;
use serialize::hex::ToHex;
use rust_crypto::md5::Md5;
use rust_crypto::digest::Digest;


pub trait DataStore {
  fn write(&self, data: &[u8]) -> ();
}


pub struct FileStore{
  path: String,
}


impl FileStore{
  pub fn new(path: String) -> FileStore {
    FileStore {
      path: path,
    }
  } 
}


impl DataStore for FileStore{
  fn write(&self, data: &[u8]){
    let mut hash = Md5::new();
    hash.input(data);
    let mut hashbits: [u8, ..16] = [0u8, ..16];
    hash.result(&mut hashbits);
    let root: Path = Path::new(self.path.as_slice());
    let path = root.join(hashbits.as_slice().to_hex());

    let mut file = File::create(&path);
    file.write(data);
  }
}


