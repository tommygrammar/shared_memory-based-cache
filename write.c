#include <mongoc/mongoc.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SHM_KEY 0x5678   // Shared memory key
#define SIGNAL_READY 1
#define SIGNAL_IDLE 0

// Function to perform a write to MongoDB
void write_to_mongodb(const char *data) {
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_t *doc;
    bson_error_t error;

    mongoc_init();
    client = mongoc_client_new("mongodb://localhost:27017");
    collection = mongoc_client_get_collection(client, "test_db", "test_collection");

    // Create a BSON document from the data
    doc = bson_new_from_json((const uint8_t *)data, -1, &error);

    if (!doc) {
        fprintf(stderr, "Error creating BSON: %s\n", error.message);
        return;
    }

    if (!mongoc_collection_insert_one(collection, doc, NULL, NULL, &error)) {
        fprintf(stderr, "Error inserting document: %s\n", error.message);
    }

    bson_destroy(doc);
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}

// Function to write signal to shared memory
void set_signal(int signal) {
    int shmid = shmget(SHM_KEY, sizeof(int), 0666 | IPC_CREAT);
    int *signal_mem = (int *)shmat(shmid, NULL, 0);
    *signal_mem = signal;
    shmdt(signal_mem);
}

// Exposed function to write data to MongoDB and set the signal
void write_and_signal(const char *data) {
    write_to_mongodb(data);
    set_signal(SIGNAL_READY);
}

