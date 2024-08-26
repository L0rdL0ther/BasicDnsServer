#include <iostream>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 53
#define MAXLINE 1024

void create_dns_response(const char* request, char* response, int& response_length) {
    // Extract Transaction ID from request
    unsigned short transaction_id = (request[0] << 8) | request[1];

    // DNS Header
    unsigned char header[] = {
        (unsigned char)(transaction_id >> 8), (unsigned char)(transaction_id & 0xff), // Transaction ID
        0x81, 0x80, // Flags: Standard query response, no error
        0x00, 0x01, // Questions: 1
        0x00, 0x01, // Answer RRs: 1
        0x00, 0x00, // Authority RRs: 0
        0x00, 0x00  // Additional RRs: 0
    };

    // DNS Question Section (same as in request)
    const unsigned char question[] = {
        0x07, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, // example
        0x03, 0x63, 0x6f, 0x6d, // com
        0x00, // End of name
        0x00, 0x01, // Type: A (IPv4 address)
        0x00, 0x01  // Class: IN (Internet)
    };

    // DNS Answer Section
    const unsigned char answer[] = {
        0x07, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, // example
        0x03, 0x63, 0x6f, 0x6d, // com
        0x00, // End of name
        0x00, 0x01, // Type: A (IPv4 address)
        0x00, 0x01, // Class: IN (Internet)
        0x00, 0x00, 0x00, 0x3c, // TTL: 60 seconds
        0x00, 0x04, // Data length: 4 bytes
        0x5d, 0xb8, 0xd8, 0x22  // IP Address: 93.184.216.34
    };

    // Construct the response
    response_length = sizeof(header) + sizeof(question) + sizeof(answer);
    memcpy(response, header, sizeof(header));
    memcpy(response + sizeof(header), question, sizeof(question));
    memcpy(response + sizeof(header) + sizeof(question), answer, sizeof(answer));
}


int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in server_addr, client_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "DNS Server listening on port 53" << std::endl;

    while (true) {
        socklen_t len = sizeof(client_addr);
        int n = recvfrom(sockfd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&client_addr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }

        std::cout << "Received DNS request from client" << std::endl;

        // Print received request (for debugging)
        std::cout << "DNS Request Content:" << std::endl;
        for (int i = 0; i < n; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)buffer[i] << " ";
            if ((i + 1) % 16 == 0) std::cout << std::endl;
        }
        std::cout << std::dec << std::endl;

        // Prepare DNS response
        char response[MAXLINE];
        int response_length;
        create_dns_response(buffer, response, response_length);

        // Print the response for debugging
        std::cout << "DNS Response Content:" << std::endl;
        for (int i = 0; i < response_length; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)response[i] << " ";
            if ((i + 1) % 16 == 0) std::cout << std::endl;
        }
        std::cout << std::dec << std::endl;

        // Send DNS response
        sendto(sockfd, response, response_length, MSG_CONFIRM, (const struct sockaddr*)&client_addr, len);
        std::cout << "DNS response sent to client" << std::endl;
    }

    close(sockfd);
    return 0;
}