#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_KEY 0x5678
#define MAX_BATCH_SIZE 3000  /*65kb*/

// Function to read data from shared memory
const char* read_from_shared_memory() {
    int shmid = shmget(SHM_KEY, sizeof(int) + MAX_BATCH_SIZE, 0666);
    // Attach to shared memory
    char* shm_addr = shmat(shmid, NULL, 0);
    const char* data = shm_addr + sizeof(int);   
    printf(data);  
    return 0;
}

