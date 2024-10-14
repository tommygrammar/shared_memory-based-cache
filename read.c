#include <mongoc/mongoc.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SHM_KEY 0x5678  //Unique key for shared memory
#define SIGNAL_READY 1 /*Signal ready for update*/
#define SIGNAL_IDLE 0 /*Signal not ready for update*/

#define MAX_BATCH_SIZE 65536  /*64 kb*/


void read_from_mongodb_and_update_shared_memory() {
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    mongoc_cursor_t *cursor;
    bson_t *query;
    const bson_t *doc;
    char *json_str;

    int shmid;
    char *shared_mem;

    // Initialize MongoDB
    mongoc_init();
    client = mongoc_client_new("mongodb://localhost:27017");
    collection = mongoc_client_get_collection(client, "Hello_Brother", "Test_me");
    printf("Successful MongoDB initialization\n");

    // Create or access shared memory (allocate space for the signal and batch data)
    // Allocate shared memory segment with a size that includes space for a signal
    printf("Attempting to create shared memory with size: %lu\n", sizeof(int) + MAX_BATCH_SIZE);


    shmid = shmget(SHM_KEY, sizeof(int) + MAX_BATCH_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    } else {
        printf("Create or access shared memory successful\n");
    }

    // Attach shared memory
    shared_mem = (char *)shmat(shmid, NULL, 0);
    if (shared_mem == (char *)-1) {
        perror("shmat failed");
        exit(1);
    } else {
        printf("Shared memory attached successfully\n");
    }

    // Cast the first part of shared memory to signal memory for easier access
    int *signal_mem = (int *)shared_mem;

    while (1) {
        // Check if there's a signal to read data
        if (*signal_mem == SIGNAL_READY) {
            query = bson_new();  // Empty query to get all documents
            cursor = mongoc_collection_find_with_opts(collection, query, NULL, NULL);

            // Buffer for batch data
            char *batch_data = shared_mem + sizeof(int); // The data follows the signal in shared memory
            memset(batch_data, 0, MAX_BATCH_SIZE);

            // Read documents and accumulate them in the batch
            while (mongoc_cursor_next(cursor, &doc)) {
                json_str = bson_as_json(doc, NULL);
                strncat(batch_data, json_str, MAX_BATCH_SIZE - strlen(batch_data) - 1);
                bson_free(json_str);
            }

            printf(batch_data,"\n");

            // Reset the signal
            *signal_mem = SIGNAL_IDLE;

            printf("Batch read and updated shared memory.\n");

            mongoc_cursor_destroy(cursor);
            bson_destroy(query);
        }

        sleep(1);  // Avoid busy-waiting, check every second
    }

    // Clean up MongoDB connection
    mongoc_collection_destroy(collection);
    mongoc_client_destroy(client);
    mongoc_cleanup();

    shmdt(shared_mem);
}

int main() {
    read_from_mongodb_and_update_shared_memory();
    return 0;
}
