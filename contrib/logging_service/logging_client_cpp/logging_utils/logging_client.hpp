/*
 *  logging_client.hpp
 *  
 *
 *  Created by Alexandra Carpen-Amarie on 11/12/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef LOGGING_CLIENT_H
#define LOGGING_CLIENT_H

#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

enum log_level {
    DEBUG,    
    ERROR,
    WARNING,
    INFO
};


class logging_client {
    
private:
    boost::asio::io_service io_service;
    tcp::resolver resolver;
    tcp::socket socket;
    std::string server_host;
    std::string server_port;

    std::string format_message(log_level l, std::string message);
    std::string get_host();
    
public:
    logging_client(const char* conf_file);
    logging_client(std::string host, std::string port);
    ~logging_client();
    int log(log_level l, std::string message);
    void connect();
    
};

#endif
