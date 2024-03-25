#include "Buffer.h"

#include <string.h>

bool Buffer::readOneMessageWith32Header(const Buffer &src) {
  const uint32_t kHeaderLen = 4;
  if (src.size() < kHeaderLen) return false;

  uint32_t len = 0;
  memcpy(&len, src.data(), kHeaderLen);

  if (len + kHeaderLen > src.size()) return false;

  data_.resize(len);
  memcpy(data_.data(), src.data() + kHeaderLen, len);
  return true;
}

void Buffer::appendWithHeader(const char *data, size_t size) {
  data_.clear();
  data_.append((char *)&size, 4);
  data_.append(data, size);
}