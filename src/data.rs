use std::io::File;
use serialize::hex::ToHex;
use rust_crypto::md5::Md5;
use rust_crypto::digest::Digest;


static root_path: &'static str = "_documents";

pub fn write_data(data: &[u8]){
  let mut hash = Md5::new();
  hash.input(data);
  let mut hashbits: [u8, ..16] = [0u8, ..16];
  hash.result(&mut hashbits);
  let root: Path = Path::new(&root_path);
  let path = root.join(hashbits.as_slice().to_hex());

  let mut file = File::create(&path);
  file.write(data);
}
