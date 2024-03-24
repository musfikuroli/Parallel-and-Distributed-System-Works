#!/bin/bash

# Compile the server and client programs if not compiled already
make all

# Start the server in the background
./matrix_server &

# Wait for a second to ensure the server is up and running
sleep 1

echo -e "\nTesting Matrix Inverse: 3x3 (Exists)"
./matrix_client localhost <<EOF
3
3
1 -3 7
-1 4 -7
-1 3 -6
EOF

echo -e "\nTesting Matrix Inverse: 3x3 (Not Exists)"
./matrix_client localhost <<EOF
3
3
1 2 3
4 5 6
7 8 9
EOF

# Terminate the server gracefully
pkill matrix_server

# Clean up compiled files
make clean
