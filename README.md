# icecore

> Git for machines

## Requirements

- Rust 0.13.0-nightly

## Usage

```

icecore <command> [<id>] <options>

    insert
        Insert new document in the datastore

        -d  --data      DATA    Data to write

    get <id>
        Get a document by <id>

        -v  --version   VERSION Document on a specific version

    update <id>
        Update a document by <id>

        -d  --data      DATA    Data to write

    demo
        Runs an insert, a few gets and an update

    server
        Starts a TCP server which handle CRUD operations on documents.
        Operations available are : INSERT, UPDATE, GET and QUIT.
        By default listening on 127.0.0.1:7778.

```

## Builds

[![Build Status](https://travis-ci.org/jacquesrott/icecore.svg?branch=master)](https://travis-ci.org/jacquesrott/icecore)
