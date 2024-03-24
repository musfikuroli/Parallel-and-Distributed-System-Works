#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <vector>
#include <ctime>
#include <algorithm>
#include <thread>

using namespace std;

const int PORT = 8080;
const int MAX_CLIENTS = 5;

vector<pair<string, string>> jokes;

void loadJokesFromFile(vector<pair<string, string>> &jokes)
{
    ifstream file("jokes.txt");
    if (!file)
    {
        cerr << "Error opening jokes.txt file" << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        size_t delimiterPos = line.find(',');
        if (delimiterPos != string::npos)
        {
            string setup = line.substr(0, delimiterPos);
            string punchline = line.substr(delimiterPos + 1);
            jokes.emplace_back(setup, punchline);
        }
    }

    file.close();
}

void handleClient(int clientSocket)
{
    vector<int> jokeIndices(jokes.size());

    for (vector<pair<string, string>>::size_type i = 0; i < jokes.size(); ++i)
    {
        jokeIndices[i] = i;
    }
    random_shuffle(jokeIndices.begin(), jokeIndices.end());

    for (vector<pair<string, string>>::size_type i = 0; i < jokes.size(); ++i)
    {
        int randomIndex = jokeIndices[i];
        string jokeSetup = jokes[randomIndex].first;
        string jokePunchline = jokes[randomIndex].second;

        // Send "Knock knock" to the client
        string knockMsg = "Knock knock!";
        send(clientSocket, knockMsg.c_str(), knockMsg.size(), 0);

        // Receive "Who's there?" from the client
        char buffer[1024] = {0};
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer
        recv(clientSocket, buffer, sizeof(buffer), 0);
        string clientResponse = buffer;
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer

        // Handling Case Sensitivity of "Who's there?" -- Converting to Lowercase
        transform(clientResponse.begin(), clientResponse.end(), clientResponse.begin(), ::tolower);

        if (clientResponse != "who's there?")
        {
            // If not, send the appropriate response and wait for another response
            string response = "You are supposed to say, “Who’s there?”. Let’s try again.";
            send(clientSocket, response.c_str(), response.size(), 0);
            --i; // Decrement i so that the same joke is shown again

            // Receive a dummy text for response separation...
            recv(clientSocket, buffer, sizeof(buffer), 0);
            clientResponse = buffer;
            memset(buffer, 0, sizeof(buffer)); // Clear the buffer

            continue;
        }

        // Send the joke setup to the client. Ex: "Cow"
        send(clientSocket, jokeSetup.c_str(), jokeSetup.size(), 0);

        // Receive client's response. Ex: "Cow who?"
        recv(clientSocket, buffer, sizeof(buffer), 0);
        clientResponse = buffer;
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer

        // Handling Case Sensitivity of "Cow who?" -- Converting to Lowercase
        string caseSensitivity = jokeSetup;
        transform(clientResponse.begin(), clientResponse.end(), clientResponse.begin(), ::tolower);
        transform(caseSensitivity.begin(), caseSensitivity.end(), caseSensitivity.begin(), ::tolower);

        // Determine the server's response. Ex: "Cow the Goat."
        string serverResponse;
        if (clientResponse == (caseSensitivity + " who?"))
        {
            serverResponse = jokePunchline;
            // Send the server's response to the client
            send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);

            // Receive a dummy text for response separation...
            recv(clientSocket, buffer, sizeof(buffer), 0);
            clientResponse = buffer;
            memset(buffer, 0, sizeof(buffer)); // Clear the buffer

            // Send the Another One asking message to the client
            if (i < jokes.size() - 1)
            {
                string anotherOne = "Would you like to listen to another? (Y/N)";
                send(clientSocket, anotherOne.c_str(), anotherOne.size(), 0);

                // Receive client's response (Y/N)
                recv(clientSocket, buffer, sizeof(buffer), 0);
                clientResponse = buffer;
                memset(buffer, 0, sizeof(buffer)); // Clear the buffer

                transform(clientResponse.begin(), clientResponse.end(), clientResponse.begin(), ::tolower);

                if (clientResponse == "n")
                {
                    break;
                }
            }
        }
        else
        {
            serverResponse = "You are supposed to say '" + jokeSetup + " who?'. Let’s try again.";
            // Send the server's response to the client
            send(clientSocket, serverResponse.c_str(), serverResponse.size(), 0);

            // Receive a dummy text for response separation...
            recv(clientSocket, buffer, sizeof(buffer), 0);
            clientResponse = buffer;
            memset(buffer, 0, sizeof(buffer)); // Clear the buffer

            --i; // Decrement i so that the same joke is not shown again
        }
    }

    // Send "No more joke to show" and close the client socket
    string noMoreJokes = "No more joke to show.";
    send(clientSocket, noMoreJokes.c_str(), noMoreJokes.size(), 0);

    close(clientSocket);
}

int main()
{
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    vector<thread> threadList;

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    // Set the SO_REUSEADDR socket option
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        cerr << "Error setting socket options" << endl;
        return 1;
    }

    // Initialize the server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cerr << "Error binding socket" << endl;
        return 1;
    }

    // Start listening for client connections
    if (listen(serverSocket, MAX_CLIENTS) < 0)
    {
        cerr << "Error listening" << endl;
        return 1;
    }

    cout << "Server listening on port " << PORT << endl;

    loadJokesFromFile(jokes);

    int clientCount = 0;

    while (clientCount != 5)
    {
        // Accept a client connection
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket < 0)
        {
            cerr << "Error accepting connection" << endl;
            continue;
        }

        // Create a new thread to handle the client
        threadList.emplace_back(handleClient, clientSocket);
        cout << "Accepted connection from " << inet_ntoa(clientAddr.sin_addr) << endl;
        clientCount++;
    }

    // Wait for all threads/clients to finish before exiting
    for (auto &t : threadList)
    {
        t.join();
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
