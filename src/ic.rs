use data::DataStore;


pub struct Document{
    id: u64,
    version: u64,
    hash: String,
}

pub struct Transaction{
    version: u64,
}


pub struct VersionIndex{
    document_id: u64,
    versions: Vec<Document>,
}


pub struct Icecore<'a>{
    store: Box<DataStore + 'a>,
    transaction_log: Vec<Transaction>,
    document_index: Vec<VersionIndex>,
}


impl<'a> Icecore<'a>{
    pub fn new(data_store: Box<DataStore + 'a>) -> Icecore<'a>{
        Icecore {
            store: data_store,
            transaction_log: Vec::new(),
            document_index: Vec::new(),
        }
    }

    fn get_next_document_id(&self) -> u64 {
        self.document_index.len().to_u64().expect("document id overflow") + 1
    }

    fn get_next_transaction_id(&self) -> u64 {
        self.transaction_log.len().to_u64().expect("transaction id overflow") + 1
    }

    fn get_version_index(&self, id: u64) -> &VersionIndex {
        self.document_index.index(&(id - 1).to_uint().unwrap())
    }

    fn get_version_index_mut(&mut self, id: u64) -> &mut VersionIndex {
        self.document_index.index_mut(&(id - 1).to_uint().unwrap())
    }

    pub fn insert(&mut self, data: &str) -> u64 {
        let hash = self.store.write(data.as_bytes());
        let id = self.get_next_document_id();
        let doc = Document {
            id: id,
            version: self.get_next_transaction_id(),
            hash: hash,
        };
        let mut index = VersionIndex {
            document_id: doc.id,
            versions: Vec::new(),
        };
        index.versions.push(doc);
        self.document_index.push(index);

        id
    }

    pub fn get(&self, id: u64, version: Option<u64>) -> Vec<u8> {
        let index = self.get_version_index(id);
        let doc = match version {
            Some(v) => &index.versions[v.to_uint().unwrap() - 1],
            None => index.versions.last().expect("no document versions found"),
        };
        self.store.read(&doc.hash)
    }

    pub fn update(&mut self, id: u64, data: &str) -> u64 {
        let hash = self.store.write(data.as_bytes());
        let version = self.get_next_transaction_id();
        let doc = Document {
            id: id,
            version: version,
            hash: hash,
        };
        let mut index = self.get_version_index_mut(id);
        index.versions.push(doc);

        version
    }
}

