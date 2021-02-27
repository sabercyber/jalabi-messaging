/*
 * app.cpp
 *
 *  Created on: 14-Feb-2021
 *      Author: Karthik Rao
 */







#define OSTREAM_OUTPUT
#include <iostream>
#include "../inc/udp.h"
#include <vector>
#include <string>
#include <map>
#include <errno.h>

//#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <arpa/inet.h>

#include "../inc/message.h"
#include "../inc/packet_reader.h"
#include "../inc/packet_writer.h"
#include "../inc/utilities.h"





using namespace std;


const int PORT_NUM = 9109;
extern void send_packet_web(std::string data);

void runServer() {
	//make_json();
  UdpSocket sock;
  typedef std::vector<SockAddr> socket_details;

  std::map<std::string, socket_details> sub_list;
  std::map<std::string, socket_details>::iterator list_itr ;

  sock.bindTo(PORT_NUM);
  if (!sock.isOk()) {
    cerr << "Error opening port " << PORT_NUM << ": " << sock.errorMessage() << "\n";
  } else
  {
    cout << "Server started, will listen to packets on port " << PORT_NUM << std::endl;
    packet_reader pr;
    packet_writer pw;
    while (/*sock.isOk()*/1) {
      if (sock.receiveNextPacket(30 /* timeout, in ms */)) {
        pr.init(sock.packetData(), sock.packetSize());
        message *msg;
        while (pr.isOk() && (msg = pr.popMessage()) != 0) {
          int iarg, iarg2;
          std::string data_msg;
          std::string _recvd_sub_list;
          std::vector<char> v_data;
          if (msg->match("/ping").popInt32(iarg).isOkNoMoreArgs()) {
            cout << "Server: received /ping " << iarg << " from " << sock.packetOrigin() << "\n";
           message repl; repl.init("/pong").pushInt32(iarg+1);
            pw.init().addMessage(repl);
            sock.sendPacketTo(pw.packetData(), pw.packetSize(), sock.packetOrigin());
          }


          else if (msg->match("/subscribe").popStr(_recvd_sub_list).isOkNoMoreArgs()) {
        	  SockAddr &local_data = sock.packetOrigin();


        	  if (sub_list.size() == 0){
        		  list_itr = sub_list.begin();
        		  socket_details local_sock;
        		  local_sock.push_back(local_data);
        		  sub_list.insert(list_itr, std::pair <std::string, socket_details>(_recvd_sub_list,local_sock));
        		 // cout << "added first list" << endl;
        	  }
        	  else {
        		  list_itr = sub_list.find(_recvd_sub_list);
        		  if(list_itr != sub_list.end()){
        			  //sub_list.insert(list_itr, std::pair <std::string, socket_details>(_recvd_sub_list,local_sock));
        			  list_itr->second.push_back(local_data);
        			 // cout << "added found the topic list" << endl;

        		  }

        		  else {
        			  //list_itr++;
        			  //local_sock.push_back(local_data);
        			  list_itr = sub_list.end();
        			  socket_details local_sock;
        			  local_sock.push_back(local_data);
        			  list_itr = sub_list.emplace_hint(list_itr, std::pair <std::string, socket_details>(_recvd_sub_list,local_sock));
        			  //list_itr->second.push_back(local_data);
        			  //cout << "new topic" << endl;
        		  }


        	  }



        	  std::map<std::string, socket_details>::iterator Local_test_1;
        	  for(Local_test_1 = sub_list.begin(); Local_test_1 !=sub_list.end(); Local_test_1++)
        	  {
        		  cout << "Topic : "  << Local_test_1->first << std::endl;
        		  std::vector<SockAddr>::iterator itr_lst;
        		  cout << "------ List of subscribers ----------" <<  std::endl;
        		  for( itr_lst = Local_test_1->second.begin(); itr_lst !=( Local_test_1->second.end()); itr_lst++ )
        		  {
        			  cout << "   "<<*itr_lst << std::endl;
        		  }
        	  }
        	  cout << " Sub topic : " << _recvd_sub_list << " Origin : " << sock.packetOrigin() << std::endl;
        	  _recvd_sub_list.clear();
          }

          // subscription dispatching activity

          std::map<std::string, socket_details>::iterator subscription_topic_list;
          for(subscription_topic_list = sub_list.begin(); subscription_topic_list !=sub_list.end(); subscription_topic_list++)
		  {
			  cout << "Topic : "  << subscription_topic_list->first << std::endl;
			  string temp = subscription_topic_list->first;

			  int last = temp.find(">");
			  string strNew = temp.substr (1,last-1);
			  //cout << "Topic for search : "  << strNew << std::endl;
			  if (msg->match(strNew)) {


				  std::vector<SockAddr>::iterator itr_lst;
				 // cout << "------ List of subscribers ----------" <<  std::endl;
				  for( itr_lst = subscription_topic_list->second.begin(); itr_lst !=( subscription_topic_list->second.end()); itr_lst++ )
				  {
					  sock.sendPacketTo(sock.packetData(), sock.packetSize(),*itr_lst);
					  cout << "Sent data to :    "<< subscription_topic_list->first <<   "located at : " << *itr_lst << std::endl;
				  }
			  }
		  }
          // end of subscription dispatching activity

          // special operation JSON and sending to web
          std::string web_data;
          if (msg->match("/web").popStr(web_data).isOkNoMoreArgs())
          {
			  //cout << " Message: /web" << "Data: " << web_data << std::endl;
			  send_packet_web(web_data);
          }

#if 0

          if (msg->match("/home/light1").popInt32(iarg).isOkNoMoreArgs()) {
			  cout << " Message: /home/light 1" << "Data: " << iarg << std::endl;

		   }



          else if (msg->match("/home/light2").popInt32(iarg).isOkNoMoreArgs()) {
                         cout << " Message: /home/light2 " << "Data: " << iarg << std::endl;

                      }

          else if (msg->match("/home/fan1").popInt32(iarg).popInt32(iarg2).isOkNoMoreArgs())
           {
              cout << " Message: /home/fan1 " << "Data1: " << iarg <<  "Data2: " << iarg2 << std::endl;

           }

          else if (msg->match("/foo/base").popStr(data_msg).isOkNoMoreArgs())
           {
              //sub_list["/foo/base"]= sock.packetOrigin();
              cout << " Message: /foo/base" << "Data1: " << data_msg << std::endl;

           }

          else  if (msg->match("/Bangalore/data").popBlob(v_data).isOkNoMoreArgs())
           {


               cout << " Message: /Bangalore/data " << "Data1: " <<  std::endl;
               //std::map<std::string, socket_details>::iterator Local_test_1 = sub_list.begin();
              // cout << "message origin : " << Local_test_1->second[0]   ;

               //message repl; repl.init("/Bangalore/data").pushInt32(64);
               // pw.init().addMessage(repl);
               // sock.sendPacketTo(pw.packetData(), pw.packetSize(), Local_test_1->second[0]);

              std::vector<char>::iterator it;
              it = v_data.begin() ;
              for(it = v_data.begin() ; it != v_data.end();it++)
                cout << *it;
                cout << std::endl;

           }

#endif

#ifdef Debug_Server_message

            cout << "Server:  message: " << *msg << "\n";
#endif

        }
      }
    }
  }
}



int main(int argc, char **argv) {

    runServer();

}
