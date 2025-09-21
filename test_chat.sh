#!/bin/bash

# Test script for the chat server/client

echo "Starting server in background..."
./build/lab7/Debug/lab7-q-server localhost 9086 &
SERVER_PID=$!
sleep 1

echo "Starting first client in background..."
(echo "Hello from client 1"; sleep 1) | ./build/lab7/Debug/lab7-q-client localhost 9086 &
CLIENT1_PID=$!

echo "Starting second client..."
(echo "Hello from client 2"; sleep 1) | ./build/lab7/Debug/lab7-q-client localhost 9086 &
CLIENT2_PID=$!

# Wait a bit for messages to be exchanged
sleep 3

echo "Cleaning up..."
kill $SERVER_PID $CLIENT1_PID $CLIENT2_PID 2>/dev/null
wait $SERVER_PID $CLIENT1_PID $CLIENT2_PID 2>/dev/null

echo "Test complete!"
