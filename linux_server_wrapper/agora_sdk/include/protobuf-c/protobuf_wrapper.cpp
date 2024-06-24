//
//  protobuf_wrapper.cpp
//  agora_rtc_sdk
//
//  Created by Yao Yuan on 2019/4/3.
//

#include "protobuf_wrapper.h"

#include "base/AgoraBase.h"

namespace agora {
namespace base {

ProtobufWrapper::ProtobufWrapper(size_t* size_address, char*** string_vector_address)
    : size_address_(size_address),
      string_vector_address_(string_vector_address),
      string_address_(nullptr),
      bytes_address_(nullptr),
      bytes_vector_address_(nullptr) {
  if (size_address && *size_address && string_vector_address && *string_vector_address) {
    size_t size = *size_address;
    for (int i = 0; i < size; ++i) {
      string_vector_.emplace_back((*string_vector_address_)[i]);
    }
  }
}

ProtobufWrapper::ProtobufWrapper(char** string_address)
    : size_address_(nullptr),
      string_address_(string_address),
      string_vector_address_(nullptr),
      bytes_address_(nullptr),
      bytes_vector_address_(nullptr) {
  if (string_address && *string_address) {
    string_.assign(*string_address);
  }
}

ProtobufWrapper::ProtobufWrapper(ProtobufCBinaryData* bytes_address)
    : size_address_(nullptr),
      string_address_(nullptr),
      string_vector_address_(nullptr),
      bytes_address_(bytes_address),
      bytes_vector_address_(nullptr) {
  if (bytes_address && bytes_address->len > 0 && bytes_address->data) {
    string_.assign(reinterpret_cast<char*>(bytes_address->data), bytes_address->len);
  }
}

ProtobufWrapper::ProtobufWrapper(size_t* size_address, ProtobufCBinaryData** bytes_vector_address)
    : size_address_(size_address),
      string_vector_address_(nullptr),
      string_address_(nullptr),
      bytes_address_(nullptr),
      bytes_vector_address_(bytes_vector_address) {
  if (size_address && *size_address && bytes_vector_address_ && *bytes_vector_address_) {
    for (int i = 0; i < *size_address; ++i) {
      string_vector_.emplace_back(reinterpret_cast<char*>((*bytes_vector_address_)[i].data),
                                  (*bytes_vector_address_)[i].len);
    }
  }
}

ProtobufWrapper::~ProtobufWrapper() {}

/* When pack, free the old data and write new data to the instance address */
void ProtobufWrapper::Pack() {
  if (string_vector_address_ && size_address_) {
    int old_size = *size_address_;
    for (int i = 0; i < old_size; ++i) {
      free((*string_vector_address_)[i]);
      (*string_vector_address_)[i] = nullptr;
    }
    free(*string_vector_address_);
    *string_vector_address_ = nullptr;
    *size_address_ = 0;

    int size = string_vector_.size();
    if (size == 0) {
      return;
    }
    *size_address_ = size;
    *string_vector_address_ = (char**)malloc(sizeof(char*) * size);
    for (int i = 0; i < size; ++i) {
      TransformStringToCharPtrDeep(&(*string_vector_address_)[i], string_vector_[i]);
    }
  } else if (string_address_) {
    if (!isProtobufEmptyString(*string_address_)) {
      free(*string_address_);
      *string_address_ = nullptr;
    }
    TransformStringToCharPtrDeep(string_address_, string_);
  } else if (bytes_address_) {
    free(bytes_address_->data);
    bytes_address_->data = nullptr;
    bytes_address_->len = 0;
    TransformStringToBytes(bytes_address_, string_);
  } else if (bytes_vector_address_ && size_address_) {
    for (int i = 0; i < *size_address_; ++i) {
      free((*bytes_vector_address_)[i].data);
      (*bytes_vector_address_)[i].data = nullptr;
      (*bytes_vector_address_)[i].len = 0;
    }
    free(*bytes_vector_address_);
    *bytes_vector_address_ = nullptr;
    *size_address_ = 0;

    int size = string_vector_.size();
    if (size == 0) {
      return;
    }
    *size_address_ = size;
    *bytes_vector_address_ = (ProtobufCBinaryData*)malloc(sizeof(ProtobufCBinaryData) * size);
    for (int i = 0; i < size; ++i) {
      TransformStringToBytes(&(*bytes_vector_address_)[i], string_vector_[i]);
    }
  }
}

void ProtobufWrapper::TransformStringToCharPtrDeep(char** output, const std::string& input) {
  size_t size = input.size();
  *output = (char*)malloc(sizeof(char) * (size + 1));
  input.copy(*output, size);
  (*output)[size] = '\0';
}

void ProtobufWrapper::TransformStringToBytes(ProtobufCBinaryData* output,
                                             const std::string& input) {
  if (input.empty()) {
    return;
  }
  size_t size = input.size();
  output->len = size;
  output->data = (uint8_t*)malloc(sizeof(uint8_t) * size);
  input.copy(reinterpret_cast<char*>(output->data), size);
}

bool ProtobufWrapper::isProtobufCBinaryDataEmpty(const ProtobufCBinaryData& binaryData) {
  return binaryData.len == 0 || !binaryData.data;
}

ProtobufInstance::ProtobufInstance(InitInstanceFunc initFunc, GetPacketedSizeFunc getSizeFunc,
                                   PackFunc packFunc, UnpackFunc unpackFunc,
                                   FreeUnpackedFunc freeUnpackedFunc, size_t instance_size)
    : init_(initFunc),
      get_packed_size_(getSizeFunc),
      pack_(packFunc),
      unpack_(unpackFunc),
      free_unpacked_(freeUnpackedFunc),
      instance_size_(instance_size),
      instance_(nullptr) {
  InitInstance();
}

ProtobufInstance::ProtobufInstance(const std::string& content, InitInstanceFunc initFunc,
                                   GetPacketedSizeFunc getSizeFunc, PackFunc packFunc,
                                   UnpackFunc unpackFunc, FreeUnpackedFunc freeUnpackedFunc)
    : init_(initFunc),
      get_packed_size_(getSizeFunc),
      pack_(packFunc),
      unpack_(unpackFunc),
      free_unpacked_(freeUnpackedFunc),
      instance_(nullptr) {
  UnpackInstance(content);
}

ProtobufInstance::ProtobufInstance(ProtobufInstance&& rhs) {
  init_ = rhs.init_;
  get_packed_size_ = rhs.get_packed_size_;
  pack_ = rhs.pack_;
  unpack_ = rhs.unpack_;
  free_unpacked_ = rhs.free_unpacked_;
  instance_ = rhs.instance_;
  wrapper_map_ = std::move(rhs.wrapper_map_);
  rhs.instance_ = nullptr;
}

ProtobufInstance& ProtobufInstance::operator=(ProtobufInstance&& rhs) {
  if (this != &rhs) {
    init_ = rhs.init_;
    get_packed_size_ = rhs.get_packed_size_;
    pack_ = rhs.pack_;
    unpack_ = rhs.unpack_;
    free_unpacked_ = rhs.free_unpacked_;
    instance_ = rhs.instance_;
    wrapper_map_ = std::move(rhs.wrapper_map_);
    rhs.instance_ = nullptr;
  }
  return *this;
}

ProtobufInstance::~ProtobufInstance() {
  if (instance_) {
    DeinitInstance();
  }
}

void ProtobufInstance::reset() {
  InitInstance();
  wrapper_map_.clear();
}

ProtobufCMessage* ProtobufInstance::release() {
  ProtobufCMessage* tmp = instance_;
  instance_ = nullptr;
  return tmp;
}

const ProtobufCMessage* ProtobufInstance::instance() const { return instance_; }

ProtobufCMessage* ProtobufInstance::instance() { return instance_; }

bool ProtobufInstance::setString(const std::string& property_name, const std::string& value) {
  std::string* str = getString(property_name);
  if (!str) {
    return false;
  }
  *str = value;
  return true;
}

std::string* ProtobufInstance::getString(const std::string& property_name) {
  return const_cast<std::string*>(
      static_cast<const ProtobufInstance&>(*this).getString(property_name));
}

const std::string* ProtobufInstance::getString(const std::string& property_name) const {
  if (!instance_ || property_name.empty()) {
    return nullptr;
  }
  const auto descriptor = instance_->descriptor;
  for (unsigned int i = 0; i < descriptor->n_fields; ++i) {
    const auto fieldDescriptor = descriptor->fields + i;
    const std::string temp_name = fieldDescriptor->name;
    if (PROTOBUF_C_LABEL_REPEATED == fieldDescriptor->label ||
        !caseInSensStringCompare(temp_name, property_name)) {
      continue;
    }
    switch (fieldDescriptor->type) {
      case PROTOBUF_C_TYPE_STRING: {
        char** member_address =
            reinterpret_cast<char**>(reinterpret_cast<char*>(instance_) + fieldDescriptor->offset);
        return getString(property_name, member_address);
        break;
      }
      case PROTOBUF_C_TYPE_BYTES: {
        ProtobufCBinaryData* member_address = reinterpret_cast<ProtobufCBinaryData*>(
            reinterpret_cast<char*>(instance_) + fieldDescriptor->offset);
        return getString(property_name, member_address);
        break;
      }
      default:
        break;
    }
  }
  return nullptr;
}

std::vector<std::string>* ProtobufInstance::getStringList(const std::string& property_name) {
  return const_cast<std::vector<std::string>*>(
      static_cast<const ProtobufInstance&>(*this).getStringList(property_name));
}

const std::vector<std::string>* ProtobufInstance::getStringList(
    const std::string& property_name) const {
  if (!instance_ || property_name.empty()) {
    return nullptr;
  }
  const auto descriptor = instance_->descriptor;
  for (int i = 0; i < descriptor->n_fields; i++) {
    const ProtobufCFieldDescriptor* fieldDescriptor = descriptor->fields + i;
    const std::string temp_name = fieldDescriptor->name;
    if (PROTOBUF_C_LABEL_REPEATED != fieldDescriptor->label ||
        !caseInSensStringCompare(temp_name, property_name)) {
      continue;
    }
    switch (fieldDescriptor->type) {
      case PROTOBUF_C_TYPE_STRING: {
        char*** member =
            reinterpret_cast<char***>(reinterpret_cast<char*>(instance_) + fieldDescriptor->offset);
        size_t* qmember = reinterpret_cast<size_t*>(reinterpret_cast<char*>(instance_) +
                                                    fieldDescriptor->quantifier_offset);
        return getStringList(property_name, qmember, member);
        break;
      }
      case PROTOBUF_C_TYPE_BYTES: {
        ProtobufCBinaryData** member = reinterpret_cast<ProtobufCBinaryData**>(
            reinterpret_cast<char*>(instance_) + fieldDescriptor->offset);
        size_t* qmember = reinterpret_cast<size_t*>(reinterpret_cast<char*>(instance_) +
                                                    fieldDescriptor->quantifier_offset);
        return getStringList(property_name, qmember, member);
        break;
      }
      default:
        break;
    }
  }
  return nullptr;
}

size_t ProtobufInstance::PackInstance(std::string& content) {
  PrePack();
  size_t size = get_packed_size_(instance_);
  if (size == 0) {
    return 0;
  }
  content.resize(size);
  pack_(instance_, reinterpret_cast<uint8_t*>(&content[0]));
  return size;
}

void ProtobufInstance::UnpackInstance(const std::string& content) {
  wrapper_map_.clear();
  if (content.empty()) {
    InitInstance();
    return;
  } else if (instance_) {
    DeinitInstance();
  }
  const uint8_t* buf = reinterpret_cast<const uint8_t*>(&content[0]);
  instance_ = unpack_(nullptr, content.size(), buf);
}

/* update data in wrapper to instance, prepare to pack; */
void ProtobufInstance::PrePack(const std::string& property_name) {
  if (property_name.empty()) {
    for (auto& p : wrapper_map_) {
      p.second.Pack();
    }
  } else {
    ProtobufWrapper* wrapper = getWrapper(property_name);
    if (wrapper) {
      wrapper->Pack();
    }
  }
}

void ProtobufInstance::InitInstance() {
  if (instance_) {
    DeinitInstance();
  }
  instance_ = (ProtobufCMessage*)malloc(instance_size_);
  init_(instance_);
}

void ProtobufInstance::DeinitInstance() {
  free_unpacked_(instance_, nullptr);
  instance_ = nullptr;
}

ProtobufWrapper* ProtobufInstance::getWrapper(const std::string& name) {
  auto iter = wrapper_map_.find(name);
  if (iter != wrapper_map_.end()) {
    return &iter->second;
  }
  return nullptr;
}

const ProtobufWrapper* ProtobufInstance::getWrapper(const std::string& name) const {
  auto iter = wrapper_map_.find(name);
  if (iter != wrapper_map_.end()) {
    return &iter->second;
  }
  return nullptr;
}

void ProtobufInstance::BindString(const std::string& property_name, char** string_address) const {
  if (property_name.empty() || !string_address) {
    return;
  }
  auto iter = wrapper_map_.find(property_name);
  if (iter != wrapper_map_.end()) {
    return;
  }
  // use piecewise_construct to construct in order to avoid copy construct
  wrapper_map_.emplace(std::piecewise_construct, std::make_tuple(property_name),
                       std::make_tuple(string_address));
}

void ProtobufInstance::BindString(const std::string& property_name,
                                  ProtobufCBinaryData* bytes_address) const {
  if (property_name.empty() || !bytes_address) {
    return;
  }
  auto iter = wrapper_map_.find(property_name);
  if (iter != wrapper_map_.end()) {
    return;
  }
  // use piecewise_construct to construct in order to avoid copy construct
  wrapper_map_.emplace(std::piecewise_construct, std::make_tuple(property_name),
                       std::make_tuple(bytes_address));
}

void ProtobufInstance::BindStringList(const std::string& property_name, size_t* size_address,
                                      char*** string_vector_address) const {
  if (property_name.empty() || !size_address || !string_vector_address) {
    return;
  }
  auto iter = wrapper_map_.find(property_name);
  if (iter != wrapper_map_.end()) {
    return;
  }
  wrapper_map_.emplace(std::piecewise_construct, std::make_tuple(property_name),
                       std::make_tuple(size_address, string_vector_address));
}

void ProtobufInstance::BindStringList(const std::string& property_name, size_t* size_address,
                                      ProtobufCBinaryData** bytes_vector_address) const {
  if (property_name.empty() || !size_address || !bytes_vector_address) {
    return;
  }
  auto iter = wrapper_map_.find(property_name);
  if (iter != wrapper_map_.end()) {
    return;
  }
  wrapper_map_.emplace(std::piecewise_construct, std::make_tuple(property_name),
                       std::make_tuple(size_address, bytes_vector_address));
}

std::string* ProtobufInstance::getString(const std::string& property_name, char** string_address) {
  return const_cast<std::string*>(
      static_cast<const ProtobufInstance&>(*this).getString(property_name, string_address));
}

const std::string* ProtobufInstance::getString(const std::string& property_name,
                                               char** string_address) const {
  if (property_name.empty()) {
    return nullptr;
  }
  const ProtobufWrapper* wrapper = getWrapper(property_name);
  if (wrapper) {
    return wrapper->getString();
  } else if (string_address) {
    BindString(property_name, string_address);
    return getWrapper(property_name)->getString();
  }
  return nullptr;
}

std::string* ProtobufInstance::getString(const std::string& property_name,
                                         ProtobufCBinaryData* bytes_address) {
  return const_cast<std::string*>(
      static_cast<const ProtobufInstance&>(*this).getString(property_name, bytes_address));
}

const std::string* ProtobufInstance::getString(const std::string& property_name,
                                               ProtobufCBinaryData* bytes_address) const {
  if (property_name.empty()) {
    return nullptr;
  }
  const ProtobufWrapper* wrapper = getWrapper(property_name);
  if (wrapper) {
    return wrapper->getString();
  } else if (bytes_address) {
    BindString(property_name, bytes_address);
    return getWrapper(property_name)->getString();
  }
  return nullptr;
}

std::vector<std::string>* ProtobufInstance::getStringList(const std::string& property_name,
                                                          size_t* size_address,
                                                          char*** string_vector_address) {
  return const_cast<std::vector<std::string>*>(
      static_cast<const ProtobufInstance&>(*this).getStringList(property_name, size_address,
                                                                string_vector_address));
}

const std::vector<std::string>* ProtobufInstance::getStringList(
    const std::string& property_name, size_t* size_address, char*** string_vector_address) const {
  if (property_name.empty()) {
    return nullptr;
  }
  const ProtobufWrapper* wrapper = getWrapper(property_name);
  if (wrapper) {
    return wrapper->getStringList();
  } else if (size_address && string_vector_address) {
    BindStringList(property_name, size_address, string_vector_address);
    return getWrapper(property_name)->getStringList();
  }
  return nullptr;
}

std::vector<std::string>* ProtobufInstance::getStringList(
    const std::string& property_name, size_t* size_address,
    ProtobufCBinaryData** bytes_vector_address) {
  return const_cast<std::vector<std::string>*>(
      static_cast<const ProtobufInstance&>(*this).getStringList(property_name, size_address,
                                                                bytes_vector_address));
}

const std::vector<std::string>* ProtobufInstance::getStringList(
    const std::string& property_name, size_t* size_address,
    ProtobufCBinaryData** bytes_vector_address) const {
  if (property_name.empty()) {
    return nullptr;
  }
  const ProtobufWrapper* wrapper = getWrapper(property_name);
  if (wrapper) {
    return wrapper->getStringList();
  } else if (size_address && bytes_vector_address) {
    BindStringList(property_name, size_address, bytes_vector_address);
    return getWrapper(property_name)->getStringList();
  }
  return nullptr;
}

bool ProtobufInstance::compareChar(const char& c1, const char& c2) {
  if (c1 == c2) {
    return true;
  } else if (std::toupper(c1) == std::toupper(c2)) {
    return true;
  }
  return false;
}

bool ProtobufInstance::caseInSensStringCompare(const std::string& str1, const std::string& str2) {
  return ((str1.size() == str2.size()) &&
          std::equal(str1.begin(), str1.end(), str2.begin(), &compareChar));
}

}  // namespace base
}  // namespace agora
