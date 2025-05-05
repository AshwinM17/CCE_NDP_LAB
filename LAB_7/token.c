#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define REPLENISH_RATE 10    // KBps
#define MAX_RATE 20          // KBps (for part iii)
#define BUCKET_SIZE 50       // KBytes
#define PACKET_SIZE 15       // KBytes
#define PACKET_INTERVAL 0.5  // seconds

typedef struct {
    double tokens;
    double capacity;
    double rate;
} TokenBucket;

void update_tokens(TokenBucket *bucket, double elapsed_time) {
    bucket->tokens += bucket->rate * elapsed_time;
    if (bucket->tokens > bucket->capacity)
        bucket->tokens = bucket->capacity;
}

int main() {
    TokenBucket bucket = {BUCKET_SIZE, BUCKET_SIZE, REPLENISH_RATE};
    double time = 0.0;
    int packets_sent = 0;

    // (i)
    while (time <= 1.5) {
        update_tokens(&bucket, PACKET_INTERVAL);
        time += PACKET_INTERVAL;

        if (bucket.tokens >= PACKET_SIZE) {
            bucket.tokens -= PACKET_SIZE;
            packets_sent++;
        } else {
            break; // Packet is queued
        }
    }
    
    printf("Tokens left in the bucket after 1.5 seconds: %.2f KB\n", bucket.tokens);

    // (ii)
    bucket.tokens = BUCKET_SIZE;
    time = 0.0;
    packets_sent = 0;
    while (1) {
        update_tokens(&bucket, PACKET_INTERVAL);
        time += PACKET_INTERVAL;
        
        if (bucket.tokens >= PACKET_SIZE) {
            bucket.tokens -= PACKET_SIZE;
            packets_sent++;
        } else {
            printf("Packets start queuing at t = %.1f seconds\n", time);
            break;
        }
    }
    
    // (iii)
    bucket.tokens = BUCKET_SIZE;
    double max_burst = bucket.tokens + MAX_RATE;
    printf("Maximum possible burst size: %.2f KB\n", max_burst);
    
    return 0;
}
