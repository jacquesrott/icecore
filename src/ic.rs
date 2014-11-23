use std::io::{File};
use std::io::fs::PathExtensions;
use std::slice::{Found, NotFound};
use data::DataStore;

pub struct Document{
    id: u64,
    version: u64,
    hash: String,
}


impl Document {
    pub fn cmp_version(&self, other: u64) -> Ordering {
        if self.version == other {
            Equal
        } else if self.version < other {
            Less
        } else {
            Greater
        }
    }
}


pub struct Transaction{
    version: u64,
}


pub struct VersionIndex {
    document_id: u64,
    versions: Vec<Document>,
}


impl VersionIndex {
    pub fn find_version(&self, version: u64) -> Option<&Document> {
        let index = match self.versions.binary_search(|document| document.cmp_version(version)) {
            Found(i) => i,
            NotFound(i) => {
                if i > 0 {
                    i - 1
                } else {
                    0
                }
            }
        };
        self.versions.get(index)
    }
}


pub struct DocumentIndex {
    documents: Vec<VersionIndex>,
}


impl DocumentIndex {
    fn new() -> DocumentIndex{
        DocumentIndex {
            documents: Vec::new(),
        }
    }

    fn dump(&self, w: &mut Writer) {
        w.write_be_u64(self.documents.len().to_u64().unwrap());
        for &ref index in self.documents.iter() {
            w.write_be_u64(index.document_id);
            w.write_be_u64(index.versions.len().to_u64().unwrap());
            for &ref doc in index.versions.iter() {
                w.write_be_u64(doc.version);
                w.write_str(doc.hash.as_slice());
            }
        }
    }

    fn load(&mut self, r: &mut Reader) {
        let document_count = r.read_be_u64().unwrap();
        for i in range(0, document_count) {
            let id = r.read_be_u64().unwrap();
            let version_count = r.read_be_u64().unwrap();
            let mut index = VersionIndex {
                document_id: id,
                versions: Vec::new(),
            };
            for j in range(0, version_count) {
                let version = r.read_be_u64().unwrap();
                let hash = r.read_exact(32).unwrap();  // hex encoded md5
                let doc = Document {
                    id: id,
                    version: version,
                    hash: String::from_utf8(hash).unwrap(),
                };
                index.versions.push(doc);
            }
            self.documents.push(index);
        }
    }

    fn get_version_index(&self, id: u64) -> &VersionIndex {
        self.documents.index(&(id - 1).to_uint().unwrap())
    }

    fn get_version_index_mut(&mut self, id: u64) -> &mut VersionIndex {
        self.documents.index_mut(&(id - 1).to_uint().unwrap())
    }

    fn get_next_id(&self) -> u64 {
        self.documents.len().to_u64().expect("document id overflow") + 1
    }

    fn add(&mut self, doc: Document) {
        let mut index = VersionIndex {
            document_id: doc.id,
            versions: Vec::new(),
        };
        index.versions.push(doc);
        self.documents.push(index);
    }

}


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

