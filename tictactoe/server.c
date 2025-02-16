#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/tic_tac_toe_socket"
#define MAX_BUF 1024

// Define the game board
char board[3][3];
int currentPlayer = 1; // Player 1 starts
int client_sockets[2];

// Function to print the Tic-Tac-Toe board with grid lines
void print_board() {
    printf("\nCurrent Board:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf(" %c ", board[i][j]);
            if (j < 2) printf("|");
        }
        printf("\n");
        if (i < 2) {
            printf("---+---+---\n"); // Print horizontal grid lines
        }
    }
    printf("\n");
}

// Function to check if a player has won
int check_winner() {
    // Check rows, columns, and diagonals
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ') {
            return 1; // Winner found
        }
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ') {
            return 1; // Winner found
        }
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ') {
        return 1; // Winner found
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ') {
        return 1; // Winner found
    }
    return 0; // No winner
}

// Function to check if the board is full (draw condition)
int check_draw() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                return 0; // Board is not full
            }
        }
    }
    return 1; // Draw condition
}

// Handle client connection and gameplay
void handle_gameplay(int client_sock) {
    char message[MAX_BUF];
    int row, col;
    char symbol = currentPlayer == 1 ? 'X' : 'O';

    // Send the current board to the client
    print_board();
    sprintf(message, "Player %d's turn. Your symbol is '%c'.\nEnter your move (row col): ", currentPlayer, symbol);
    send(client_sock, message, strlen(message) + 1, 0);

    // Receive the move from the client
    if (recv(client_sock, message, MAX_BUF, 0) <= 0) {
        perror("Error receiving move");
        return;
    }
    sscanf(message, "%d %d", &row, &col);

    // Check if the move is valid
    if (board[row][col] == ' ') {
        board[row][col] = symbol; // Make the move
        if (check_winner()) {
            sprintf(message, "Player %d wins!\n", currentPlayer);
            send(client_sockets[0], message, strlen(message) + 1, 0);
            send(client_sockets[1], message, strlen(message) + 1, 0);
            return;
        } else if (check_draw()) {
            sprintf(message, "It's a draw!\n");
            send(client_sockets[0], message, strlen(message) + 1, 0);
            send(client_sockets[1], message, strlen(message) + 1, 0);
            return;
        }
        currentPlayer = (currentPlayer == 1) ? 2 : 1; // Switch players
        handle_gameplay(client_sockets[currentPlayer - 1]);
    } else {
        sprintf(message, "Invalid move. Try again.\n");
        send(client_sock, message, strlen(message) + 1, 0);
        handle_gameplay(client_sock);
    }
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;

    // Create Unix domain socket
    if ((server_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    // Bind the socket to the address
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 2) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening...\n");

    // Accept two clients (players)
    client_len = sizeof(client_addr);
    client_sockets[0] = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sockets[0] == -1) {
        perror("Client 1 connection failed");
        exit(EXIT_FAILURE);
    }
    printf("Player 1 connected.\n");

    client_sockets[1] = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sockets[1] == -1) {
        perror("Client 2 connection failed");
        exit(EXIT_FAILURE);
    }
    printf("Player 2 connected.\n");

    // Initialize the board
    memset(board, ' ', sizeof(board));

    // Start the game
    handle_gameplay(client_sockets[0]);

    // Close the server socket
    close(server_sock);
    unlink(SOCKET_PATH);

    return 0;
}