#pragma once
#include "SocketBase.hpp"




class TcpSocket:public SocketBase
{
public:
   
    TcpSocket(/* args */)=default;
    ~TcpSocket()=default;

    TcpSocket(const TcpSocket&)=delete;
    TcpSocket& operator=(const TcpSocket&)=delete;


    TcpSocket(TcpSocket&&)noexcept = default;
    TcpSocket& operator=(TcpSocket&&) noexcept = default;

    bool connect(std::u8string ip,std::u8string port);
    bool send(const char*  msg,size_t size,size_t& transmitted);
    bool recv(char* recvbuf,size_t size,size_t& received);
};