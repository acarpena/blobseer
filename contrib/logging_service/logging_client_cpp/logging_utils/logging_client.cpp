/*
 *  logging_client.cpp
 *  
 *
 *  Created by Alexandra Carpen-Amarie on 11/12/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "logging_client.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "libconfig.h++"

/* 
 *Constructor - initiates the connection to the server represented by the host, port parameters
 */
logging_client::logging_client(std::string host, std::string port):
resolver(io_service),
socket(io_service),
server_host(host),
server_port(port)
{
    
    connect();        
    
}
 

/* 
 *Constructor - initiates the connection to the server represented by the host, port parameters
 *          using the provided configuration file
 */
logging_client::logging_client(const char* conf_file):
resolver(io_service),
socket(io_service)
{
    
    libconfig::Config cfg;
    try {
        cfg.readFile(conf_file);
        if (!(cfg.lookupValue("logging_client.host", server_host)
              && cfg.lookupValue("logging_client.port", server_port) 
              ))
            throw libconfig::ConfigException();
    } catch(libconfig::FileIOException &e) {
        std::cerr<<"I/O exception on config file";
    } catch(libconfig::ParseException &e) {
        std::cerr<<"Parse exception (line " << e.getLine() << "): " << e.getError();
    } catch(...) {
        std::cerr<<"Invalid configuration, check format (run without config file for help)";
    }
    
    connect();        
}


/*
 * Destructor
 */
logging_client::~logging_client() {    
    socket.close();
}


/* 
 *Method that implements the connection to the server
 */
void logging_client::connect() {
    
    boost::asio::ip::tcp::resolver::query query(server_host, server_port);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::ip::tcp::resolver::iterator end;
   
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
        tcp::endpoint ep = *endpoint_iterator++; 
        socket.close();
        socket.connect(ep, error);
    }
 }

/*
 * Method that sends the logging message to the socket through the connection realised 
 * when the object was created
 */
int logging_client::log(log_level l, std::string message) {
    
    std::string formatted_message = format_message(l, message);
    try {
        boost::system::error_code ignored_error;
        boost::asio::write(socket, boost::asio::buffer(formatted_message, formatted_message.length()),  
                           boost::asio::transfer_all(), ignored_error);
    }
    catch (std::exception& e)
    {
        return 1;
    }
    
    return 0;
}

/*
 * Method that formats the message received from the client by adding the logging level and the host name
 * of the node where the client runs on to the logged message.
 */
std::string logging_client::format_message(log_level l, std::string message) {
    
    std::string level;
    std::string s;
    
    switch (l) {
        case DEBUG: {
            level="DEBUG";
            break;
        }
        case ERROR: {
            level="ERROR";
            break;
        }
        case WARNING: {
            level="WARNING";
            break;
        }
        case INFO: {
            level="INFO";
            break;
        }
    }
    
    s = "[" + level + "] [" + get_host() + "] " + message;
    return s;
}


/*
 * Method the returns the full name of the host where the process runs on
 */
std::string logging_client::get_host() {
    
    std::string host;
    struct addrinfo hints, *info, *p;
    int gai_result;
    
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    host = "";
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    
    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
        exit(1);
    }
    
    for(p = info; p != NULL; p = p->ai_next) {
        host += p->ai_canonname; 
        break;
    }
    
    return host;
}


