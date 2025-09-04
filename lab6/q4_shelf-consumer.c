#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

// Define the names for the two named pipes (must match the producer)
#define PRODUCER_TO_CONSUMER_FIFO "/tmp/prod_to_cons_fifo"
#define CONSUMER_TO_PRODUCER_FIFO "/tmp/cons_to_prod_fifo"

int main() {
    int p_to_c_fd; // File descriptor for producer-to-consumer pipe
    int c_to_p_fd; // File descriptor for consumer-to-producer pipe
    int shelf_count;

    printf("Consumer starting...\n");
    printf("Connecting to producer...\n");

    // Open the pipes. The order is important to prevent deadlock and must be
    // the reverse of the producer's open calls. The consumer opens its read-only
    // pipe first, which will block until the producer opens it for writing.
    p_to_c_fd = open(PRODUCER_TO_CONSUMER_FIFO, O_RDONLY);
    if (p_to_c_fd == -1) {
        perror("open producer_to_consumer_fifo");
        exit(EXIT_FAILURE);
    }

    // Then, the consumer opens its write-only pipe.
    c_to_p_fd = open(CONSUMER_TO_PRODUCER_FIFO, O_WRONLY);
     if (c_to_p_fd == -1) {
        perror("open consumer_to_producer_fifo");
        exit(EXIT_FAILURE);
    }

    printf("Producer connected.\n");

    // Main loop for the consumer
    while (1) {
        // Wait for the producer to send the current shelf count.
        // This read call is blocking and synchronizes the processes.
        int bytes_read = read(p_to_c_fd, &shelf_count, sizeof(shelf_count));
        if (bytes_read == -1) {
            perror("read from producer");
            break;
        }
        // If read returns 0, the producer has closed its end of the pipe.
        if (bytes_read == 0) {
            printf("Producer disconnected.\n");
            break;
        }

        // Check if there are items on the shelf to take.
        if (shelf_count > 0) {
            shelf_count--;
            printf("Consumer took an item. Shelf count: %d\n", shelf_count);
        } else {
            // Report a trip to the shelf even if it's empty.
            printf("Trip to the shelf, but no items. Shelf count: %d\n", shelf_count);
        }
        fflush(stdout); // Ensure the message is printed immediately.

        // Simulate time taken to consume an item.
        // Making the consumer slower than the producer will cause the shelf to fill up.
        sleep(2);

        // Send the updated (or same) count back to the producer to signal that
        // the consumer has completed its turn. This write call will block until
        // the producer is ready to read.
        if (write(c_to_p_fd, &shelf_count, sizeof(shelf_count)) == -1) {
            perror("write to producer");
            break;
        }
    }

    // Clean up: close file descriptors.
    close(p_to_c_fd);
    close(c_to_p_fd);

    return 0;
}
