#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <algorithm>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>

using namespace std;

const char *SERVER_IP = "127.0.0.1"; // Replace with the server IP address
const int PORT = 8080;

int main()
{
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create a socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    // Initialize the server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cerr << "Error connecting to server" << endl;
        return 1;
    }

    cout << "Connected to server" << endl;

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer)); // Clear the buffer

    while (true)
    {
        // Server: Knock knock!
        recv(clientSocket, buffer, sizeof(buffer), 0);
        string serverResponse = buffer;
        cout << "Server: " + serverResponse << endl;
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer

        // Client: Who's there?
        string userInput;
        printf("Client: ");
        getline(cin, userInput);

        // Send the data to the server
        send(clientSocket, userInput.c_str(), userInput.size(), 0);

        string checkInput = userInput.c_str();

        // Handling Case Sensitivity of "Who's there?" -- Converting to Lowercase
        transform(checkInput.begin(), checkInput.end(), checkInput.begin(), ::tolower);

        // Server: You are supposed to say, “Who’s there?”. Let’s try again.
        if (checkInput != "who's there?")
        {
            recv(clientSocket, buffer, sizeof(buffer), 0);
            serverResponse = buffer;
            cout << "Server: " + serverResponse << endl;
            memset(buffer, 0, sizeof(buffer)); // Clear the buffer

            // Send a dummy text for response seperation...
            string knockMsg = " ";
            send(clientSocket, knockMsg.c_str(), knockMsg.size(), 0);

            continue;
        }

        // Server: Cow.
        recv(clientSocket, buffer, sizeof(buffer), 0);
        serverResponse = buffer;
        cout << "Server: " + serverResponse << endl;
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer

        // Client: Cow who?
        printf("Client: ");
        getline(cin, userInput);

        // Send the data to the server
        send(clientSocket, userInput.c_str(), userInput.size(), 0);
        checkInput = userInput.c_str();

        // Handling Case Sensitivity of "Cow who?" -- Converting to Lowercase
        string caseSensitivity = serverResponse;
        transform(checkInput.begin(), checkInput.end(), checkInput.begin(), ::tolower);
        transform(caseSensitivity.begin(), caseSensitivity.end(), caseSensitivity.begin(), ::tolower);

        // Server: You are supposed to say, “Cow who?”. Let’s try again.
        if (checkInput != caseSensitivity + " who?")
        {
            recv(clientSocket, buffer, sizeof(buffer), 0);
            serverResponse = buffer;
            cout << "Server: " + serverResponse << endl;
            memset(buffer, 0, sizeof(buffer)); // Clear the buffer

            // Send a dummy text for response seperation...
            string knockMsg = " ";
            send(clientSocket, knockMsg.c_str(), knockMsg.size(), 0);

            continue;
        }

        // Server: Cow is the GOAT.
        recv(clientSocket, buffer, sizeof(buffer), 0);
        serverResponse = buffer;
        cout << "Server: " + serverResponse << endl;
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer

        // Send a dummy text for response separation...
        string knockMsg = " ";
        send(clientSocket, knockMsg.c_str(), knockMsg.size(), 0);

        // Server: Would you like to listen to another? (Y/N)
        recv(clientSocket, buffer, sizeof(buffer), 0);
        serverResponse = buffer;
        cout << "Server: " + serverResponse << endl;
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer

        // Closing client if there is no more joke in the database
        if (serverResponse == "No more joke to show.")
        {
            break;
        }

        // Client: Y/N
        printf("Client: ");
        getline(cin, userInput);

        // Send the data to the server
        send(clientSocket, userInput.c_str(), userInput.size(), 0);
        checkInput = userInput.c_str();
        transform(checkInput.begin(), checkInput.end(), checkInput.begin(), ::tolower);

        if (checkInput == "y")
        {
            continue;
        }
        else
            break;
    }

    // Close the client socket
    close(clientSocket);

    return 0;
}