use std::io::{File};
use std::io::fs::PathExtensions;
use data::DataStore;
use document::{Transaction, Document, DocumentIndex};


pub struct Icecore<'a>{
    path: Path,
    index_file_path: Path,
    store: Box<DataStore + 'a>,
    transaction_log: Vec<Transaction>,
    document_index: DocumentIndex,
}


impl<'a> Icecore<'a>{
    pub fn new(path: Path, data_store: Box<DataStore + 'a>) -> Icecore<'a>{
        let index_file_path = path.clone().join("_documents.index");
        Icecore {
            path: path,
            index_file_path: index_file_path,
            store: data_store,
            transaction_log: Vec::new(),
            document_index: DocumentIndex::new(),
        }
    }

    pub fn load(&mut self) {
        if !self.index_file_path.exists() {
            println!("no document index file found");
            return;
        }
        let mut reader = File::open(&self.index_file_path).unwrap();
        self.document_index.load(&mut reader);
    }

    pub fn dump(&mut self) {
        let mut writer = File::create(&self.index_file_path).unwrap();
        self.document_index.dump(&mut writer);
    }

    fn get_next_transaction_id(&self) -> u64 {
        self.transaction_log.len().to_u64().expect("transaction id overflow") + 1
    }

    pub fn insert(&mut self, data: String) -> u64 {
        let hash = self.store.write(data);
        let id = self.document_index.get_next_id();
        let doc = Document {
            id: id,
            version: self.get_next_transaction_id(),
            hash: hash,
        };
        self.document_index.add(doc);

        id
    }

    pub fn get(&self, id: u64, version: Option<u64>) -> String {
        let index = self.document_index.get_version_index(id);
        let doc = match version {
            Some(v) => index.find_version(v.to_u64().unwrap()).unwrap(),
            None => index.versions.last().expect("no document versions found"),
        };
        self.store.read(&doc.hash)
    }

    pub fn update(&mut self, id: u64, data: String) -> u64 {
        let hash = self.store.write(data);
        let version = self.get_next_transaction_id();
        let doc = Document {
            id: id,
            version: version,
            hash: hash,
        };
        let mut index = self.document_index.get_version_index_mut(id);
        index.versions.push(doc);

        version
    }
}

