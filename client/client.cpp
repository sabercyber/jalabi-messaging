/*
 * client.cpp
 *
 *  Created on: 14-Feb-2021
 *      Author: Karthik Rao
 */



#define OSTREAM_OUTPUT
#include <iostream>
#include <vector>
#include <string>

#include "../inc/message.h"
#include "../inc/packet_reader.h"
#include "../inc/packet_writer.h"
#include "../inc/utilities.h"
#include "../inc/udp.h"


using namespace std;


const int PORT_NUM = 9109;



void new_publish(int type){

    //Message msg(int type, string)


}

void publish()
{
    UdpSocket sock;
    sock.connectTo("localhost", PORT_NUM);
  if (!sock.isOk()) {
    cerr << "Error connection to port " << PORT_NUM << ": " << sock.errorMessage() << "\n";
  } else {
    cout << "Client started, will send packets to port " << PORT_NUM << std::endl;


#if 0
    // keeping message declarations for references
     int iping = 1;



      message msg("/ping"); msg.pushInt32(iping);

      message msg_light1("/home/light1");msg_light1.pushInt32(1);

      message msg_fan1("/home/fan1");  msg_fan1.pushInt32(1).pushInt32(75);

      packet_writer pw;
      pw.startBundle().startBundle().addMessage(msg_light1).endBundle().endBundle();
      bool ok = sock.sendPacket(pw.packetData(), pw.packetSize());
      cout << "Client: sent //home/light1 : ok=" << ok << "\n";

      packet_writer pw2;
      pw2.startBundle().startBundle().addMessage(msg).endBundle().endBundle();
      ok = sock.sendPacket(pw2.packetData(), pw2.packetSize());
      cout << "Client: sent /ping " << iping++ << ", ok=" << ok << "\n";

      packet_writer pw3;

      pw3.startBundle().startBundle().addMessage(msg_fan1).endBundle().endBundle();
      ok = sock.sendPacket(pw3.packetData(), pw3.packetSize());
      cout << "Client: sent //home/fan1: ok=" << ok << "\n";


#endif


      // Hold on to the socket to continously listen for incoming subscriptin messages
      if (sock.receiveNextPacket(30 /* timeout, in ms */)) {
    	  packet_reader pr(sock.packetData(), sock.packetSize());
        message *incoming_msg;


        while (pr.isOk() && (incoming_msg = pr.popMessage()) != 0) {
         cout << "Client: received " << *incoming_msg << "\n";
        }
      }
    }
    //cout << "sock error: " << sock.errorMessage() << " -- is the server running?\n";
      sock.close_socket();
}




int main(int argc, char **argv) {
     std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    auto epoch = now.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    int64_t duration = value.count();
    time_data local(duration);


  if (argc > 3) {
	  message msg(argv[1]);
	  for(int i =0; i < 4 ;i++)
	  {
	     cout << "Item  " << i << " : " <<  argv[i] << std::endl;
	  }
      UdpSocket sock;
      sock.connectTo("localhost", PORT_NUM);
     if (!sock.isOk()) {
         cerr << "Error connection to port " << PORT_NUM << ": " << sock.errorMessage() << "\n";
     }
     switch (atoi(argv[2])) {
          case 0: msg.pushBool(atoi(argv[3])); break;
          case 1: msg.pushInt32(atoi(argv[3])); break;
          case 2: msg.pushInt64(atoi(argv[3])); break;
          case 3: msg.pushFloat(std::stof(argv[3])); break;
          case 4: msg.pushDouble(std::stod(argv[3])); break;
          case 5: {
            std::string s = argv[3];
            if (s.size()>1) { s[0] = '<'; s[s.size()-1] = '>'; }
            msg.pushStr(s);
          } break;
          case 6: {
            std::vector<std::string> b ;
            b.push_back(argv[3]);
            char  test[100];
            strcpy(test, argv[3]);
            std::vector<std::string>::iterator it = b.begin();

           // b.resize(oscpkt::storage_alloaction, char(0x55));
            //if (b.size() > 1) { b.front() = 0x44; b.back() = 0x66; }
            msg.pushBlob(test, 100);
          } break;
        }
        packet_writer pw;
        pw.startBundle().startBundle().addMessage(msg).endBundle().endBundle();
        bool ok = sock.sendPacket(pw.packetData(), pw.packetSize());
         cout << "Client: "<< argv[1]  << ok << "\n";

         // wait for a reply ?
//               if (sock.receiveNextPacket(300 /* timeout, in ms */)) {
//                 packet_reader pr(sock.packetData(), sock.packetSize());
         	 while (sock.isOk()) {
         		packet_reader pr;
               if (sock.receiveNextPacket(30 /* timeout, in ms */)) {
                 pr.init(sock.packetData(), sock.packetSize());
                 //message *msg;
                 message *incoming_msg;


                 while (pr.isOk() && (incoming_msg = pr.popMessage()) != 0) {
                   cout << "Client: received " << *incoming_msg << "\n";
                 }
               }
         	 }

  }
  else {
    cout << "syntax:  <msg id> <type> <msg> \n  : /foo/base 5  \"Hi there stupid\"\n";
  }
}

