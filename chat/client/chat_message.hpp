//
// chat_message.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>

//snappy
#include <snappy\snappy.h>

//Crypto++
#include <cryptopp\aes.h>
#include <cryptopp\ccm.h>
#include <cryptopp\filters.h>
using namespace CryptoPP;

class chat_message
{
public:
  enum { header_length = 4 };
  enum { max_body_length = 512 };

  chat_message()
    : body_length_(0)
  {
  }

  const char* data() const
  {
    return data_;
  }

  char* data()
  {
    return data_;
  }

  std::size_t length() const
  {
    return header_length + body_length_;
  }

  const char* body() const
  {
    return data_ + header_length;
  }

  char* body()
  {
    return data_ + header_length;
  }

  std::size_t body_length() const
  {
    return body_length_;
  }

  void body_length(std::size_t new_length)
  {
    body_length_ = new_length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
  }

  bool decode_header()
  {
    char header[header_length + 1] = "";
    std::strncat(header, data_, header_length);
    body_length_ = std::atoi(header);
    if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }

    return true;
  }

  void encode_header()
  {
    char header[header_length + 1] = "";
    std::sprintf(header, "%4d", static_cast<int>(body_length_));
    std::memcpy(data_, header, header_length);
  }

  //—πÀı
  bool compress()
  {
	  std::string compressed;
	  auto ret = snappy::Compress(data_ + header_length, body_length_, &compressed);
	  memset(data_ + header_length, 0, max_body_length);
	  memcpy(data_ + header_length, compressed.data(), compressed.length());
	  body_length_ = compressed.length();
	  return true;
  }

  //Ω‚—πÀı
  bool unCompress()
  {
	  std::string plain;
	  bool ret = snappy::Uncompress(data_ + header_length, body_length_, &plain);
	  if (!ret)
	  {
		  return false;
	  }
	  memset(data_ + header_length, 0, max_body_length);
	  memcpy(data_ + header_length, plain.data(), plain.size());
	  body_length_ = plain.size();
	  return true;
  }

  //º”√‹
  bool encrypt()
  {
	  std::string plain(data_ + header_length,body_length_),encode;
	  try {
		  CTR_Mode<AES>::Encryption e;
		  e.SetKeyWithIV(key, sizeof(key), iv);
		  StringSource(plain, true,
			  new StreamTransformationFilter(e,
				  new StringSink(encode)
			  )
		  );

		  memset(data_ + header_length, 0, max_body_length);
		  memcpy(data_ + header_length, encode.data(), encode.length());
		  body_length_ = encode.length();
	  }
	  catch (Exception& e)
	  {
		  std::cout << e.what() << std::endl;
		  return false;
	  }

	  return true;
  }


  //Ω‚√‹
  bool decrypt()
  {
	  std::string encode(data_ + header_length,body_length_), recovered;
	  try
	  {
		  CTR_Mode<AES>::Decryption d;
		  d.SetKeyWithIV(key, sizeof(key), iv);

		  StringSource(encode, true,
			  new StreamTransformationFilter(d,
				  new StringSink(recovered)
			  )
		  );

		  memset(data_ + header_length, 0, max_body_length);
		  memcpy(data_ + header_length, recovered.data(), recovered.size());
		  body_length_ = recovered.size();
	  }
	  catch (Exception& e)
	  {
		  std::cout << e.what() << std::endl;
		  return false;
	  }
	  return true;
  }

private:
	byte key[AES::DEFAULT_KEYLENGTH] = "iscas";
	byte iv[AES::BLOCKSIZE] = "tianji";

private:
  char data_[header_length + max_body_length];
  std::size_t body_length_;
};

#endif // CHAT_MESSAGE_HPP
