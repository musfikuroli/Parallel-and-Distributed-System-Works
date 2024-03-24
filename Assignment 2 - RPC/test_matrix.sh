#!/bin/bash

# Compile the server and client programs if not compiled already
make all

# Start the server in the background
./matrix_server &

# Wait for a second to ensure the server is up and running
sleep 1

# Test matrix addition-----------------------------------------------------------------------------------------------
echo -e "\nTesting Matrix Addition: 2x2"
./matrix_client localhost <<EOF
1
2 2
1 2
3 4
1 2
3 4
EOF

echo -e "\nTesting Matrix Addition: 3x3"
./matrix_client localhost <<EOF
1
3 3
1 2 3
4 5 6
7 8 9
1 2 3
4 5 6
7 8 9
EOF

# Test Matrix Multiplication------------------------------------------------------------------------------------------
echo -e "\nTesting Matrix Multiplication: 2x2 * 2x3"
./matrix_client localhost <<EOF
2
2 2
1 2
3 4
2 3
5 6 7
8 9 10
EOF

#Test Matrix Inverse--------------------------------------------------------------------------------------------------
echo -e "\nTesting Matrix Inverse: 2x2 (Exists)"
./matrix_client localhost <<EOF
3
2
4 7
2 6
EOF

echo -e "\nTesting Matrix Inverse: 3x3 (Exists)"
./matrix_client localhost <<EOF
3
3
1 -3 7
-1 4 -7
-1 3 -6
EOF

echo -e "\nTesting Matrix Inverse: 2x2 (Not Exists)"
./matrix_client localhost <<EOF
3
2
3 4
6 8
EOF

echo -e "\nTesting Matrix Inverse: 3x3 (Not Exists)"
./matrix_client localhost <<EOF
3
3
1 2 3
4 5 6
7 8 9
EOF

# Test Matrix Transpose-----------------------------------------------------------------------------------------------
echo -e "\nTesting Matrix Transpose: 3x3"
./matrix_client localhost <<EOF
4
3 3
1 2 3
4 5 6
7 8 9
EOF

echo -e "\nTesting Matrix Transpose: 4x4"
./matrix_client localhost <<EOF
4
4 4
1 2 3 4
5 6 7 8
9 10 11 12
13 14 15 16
EOF

# Terminate the server gracefully
pkill matrix_server

# Clean up compiled files
make clean
