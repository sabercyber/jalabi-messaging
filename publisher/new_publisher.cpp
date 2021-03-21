/*
 * publisher.cpp
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
#include <socket_server.h>

using namespace std;





// ditto application as the client, except does not have to wait for response


int main(int argc, char **argv) {

	// setup epoch time for messages to be tagged for chronologically
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    auto epoch = now.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    int64_t duration = value.count();
    time_data local(duration);


  if (argc > 3)
   {
	  message msg(argv[1]);
	  for(int i =0; i < 7 ;i++)
	  {
	     cout << "Item  " << i << " : " <<  argv[i] << std::endl;
	  }
      Socket  pub_sock(argv[6],atoi(argv[4]), argv[5]);
    
     pub_sock.init();
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

            // TODO: Currently limiting the size of blob to 100 bytes
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
        bool ok = pub_sock.send_data(pw.packetData(), pw.packetSize());
         cout << "Client: "<< argv[1]  << ok << "\n";



  }
  else {
    cout << "syntax:  <msg id> <type> <msg> \n  : /foo/base 5  \"Hi there stupid\"\n";
  }
}

