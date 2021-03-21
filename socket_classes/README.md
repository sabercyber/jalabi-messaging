# socket_classes
Abstraction of socket into c++ classes

# Purpose
I needed a abstration of sockets in c++ that was simple to use without the pain of handling depencies. I do not doubt there are such abstrations already available. This is created as my requirement to just start up operation on udp or tcp and then marry additional layers for data serialization on top of it as I would need them. 

# Overall Design
![System Design](https://res.cloudinary.com/sabercyber/image/upload/v1615046055/socket_classes/system_view_erbrbo.bmp)
On top of standard Linux support and existing socket call, a layered wrapper set of classes is created. 
Socket class : abstracts the absolute rudimentary socket calls to publish data on the socket pipe. UDP and TCP is supported
Socket_server class: Extends the Socket class to add server class like socket calls of  bind(),listen() and accept() for TCP operations and bind(), receivefrom() and sendto for UDP.
In the Socket server class it is possible to have the following operations
  1. TCP server on a port specified by you
  2. UDP server on a port specified by you
  3. TCP and UDP on a port specified by you. Current you can specify different ports for TCP and UDP. 

# Code Implementation Summary
<p align="center">
  <img src ="https://res.cloudinary.com/sabercyber/image/upload/v1615046056/socket_classes/code_socket_class_usvepr.bmp">
</p>

# Compilation and Usage
## Code Organization 
    -->app : has application that use the class created
    -->inc : header files
    -->src : implementation files 
### Compile for Socket application for publishing message
in the app folder
```
g++ client_app.cpp ../src/*.cpp -o client_app -I ../inc
```

### Compile for Socket server application for handling messages
in the app folder
```
g++ server_app.cpp ../src/*.cpp -o server_app -I ../inc
```

![operation](https://res.cloudinary.com/sabercyber/image/upload/v1615046054/socket_classes/execution_ubaidi.png)
