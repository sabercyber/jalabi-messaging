/*
 * client_message.h
 *
 *  Created on: 09-Feb-2021
 *      Author: Chethan Deekshith
 */

//Serialization
#include <iostream>
#include <sstream>

class Message
{
    public:
    int test_int_1;
    std::string test_str_1;
    void test_method(){;}
    private:
    inline friend std::ostream& operator <<(std::ostream& out, Message& obj)
    {
        out<<obj.test_int_1<<" "<<obj.test_str_1;
        return out;
    }
    inline friend std::istream& operator >> (std::istream& in, Message& obj)
    {
        in>>obj.test_int_1;
        in>>obj.test_str_1;
        return in;
    }
};