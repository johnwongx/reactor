/*
发送与接收缓冲区
*/
#pragma once
#include <assert.h>

#include <string>

class Buffer {
 public:
  Buffer() = default;
  ~Buffer() = default;

  void append(const char *data, size_t size) { data_.append(data, size); }
  void appendWithHeader(const char *data, size_t size);

  void clear() { data_.clear(); }

  size_t size() const { return data_.length(); }

  const char *data() const { return data_.data(); }

  // 读取一条带有4字节信息长度的消息，长度不足时返回false
  bool readOneMessageWith32Header(const Buffer &src);

  void erase(size_t pos, size_t len) {
    assert(data_.size() >= pos + len);
    data_.erase(pos, len);
  }

 private:
  std::string data_;
};