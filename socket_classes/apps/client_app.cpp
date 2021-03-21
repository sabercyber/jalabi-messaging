
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
 * @file client_app.cpp
 * @author karthik rao
 * @date 05 March 2021
 * @brief File implements application from Socket class. A simple set of 3 lines of code 
 * get the socket operation going.
 * command line examples
 * Sending tcp message: client_app 127.0.0.1 5000 tcp
 * Sending udp message: client_app 127.0.0.1 5000 udp
 * Use the port of the server operation 
 *     
 */


#include "socket.h"

int main(int argc, char* argv[])
{
    if (argc != 5)
    {
        std::cout << "Incorrect syntax : User <app> hostname port tcp or upd" << std::endl;
    }
    else 
    {
        Socket local_socket(argv[1], atoi(argv[2]), argv[3]);
        local_socket.init();
        local_socket.send_data(argv[4]);
        
    }
    

    return 0;
}
