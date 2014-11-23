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


pub struct DocumentIndex {
    documents: Vec<VersionIndex>,
}


impl DocumentIndex {
    fn new() -> DocumentIndex{
        DocumentIndex {
            documents: Vec::new(),
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
    store: Box<DataStore + 'a>,
    transaction_log: Vec<Transaction>,
    document_index: DocumentIndex,
}


impl<'a> Icecore<'a>{
    pub fn new(data_store: Box<DataStore + 'a>) -> Icecore<'a>{
        Icecore {
            store: data_store,
            transaction_log: Vec::new(),
            document_index: DocumentIndex::new(),
        }
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
            Some(v) => &index.versions[v.to_uint().unwrap() - 1],
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

