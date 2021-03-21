/****************************************************************************
 * Copyright (C) 2021 by Karthik Rao (sabercyber)                                  *
 *                                                                          *
 * This file is part of C++ socket classes .                                                *
 *                                                                          *
 *   MIT License



Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 ****************************************************************************/

/**
 * @file server_app.cpp
 * @author karthik rao
 * @date 05 March 2021
 * @brief File implements application from Socket_server class. A simple set of 3 lines of code 
 * gets the server opration going.
 * command line examples
 * For TCP only opreation   : server_app 5000 tcp
 * For UDP only opreation   : server_app 5000 udp
 * For TCP & UDP opreation  : server_app 5000 tcp__and_udp
 * select server port needed
 *     
 */




#include "socket_server.h"

int main(int argc, char* argv[])
{
    std::string test_data ;
    if (argc != 3)
    {
        std::cout << " Incorrect Syntax: <app> port tcp/upd/tcp_and_udp"<<std::endl;
    }
    else
    {
        Socket_server myserver(atoi(argv[1]),argv[2]);
        std::cout << " about to call run_server" << std::endl;
        myserver.send_data("Hi there from server");
        myserver.server_init();
        while(1)
        {
            myserver.run_server();
            test_data = myserver.packetData();
            std::cout << "Data at the top : " << test_data << std::endl;
        }
    }
    return 0;
}
