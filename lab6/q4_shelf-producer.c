#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define PRODUCER_TO_CONSUMER_FIFO "/tmp/prod_to_cons_fifo"
#define CONSUMER_TO_PRODUCER_FIFO "/tmp/cons_to_prod_fifo"

#define MAX_SHELF 5

int main() {
    int p_to_c_fd; // File descriptor for producer-to-consumer pipe
    int c_to_p_fd; // File descriptor for consumer-to-producer pipe
    int shelf_count = MAX_SHELF;

    // Create the FIFOs if they don't already exist.
    // mkfifo will return an error if the file exists, so we check for EEXIST
    // and ignore it, as it means another process (or a previous run) created it.
    if (mkfifo(PRODUCER_TO_CONSUMER_FIFO, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo producer_to_consumer_fifo");
        exit(EXIT_FAILURE);
    }
    if (mkfifo(CONSUMER_TO_PRODUCER_FIFO, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo consumer_to_producer_fifo");
        exit(EXIT_FAILURE);
    }

    printf("Producer starting...\n");
    printf("Waiting for consumer to connect...\n");

    // Open the pipes. The order is important to prevent deadlock.
    // The producer opens its write-only pipe first, which will block until the
    // consumer opens the same pipe for reading.
    p_to_c_fd = open(PRODUCER_TO_CONSUMER_FIFO, O_WRONLY);
    if (p_to_c_fd == -1) {
        perror("open producer_to_consumer_fifo");
        exit(EXIT_FAILURE);
    }

    // Then, the producer opens its read-only pipe.
    c_to_p_fd = open(CONSUMER_TO_PRODUCER_FIFO, O_RDONLY);
    if (c_to_p_fd == -1) {
        perror("open consumer_to_producer_fifo");
        exit(EXIT_FAILURE);
    }

    printf("Consumer connected. Initializing shelf count to %d.\n", shelf_count);

    // Send the initial shelf count to the consumer.
    if (write(p_to_c_fd, &shelf_count, sizeof(shelf_count)) == -1) {
        perror("write initial count");
        exit(EXIT_FAILURE);
    }

    // Main loop for the producer
    while (1) {
        // Wait for the consumer to send back the count after taking an item.
        // This read call is blocking and synchronizes the two processes.
        int bytes_read = read(c_to_p_fd, &shelf_count, sizeof(shelf_count));
        if (bytes_read == -1) {
            perror("read from consumer");
            break;
        }
        // If read returns 0, the consumer has closed its end of the pipe.
        if (bytes_read == 0) {
            printf("Consumer disconnected.\n");
            break;
        }

        // Check if the shelf has space to add another item.
        if (shelf_count < MAX_SHELF) {
            shelf_count++;
            printf("Producer added an item. Shelf count: %d\n", shelf_count);
        } else {
            printf("Shelf is full. Producer is waiting.\n");
        }
        fflush(stdout); // Ensure the message is printed immediately.

        // Simulate time taken to produce an item.
        sleep(1);

        // Send the updated count back to the consumer.
        // This write call will block until the consumer is ready to read.
        if (write(p_to_c_fd, &shelf_count, sizeof(shelf_count)) == -1) {
            perror("write to consumer");
            break;
        }
    }

    // Clean up: close file descriptors and remove the FIFOs from the filesystem.
    close(p_to_c_fd);
    close(c_to_p_fd);
    unlink(PRODUCER_TO_CONSUMER_FIFO);
    unlink(CONSUMER_TO_PRODUCER_FIFO);

    return 0;
}
