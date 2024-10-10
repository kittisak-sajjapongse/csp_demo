# Publish-Subscribe
For QT client to receive published TM from ground control server

## Before Running
```
make; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
```

## Running
Publisher (Ground Control Server)
```
./publish
```

Subscriber (Ground Control client)
```
./subscribe
```

# Client-Server
Sending TC from QT client to ground control server

## Before Running
```
make; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
```

## Running
Server (Ground Control Server)
```
./server_respond
```

Client (Ground Control Client)
```
./client_request <CLIENT_ID_TEXT>
```