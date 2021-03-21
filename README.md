# jalabi-messaging
New messaging services for a new world 

# Objective
The objective for creating this new messaging framework is to have simplistic, developer centric messaging service that is able to connect a c++ process running a constrained hardware like a microcontroller to a front end application such as a browser. It also aims to link multiple end point via the publish subscribe model.
There are multiple frameworks available this has been created the following in mind. 

## Human readable topic
There is need to support publish subscribe methodology with human readable topics. There are large number of frameworks that have complex index based messaging identifier. MQTT solves this problem with string based human readable message identified. MQTT method has been on of the inspration. But why not just use MQTT. The complexity on the configurability that comes with MQTT defeats the purpose we set out for. Simplicity and deployability on a constranted hardware. 

## Simplicity
Simplicity of code based is achived. We have borrowed Open Sound Connect specification while modifying what is required for use to 
  1. Meeting front end display capability like on the browser. Establish a simplistic dashboard
  2. Developer centric syntax for message creation and deployment
  3. scalability 
  4. Small foot print
  5. Fast
  6. Flexibility

## Flexibility 
The design and implementation creates simple message creation and transport pattern. A number of differnt transports will be added, we start with UDP, we will extend it to TCP, Zero MQ, Drivers - to physically access data from devices and publish data out. It would also be possible to control device configuration and setting from the front end. This is seen a useful embedded debgging tool. 

## Fast
This messagging service fast with a server operating at at efficiency 2n. 

## Scalable
Since the creation of the subscription list is dynamic is nature, new services can be added on the fly without server code being updated. 

## Small foot print
The server code is small in memory foot print in < 500KB

## Developer centric message creation
```
      // message class creates a message with topic /ping. The message payload is an interger
      message msg("/ping"); msg.pushInt32(iping);
      // write the data to message store with correct class
      packet_writer pw;
      pw.startBundle().startBundle().addMessage(msg_light1).endBundle().endBundle();
      // send the data
      bool ok = sock.sendPacket(pw.packetData(), pw.packetSize());
```

## Compilation with socket classes
Linux compliation tested
Remember to compile the socket classes library. Follow the instruction from that folder
Remember to export 
### server compilation 
#### at the app folder level
```
g++  new_app.cpp -I ../socket_classes/inc/ -L ../socket_classes/apps/ -lmysocket -o new_server_app -D Debug_Server_message ./new_server_app

# for compilation without debug support
g++  new_app.cpp -I ../socket_classes/inc/ -L ../socket_classes/apps/ -lmysocket -o new_server_app  ./new_server_app

```
operation is just to execute the executable
```
./new_server_app
```
### client compilation 
#### at the client folder
```
g++ new_client.cpp -o new_client -I ../socket_classes/inc/ -L ../socket_classes/apps/ -lmysocket
# for launching 
./new_client "/subscribe" 5 " /karthik " 5000  udp  127.0.0.1

```

### publisher compilation 
#### at the publisher folder
```
g++ new_publisher.cpp -o new_pub -I ../socket_classes/inc/ -L ../socket_classes/apps/ -lmysocket 
# for launching 
./new_pub "/karthik" 5 " Hi There " 5000 tcp 127.0.0.1

```


## Compilation - Depricated


### Server compilation 
For windows
```
// path /app folder
g++ app.cpp -o app -lws2_32
```
For Linux
```
// path /app folder
g++ app.cpp -o app 
```

### Client compilation
This is the subscription application 
For Windows
```
// path /client folder
g++ client.cpp -o sub -lws2_32
```
For Linux
```
// path /client folder
g++ client.cpp -o sub 
```

### Publisher compilation
This is the publisher application 
For Windows
```
// path /publisher folder
g++ publisher.cpp -o pub -lws2_32
```
For Linux
```
// path /publisher folder
g++ publisher.cpp -o pub 
```


