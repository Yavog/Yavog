#include "SocketPoll.hpp"
#include <thread>
#include <assert.h>

void SocketPoll::add(SocketBase& socket,bool write,bool read){
    auto handle = socket.getHandle();
    POLLFD* p = nullptr;
    if(handle2Index.find(handle) != handle2Index.end()){
        // exists
        int index = handle2Index[handle]; 
        p = &connections.at(index);
    }else{
        // doesn't exists
        POLLFD con;
        con.fd = handle;
        con.events  = 0;
        con.revents = 0;
        handle2Index[handle] = connections.size();

        int index = connections.size();
        connections.push_back(con);
        p = &connections.at(index);
    }
    if(write)
        p->events |= POLLWRNORM;
    if(read)
        p->events |= POLLRDNORM;
    
}
void SocketPoll::remove(SocketBase& socket){
    int handle = socket.getHandle();
    remove(handle);
}
void SocketPoll::remove(int handle){
    if(handle2Index.find(handle)==handle2Index.end())
        return;
    // swap remove
    int index = handle2Index[handle];
    handle2Index.erase(handle);
    std::swap(connections[index],connections[connections.size()-1]);
    connections.pop_back();
    if(connections.size() > index){
        int otherHandle = connections[index].fd;
        handle2Index[otherHandle] = index;
    }
}

void SocketPoll::addWrite   (SocketBase& socket){
    auto handle = socket.getHandle();
    POLLFD* p = nullptr;
    if(handle2Index.find(handle) == handle2Index.end()){
        // doesn't exists
        return add(socket,true,false);
    }
    // exists
    int index = handle2Index[handle]; 
    p = &connections.at(index);
    p->events |= POLLWRNORM;
}
void SocketPoll::removeWrite(SocketBase& socket){
    auto handle = socket.getHandle();
    POLLFD* p = nullptr;
    if(handle2Index.find(handle) == handle2Index.end()){
        // doesn't exists
        return;
    }
    // exists
    int index = handle2Index[handle]; 
    p = &connections.at(index);
    p->events &= ~POLLWRNORM;
}
    

bool SocketPoll::wait(size_t ms){
    //remove all invalid sockets:
    for (size_t i = 0; i < connections.size();){
        auto& con = connections[i];
        if(con.revents & (POLLERR|POLLHUP|POLLNVAL)){
            remove(con.fd);
        }
        else {
            i++;
        }
    }

    //reset everything
    for (auto &&con : connections){
        con.revents = 0;
    }
    
#ifdef __linux__
    if(!connections.size()){
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        return false;
    }
    int v = poll(connections.data(),connections.size(),ms);
#elif WIN32
    if(!connections.size()){
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        return false;
    }
    //WSAPoll requires at least one element in fds[first arg]
    int v = WSAPoll(connections.data(),connections.size(),ms);
#endif
    
    if(v<0){
        perror("poll");
    }
    return v!=0;
}
bool SocketPoll::isWriteable(SocketBase& socket){
    int handle = socket.getHandle();
    if(handle2Index.find(handle)==handle2Index.end())
        return false;
    int index = handle2Index[handle];
    return connections[index].revents & POLLWRNORM;
}
bool SocketPoll::isReadable (SocketBase& socket){
    int handle = socket.getHandle();
    if(handle2Index.find(handle)==handle2Index.end())
        return false;
    int index = handle2Index[handle];
    return connections[index].revents & POLLRDNORM;
}
bool SocketPoll::isClosed   (SocketBase& socket){
    int handle = socket.getHandle();
    if(handle2Index.find(handle)==handle2Index.end())
        return true;
    int index = handle2Index[handle];
    return connections[index].revents & (POLLERR|POLLHUP|POLLNVAL);

}
