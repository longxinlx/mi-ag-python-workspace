//
//  Agora Media SDK
//
//  Created by Yao Yuan in 2015-05.
//  Copyright (c) 2015 Agora IO. All rights reserved.
//

#pragma once

#include <cctype>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "protobuf-c/protobuf-c.h"

typedef void (*InitInstanceFunc)(ProtobufCMessage*);
typedef size_t (*GetPacketedSizeFunc)(const ProtobufCMessage*);
typedef size_t (*PackFunc)(const ProtobufCMessage*, uint8_t*);
typedef ProtobufCMessage* (*UnpackFunc)(ProtobufCAllocator*, size_t, const uint8_t*);
typedef void (*FreeUnpackedFunc)(ProtobufCMessage*, ProtobufCAllocator*);

#define PROTOBUF_ADD_PREFIX(NameSpace, MessageName) Io__Agora__Pb__##NameSpace##__##MessageName
#define PROTOBUF_ADD_PREFIX_VOSDK(MessageName) PROTOBUF_ADD_PREFIX(Vosdk, MessageName)
#define PROTOBUF_ADD_PREFIX_COUNTER(MessageName) PROTOBUF_ADD_PREFIX(Counter, MessageName)
#define PROTOBUF_ADD_PREFIX_MESSAGE(MessageName) PROTOBUF_ADD_PREFIX(Message, MessageName)
#define PROTOBUF_ADD_PREFIX_CACHE(MessageName) PROTOBUF_ADD_PREFIX(Cache, MessageName)
#define PROTOBUF_ADD_PREFIX_RTMSDK(MessageName) PROTOBUF_ADD_PREFIX(Rtmsdk, MessageName)
#define PROTOBUF_ADD_PREFIX_RTMSDK2(MessageName) PROTOBUF_ADD_PREFIX(Rtmsdk2, MessageName)
#define PROTOBUF_ADD_PREFIX_RSK(MessageName) PROTOBUF_ADD_PREFIX(Rtmp, MessageName)

#define PROTOBUF_FUNCTION(NameSpace, MessageName)                                          \
  (InitInstanceFunc) io__agora__pb__##NameSpace##__##MessageName##__init,                  \
      (GetPacketedSizeFunc)io__agora__pb__##NameSpace##__##MessageName##__get_packed_size, \
      (PackFunc)io__agora__pb__##NameSpace##__##MessageName##__pack,                       \
      (UnpackFunc)io__agora__pb__##NameSpace##__##MessageName##__unpack,                   \
      (FreeUnpackedFunc)io__agora__pb__##NameSpace##__##MessageName##__free_unpacked

#define PROTOBUF_MESSAGE_SIZE(MessageName) sizeof(PROTOBUF_ADD_PREFIX(Message, MessageName))
#define PROTOBUF_VOSDK_SIZE(MessageName) sizeof(PROTOBUF_ADD_PREFIX(Vosdk, MessageName))
#define PROTOBUF_COUNTER_SIZE(MessageName) sizeof(PROTOBUF_ADD_PREFIX(Counter, MessageName))
#define PROTOBUF_CACHE_SIZE(MessageName) sizeof(PROTOBUF_ADD_PREFIX(Cache, MessageName))
#define PROTOBUF_RTMSDK_SIZE(MessageName) sizeof(PROTOBUF_ADD_PREFIX(Rtmsdk, MessageName))
#define PROTOBUF_RTMSDK2_SIZE(MessageName) sizeof(PROTOBUF_ADD_PREFIX(Rtmsdk2, MessageName))
#define PROTOBUF_RSK_SIZE(MessageName) sizeof(PROTOBUF_ADD_PREFIX(Rtmp, MessageName))

#define PROTOBUF_FUNCTION_VOSDK(MessageName) PROTOBUF_FUNCTION(vosdk, MessageName)
#define PROTOBUF_FUNCTION_COUNTER(MessageName) PROTOBUF_FUNCTION(counter, MessageName)
#define PROTOBUF_FUNCTION_MESSAGE(MessageName) PROTOBUF_FUNCTION(message, MessageName)
#define PROTOBUF_FUNCTION_CACHE(MessageName) PROTOBUF_FUNCTION(cache, MessageName)
#define PROTOBUF_FUNCTION_RTMSDK(MessageName) PROTOBUF_FUNCTION(rtmsdk, MessageName)
#define PROTOBUF_FUNCTION_RTMSDK2(MessageName) PROTOBUF_FUNCTION(rtmsdk2, MessageName)
#define PROTOBUF_FUNCTION_RSK(MessageName) PROTOBUF_FUNCTION(rtmp, MessageName)
namespace agora {

namespace base {

class ProtobufWrapper {
 public:
  ProtobufWrapper(size_t* size_address, char*** string_vector_address);
  explicit ProtobufWrapper(char** string_address);
  explicit ProtobufWrapper(ProtobufCBinaryData* bytes_address);
  ProtobufWrapper(size_t* size_address, ProtobufCBinaryData** bytes_vector_address);
  ~ProtobufWrapper();

