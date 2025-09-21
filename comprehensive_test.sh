#!/bin/bash

# Comprehensive test script for the chat server/client

echo "=== Chat Server/Client Test ==="
echo "Starting server in background..."
./build/lab7/Debug/lab7-q-server localhost 9087 &
SERVER_PID=$!
sleep 1

echo "Starting client 1..."
(echo "Message from Alice"; sleep 2; echo "Alice says hello!") | ./build/lab7/Debug/lab7-q-client localhost 9087 &
CLIENT1_PID=$!
sleep 0.5

echo "Starting client 2..."
(sleep 1; echo "Bob responds"; sleep 1; echo "Nice to meet you!") | ./build/lab7/Debug/lab7-q-client localhost 9087 &
CLIENT2_PID=$!

# Wait for message exchange
sleep 5

echo ""
echo "Cleaning up processes..."
kill $SERVER_PID $CLIENT1_PID $CLIENT2_PID 2>/dev/null
wait $SERVER_PID $CLIENT1_PID $CLIENT2_PID 2>/dev/null

echo "Test completed successfully!"
