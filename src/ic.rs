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

#[cfg(test)]
mod test {
    use super::Icecore;
    use data::FileStore;
    use document::{Transaction, DocumentIndex};
    use std::io::fs::PathExtensions;
    use std::io::TempDir;

    fn get_icecore<'a>(tmpdir: &Path) -> Icecore<'a> {
        let tmpdir_string = String::from_str(tmpdir.as_str().unwrap());
        let store = box FileStore::new(tmpdir_string);
        let ic = Icecore::new(tmpdir.clone(), store);

        return ic;
    }

    #[test]
    fn icecore_new_sets_proper_index_file_path() {
        let tmpdir = TempDir::new("test").unwrap();
        let tmpdir = tmpdir.path();
        let ic = get_icecore(tmpdir);
        let expected = tmpdir.join("_documents.index");
        let expected = expected.as_str().unwrap();
        assert_eq!(ic.index_file_path.as_str().unwrap(), expected);
    }

    #[test]
    fn icecore_new_sets_proper_path() {
        let tmpdir = TempDir::new("test").unwrap();
        let tmpdir = tmpdir.path();
        let ic = get_icecore(tmpdir);
        let expected = tmpdir.as_str().unwrap();

        assert_eq!(ic.path.as_str().unwrap(), expected);
    }

    #[test]
    fn icecore_get_next_transaction_id_returns_proper_id() {
        let tmpdir = TempDir::new("test").unwrap();
        let tmpdir = tmpdir.path();
        let mut ic = get_icecore(tmpdir);
        ic.transaction_log.push(Transaction {version: 1u64});

        assert_eq!(ic.get_next_transaction_id(), 2u64);
    }

    #[test]
    fn icecore_insert_adds_new_document_to_document_index() {
        let tmpdir = TempDir::new("test").unwrap();
        let tmpdir = tmpdir.path();
        let mut ic = get_icecore(tmpdir);
        assert_eq!(ic.document_index.documents.len(), 0);

        let _ = ic.insert(String::from_str("test-data"));
        assert_eq!(ic.document_index.documents.len(), 1);
    }

    #[test]
    fn icecore_insert_returns_id_created() {
        let tmpdir = TempDir::new("test").unwrap();
        let tmpdir = tmpdir.path();
        let mut ic = get_icecore(tmpdir);
        let id = ic.insert(String::from_str("test-data"));

        assert_eq!(id, ic.document_index.documents[0].document_id);
    }

    #[test]
    fn icecore_dump_creates_proper_dump_file() {
        let tmpdir = TempDir::new("test").unwrap();
        let tmpdir = tmpdir.path();
        let mut ic = get_icecore(tmpdir);
        assert_eq!(ic.index_file_path.exists(), false);
        ic.dump();
        assert_eq!(ic.index_file_path.exists(), true);
    }

    #[test]
    fn icecore_load_loads_all_documents() {
        let tmpdir = TempDir::new("test").unwrap();
        let tmpdir = tmpdir.path();
        let mut ic = get_icecore(tmpdir);
        let id1 = ic.insert(String::from_str("test-data"));
        let id2 = ic.insert(String::from_str("test-data2"));
        ic.dump();

        ic.document_index = DocumentIndex::new();
        assert_eq!(ic.document_index.documents.len(), 0);

        ic.load();
        assert_eq!(ic.document_index.documents.len(), 2);

        assert_eq!(ic.document_index.documents[0].document_id, id1);
        assert_eq!(ic.document_index.documents[1].document_id, id2);
    }
}
