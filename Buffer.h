/*
发送与接收缓冲区
*/
#pragma once
#include <string>

class Buffer{
public:
    Buffer() = default;
    ~Buffer() = default;

    void append(const char* data, size_t size){
        data_.append(data, size);
    }

    void clear(){
        data_.clear();
    }

    size_t size() const{
        return data_.length();
    }

    const char* data() const{
        return data_.data();
    }

private:
    std::string data_;
};