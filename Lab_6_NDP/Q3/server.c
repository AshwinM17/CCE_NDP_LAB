#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF 1024

// Function to perform Hamming error correction
void hamming_correct(char *data) {
    int r = 0, i, j, len = strlen(data);
    while ((1 << r) < len + 1) r++; // Calculate the number of parity bits

    int error = 0;
    for (i = 0; i < r; i++) {
        int pos = 1 << i, parity = 0;
        for (j = pos - 1; j < len; j += 2 * pos)
            for (int k = 0; k < pos && j + k < len; k++)
                parity ^= (data[j + k] - '0'); // Check parity bits
        if (parity) error += pos; // If error found, mark the position
    }

    if (error) data[error - 1] = (data[error - 1] == '0') ? '1' : '0'; // Flip the bit to correct the error
}

// Function to decode the received data (remove parity bits and extract actual data)
void decode_data(char *data) {
    int i, j = 0;
    int len = strlen(data);
    char decoded_data[BUF];  // Temporary buffer to hold decoded data

    // Decode: Remove the parity bits (positions 1, 2, 4, 8, etc.)
    for (i = 0; i < len; i++) {
        int pos = 1;
        // Find the next parity bit position (1, 2, 4, 8, 16, etc.)
        while (pos <= i) pos *= 2;
        
        if (i + 1 != pos) {  // Skip parity bits (i+1 is the 1-based index)
            decoded_data[j++] = data[i];  // Collect the data bits (skipping parity bit positions)
        }
    }
    decoded_data[j] = '\0'; // Null-terminate the string

    // Copy decoded data to the passed buffer
    printf("Decoded (original) data: %s\n", decoded_data); // Log the decoded original data (after removing parity bits)
}

int main() {
    int s, c, opt = 1;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    char buf[BUF];

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == 0) exit(1); // Create the socket
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) exit(1); // Bind the socket
    listen(s, 3); // Start listening for connections
    printf("Listening on %d...\n", PORT);

    while ((c = accept(s, (struct sockaddr *)&addr, &addrlen)) >= 0) { // Accept a new connection
        recv(c, buf, BUF, 0); // Receive data from client
        printf("Received Hamming code: %s\n", buf); // Log the received Hamming code
        hamming_correct(buf);  // Correct errors using Hamming code
        printf("Corrected Hamming code: %s\n", buf); // Log the corrected Hamming code
        decode_data(buf);      // Decode and store the actual decoded data
        send(c, buf, strlen(buf) + 1, 0); // Send back the corrected Hamming code
        close(c); // Close the client connection
    }

    close(s); // Close the server socket
    return 0;
}
