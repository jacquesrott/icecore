use std::slice::{Found, NotFound};

pub struct Document{
    pub id: u64,
    pub version: u64,
    pub hash: String,
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
    pub document_id: u64,
    pub versions: Vec<Document>,
}


impl VersionIndex {
    pub fn find_version(&self, version: u64) -> Option<&Document> {
        let index = match self.versions.binary_search(|document| document.cmp_version(version)) {
            Found(i) => i,
            NotFound(i) => {
                if i > 0 {
                    i - 1
                } else {
                    return None
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
    pub fn new() -> DocumentIndex{
        DocumentIndex {
            documents: Vec::new(),
        }
    }

    pub fn dump(&self, w: &mut Writer) {
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

    pub fn load(&mut self, r: &mut Reader) {
        let document_count = r.read_be_u64().unwrap();
        for _ in range(0, document_count) {
            let id = r.read_be_u64().unwrap();
            let version_count = r.read_be_u64().unwrap();
            let mut index = VersionIndex {
                document_id: id,
                versions: Vec::new(),
            };
            for _ in range(0, version_count) {
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

    pub fn get_version_index(&self, id: u64) -> &VersionIndex {
        self.documents.index(&(id - 1).to_uint().unwrap())
    }

    pub fn get_version_index_mut(&mut self, id: u64) -> &mut VersionIndex {
        self.documents.index_mut(&(id - 1).to_uint().unwrap())
    }

    pub fn get_next_id(&self) -> u64 {
        self.documents.len().to_u64().expect("document id overflow") + 1
    }

    pub fn add(&mut self, doc: Document) {
        let mut index = VersionIndex {
            document_id: doc.id,
            versions: Vec::new(),
        };
        index.versions.push(doc);
        self.documents.push(index);
    }
}


#[cfg(test)]
mod tests {
    use super::{Document, VersionIndex, DocumentIndex};

    #[test]
    fn document_compare_version() {
        let doc = Document {
            id: 0u64,
            version: 1u64,
            hash: String::from_str("hash")};
        assert_eq!(doc.cmp_version(1), Equal);
        assert_eq!(doc.cmp_version(2), Less);
        assert_eq!(doc.cmp_version(0), Greater);
    }

    #[test]
    fn version_index_find_version() {
        let mut versions = vec![];

        versions.push(Document {
            id: 2u64,
            version: 2u64,
            hash: String::from_str("hash")});

        versions.push(Document {
            id: 4u64,
            version: 4u64,
            hash: String::from_str("hash")});

        let vi = VersionIndex {
            document_id: 0u64,
            versions: versions
        };
        let doc = vi.find_version(2).unwrap();
        assert_eq!(doc.id, 2u64);

        let doc = vi.find_version(3).unwrap();
        assert_eq!(doc.id, 2u64);

        let doc = vi.find_version(999).unwrap();
        assert_eq!(doc.id, 4u64);

        let doc = vi.find_version(1);
        assert!(doc.is_none());
    }

    #[test]
    fn document_index_new() {
        let di = DocumentIndex::new();
        assert_eq!(di.documents.len(), 0);
    }

    #[test]
    fn document_index_get_version_index() {
        let mut di = DocumentIndex::new();

        di.documents.push(VersionIndex {
            document_id: 3u64,
            versions: Vec::new()
        });

        {
            let version = di.get_version_index(1u64);
            assert_eq!(version.document_id, 3u64);
        }
        let mut version = di.get_version_index_mut(1u64);
        assert_eq!(version.document_id, 3u64);
    }

    #[test]
    fn document_index_get_next_id() {
        let mut di = DocumentIndex::new();

        di.documents.push(VersionIndex {
            document_id: 3u64,
            versions: Vec::new()
        });

        assert_eq!(di.get_next_id(), 2u64);
    }

    #[test]
    fn document_index_add() {
        let mut di = DocumentIndex::new();
        let doc = Document {
            id: 2u64,
            version: 4u64,
            hash: String::from_str("hash")
        };

        di.add(doc);

        assert_eq!(di.documents.len(), 1);
        let mut index = di.documents.pop().unwrap();

        assert_eq!(index.document_id, 2u64);
        assert_eq!(index.versions.len(), 1);
        let version = index.versions.pop().unwrap();

        assert_eq!(version.id, 2u64);
        assert_eq!(version.version, 4u64);
    }
}