  static void TransformStringToCharPtrDeep(char** output, const std::string& input);
  static void TransformStringToBytes(ProtobufCBinaryData* output, const std::string& input);
  static bool isProtobufCBinaryDataEmpty(const ProtobufCBinaryData& binaryData);

  void Pack();

  /* In protobuf, char* member will be init to protobuf_c_empty_string，it's
   * defined: extern const char protobuf_c_empty_string[]; So, when want to free
   * a char* member, compare it with protobuf_c_empty_string.If equal, don't
   * free */
  static bool isProtobufEmptyString(const char* string_address) {
    return string_address == reinterpret_cast<const char*>(protobuf_c_empty_string);
  }

  inline std::string* getString() { return &string_; }

  inline const std::string* getString() const { return &string_; }

  inline std::vector<std::string>* getStringList() { return &string_vector_; }

  inline const std::vector<std::string>* getStringList() const { return &string_vector_; }

 private:
  size_t* size_address_;
  char*** string_vector_address_;
  char** string_address_;
  ProtobufCBinaryData* bytes_address_;
  ProtobufCBinaryData** bytes_vector_address_;
  std::vector<std::string> string_vector_;
  std::string string_;
};

class ProtobufInterface {
 public:
  virtual ~ProtobufInterface() {}
  virtual size_t PackInstance(std::string& content) = 0;
  virtual std::string* getString(const std::string& property_name) = 0;
  virtual bool setString(const std::string& property_name, const std::string& property_value) = 0;
};

class ProtobufInstance : public ProtobufInterface {
 public:
  ProtobufInstance(InitInstanceFunc initFunc, GetPacketedSizeFunc getSizeFunc, PackFunc packFunc,
                   UnpackFunc unpackFunc, FreeUnpackedFunc freeUnpackedFunc, size_t instance_size);

  ProtobufInstance(const std::string& content, InitInstanceFunc initFunc,
                   GetPacketedSizeFunc getSizeFunc, PackFunc packFunc, UnpackFunc unpackFunc,
                   FreeUnpackedFunc freeUnpackedFunc);

  ProtobufInstance(ProtobufInstance&& rhs);

  ProtobufInstance& operator=(ProtobufInstance&& rhs);

  virtual ~ProtobufInstance();

  void reset();

  ProtobufCMessage* release();

  const ProtobufCMessage* instance() const;

  ProtobufCMessage* instance();

  bool setString(const std::string& property_name, const std::string& value) override;

  std::string* getString(const std::string& property_name) override;

  const std::string* getString(const std::string& property_name) const;

  std::vector<std::string>* getStringList(const std::string& property_name);

  const std::vector<std::string>* getStringList(const std::string& property_name) const;

  size_t PackInstance(std::string& content) override;

  void UnpackInstance(const std::string& content);
  void UnpackInstance_vector(const std::vector<uint8_t>& content) {
    wrapper_map_.clear();
    if (content.empty()) {
      InitInstance();
      return;
    } else if (instance_) {
      DeinitInstance();
    }
    const uint8_t* buf = content.data();
    instance_ = unpack_(nullptr, content.size(), buf);
  }

  /* update data in wrapper to instance, prepare to pack; */
  void PrePack(const std::string& property_name = std::string());

 private:
  void InitInstance();

  void DeinitInstance();

  ProtobufWrapper* getWrapper(const std::string& name);

  const ProtobufWrapper* getWrapper(const std::string& name) const;

  void BindString(const std::string& property_name, char** string_address) const;

  void BindString(const std::string& property_name, ProtobufCBinaryData* bytes_address) const;

  void BindStringList(const std::string& property_name, size_t* size_address,
                      char*** string_vector_address) const;

  void BindStringList(const std::string& property_name, size_t* size_address,
                      ProtobufCBinaryData** bytes_vector_address) const;

  std::string* getString(const std::string& property_name, char** string_address);

  const std::string* getString(const std::string& property_name, char** string_address) const;

  std::string* getString(const std::string& property_name, ProtobufCBinaryData* bytes_address);

  const std::string* getString(const std::string& property_name,
                               ProtobufCBinaryData* bytes_address) const;

  std::vector<std::string>* getStringList(const std::string& property_name, size_t* size_address,
                                          char*** string_vector_address);

  const std::vector<std::string>* getStringList(const std::string& property_name,
                                                size_t* size_address,
                                                char*** string_vector_address) const;

  std::vector<std::string>* getStringList(const std::string& property_name, size_t* size_address,
                                          ProtobufCBinaryData** bytes_vector_address);

  const std::vector<std::string>* getStringList(const std::string& property_name,
                                                size_t* size_address,
                                                ProtobufCBinaryData** bytes_vector_address) const;

  static bool compareChar(const char& c1, const char& c2);

  static bool caseInSensStringCompare(const std::string& str1, const std::string& str2);

  InitInstanceFunc init_;
  GetPacketedSizeFunc get_packed_size_;
  PackFunc pack_;
  UnpackFunc unpack_;
  FreeUnpackedFunc free_unpacked_;

  ProtobufCMessage* instance_;
  size_t instance_size_;
  mutable std::map<std::string, ProtobufWrapper> wrapper_map_;
};

}  // namespace base
}  // namespace agora
