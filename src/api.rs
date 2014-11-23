//extern crate icecore;

use std::io::{Listener, Acceptor};
use std::io::net::tcp::TcpListener;

use ic::Icecore;

const INSERT: u8 = 1u8;
const UPDATE: u8 = 2u8;
const GET: u8 = 3u8;
const QUIT: u8 = 255u8;


pub fn run(ic: &mut Icecore) {
    let mut acceptor = TcpListener::bind("127.0.0.1:7778").listen().unwrap();
    for opt_stream in acceptor.incoming() {
        let mut stream = opt_stream.unwrap();
        loop {
            let op = stream.read_u8().unwrap();
            println!("op={}", op);
            match op{
                INSERT => {
                    let len = stream.read_be_u32().unwrap();
                    let data = stream.read_exact(len.to_uint().unwrap()).unwrap();
                    let id = ic.insert(String::from_utf8(data).unwrap());
                    stream.write_be_u64(id);
                },
            
                UPDATE => {
                    let id = stream.read_be_u64().unwrap();
                    let v = stream.read_be_u64().unwrap();
                    let len = stream.read_be_u32().unwrap();
                    let data = stream.read_exact(len.to_uint().unwrap()).unwrap();
                    ic.update(id, String::from_utf8(data).unwrap());
                    stream.write_u8(0u8);
                },
            
                GET => {
                    let id = stream.read_be_u64().unwrap();
                    let v = stream.read_be_u64().unwrap();
                    let version = if v == 0 {None} else {Some(v)};
                    let data = ic.get(id, version);
                    let bytes = data.as_bytes();
                    println!("{}", bytes);
                    stream.write_be_u32(bytes.len().to_u32().unwrap());
                    stream.write(bytes);
                },
                
                QUIT => {
                    println!("QUIT");
                    break;
                },
                
                _ => {
                    println!("Invalid command. Disconnecting.");
                    break;
                }
                
            }
        }
    }
}
