//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "asio.hpp"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>
#include <iostream>
#include <list>
#include <string>
#include "handler_allocator.hpp"
#include <chrono>
#include "../protocal.h"


class stats {
public:
    stats()
            : mutex_(),
              total_bytes_written_(0),
              total_bytes_read_(0) {
    }

    void add(size_t bytes_written, size_t bytes_read) {
        asio::detail::mutex::scoped_lock lock(mutex_);
        total_bytes_written_ += bytes_written;
        total_bytes_read_ += bytes_read;
    }

    void print() {
        asio::detail::mutex::scoped_lock lock(mutex_);
        std::cout << total_bytes_written_ << " total bytes written\n";
        std::cout << total_bytes_read_ << " total bytes read\n";
    }

private:
    asio::detail::mutex mutex_;
    size_t total_bytes_written_;
    size_t total_bytes_read_;
};

class session {
public:
    session(asio::io_context &ioc, size_t block_size, stats &s)
            : strand_(ioc),
              socket_(ioc),
              block_size_(block_size),
              read_data_(new char[block_size]),
              read_data_length_(0),
              write_data_(new char[block_size]),
              unwritten_count_(0),
              bytes_written_(0),
              bytes_read_(0),
              stats_(s) {
//        for (size_t i = 0; i < block_size_; ++i)
//            write_data_[i] = static_cast<char>(i % 128);
        memset(write_data_, 0, block_size_);
    }

    ~session() {
        stats_.add(bytes_written_, bytes_read_);

        delete[] read_data_;
        delete[] write_data_;
    }

    void serialization() {

        memset(write_data_,0,block_size_);

        //消息头;
        SOCKET_MESSAGE_HEADER header;
        memset(&header, 0, sizeof(header));

        //消息体;
        DateStruct::DS_STATE sendMsg;
        memset(&sendMsg, 0, sizeof(DateStruct::DS_STATE));

        sendMsg.m_char = '1';
        sendMsg.m_int = 3;
        std::string name = "Liu Guangxuan";
        memcpy(sendMsg.m_string, &name[0], name.length());

        //消息体长度;
        int bodyLength = sizeof(DateStruct::DS_STATE);

        header.uhMessageLength = sizeof(header) + bodyLength;

        //将消息头拷贝到buffer中;
        memcpy(write_data_, &header, sizeof(header));

        //将消息体拷贝到buffer中;
        memcpy(write_data_ + sizeof(header), &sendMsg.m_char, sizeof(char));
        memcpy(write_data_ + sizeof(header) + sizeof(char), &sendMsg.m_int, sizeof(int));
        memcpy(write_data_ + sizeof(header) + sizeof(char) + sizeof(int), &sendMsg.m_string, 100);
    }

    void deserialization() {

        //消息头;
        SOCKET_MESSAGE_HEADER header;
        memset(&header, 0, sizeof(header));

        //消息体;
        DateStruct::DS_STATE sendMsg;
        memset(&sendMsg, 0, sizeof(DateStruct::DS_STATE));

        memcpy(&header, read_data_, sizeof(header));

        memcpy(&sendMsg.m_char,read_data_ + sizeof(header),sizeof(char));
        memcpy(&sendMsg.m_int,read_data_ + sizeof(header) + sizeof(char),sizeof(int));
        memcpy(&sendMsg.m_string,read_data_ + sizeof(header)+ sizeof(char) + sizeof(int),100);

        memset(read_data_,0,block_size_);
    }

    void start(asio::ip::tcp::resolver::results_type endpoints) {
        asio::async_connect(socket_, endpoints,
                            asio::bind_executor(strand_,
                                                boost::bind(&session::handle_connect, this,
                                                            asio::placeholders::error)));
    }

    void stop() {
        asio::post(strand_, boost::bind(&session::close_socket, this));
    }

private:
    void handle_connect(const asio::error_code &err) {
        if (!err) {
            asio::error_code set_option_err;
            asio::ip::tcp::no_delay no_delay(true);
            socket_.set_option(no_delay, set_option_err);

            start_ = std::chrono::system_clock::now();

            if (!set_option_err) {
                ++unwritten_count_;

                serialization();

                async_write(socket_, asio::buffer(write_data_, block_size_),
                            asio::bind_executor(strand_,
                                                make_custom_alloc_handler(write_allocator_,
                                                                          boost::bind(&session::handle_write, this,
                                                                                      asio::placeholders::error,
                                                                                      asio::placeholders::bytes_transferred))));
                socket_.async_read_some(asio::buffer(read_data_, block_size_),
                                        asio::bind_executor(strand_,
                                                            make_custom_alloc_handler(read_allocator_,
                                                                                      boost::bind(&session::handle_read,
                                                                                                  this,
                                                                                                  asio::placeholders::error,
                                                                                                  asio::placeholders::bytes_transferred))));
            }
        }
    }

    void handle_read(const asio::error_code &err, size_t length) {
        if (!err) {
            bytes_read_ += length;

            read_data_length_ = length;
            ++unwritten_count_;
            if (unwritten_count_ == 1) {

                deserialization();

                serialization();

                std::swap(read_data_, write_data_);
                async_write(socket_, asio::buffer(write_data_, read_data_length_),
                            asio::bind_executor(strand_,
                                                make_custom_alloc_handler(write_allocator_,
                                                                          boost::bind(&session::handle_write, this,
                                                                                      asio::placeholders::error,
                                                                                      asio::placeholders::bytes_transferred))));
                socket_.async_read_some(asio::buffer(read_data_, block_size_),
                                        asio::bind_executor(strand_,
                                                            make_custom_alloc_handler(read_allocator_,
                                                                                      boost::bind(&session::handle_read,
                                                                                                  this,
                                                                                                  asio::placeholders::error,
                                                                                                  asio::placeholders::bytes_transferred))));
            }
        }
    }

    void handle_write(const asio::error_code &err, size_t length) {
        if (!err && length > 0) {
            bytes_written_ += length;

            --unwritten_count_;
            if (unwritten_count_ == 1) {
                serialization();

                std::swap(read_data_, write_data_);
                async_write(socket_, asio::buffer(write_data_, read_data_length_),
                            asio::bind_executor(strand_,
                                                make_custom_alloc_handler(write_allocator_,
                                                                          boost::bind(&session::handle_write, this,
                                                                                      asio::placeholders::error,
                                                                                      asio::placeholders::bytes_transferred))));
                socket_.async_read_some(asio::buffer(read_data_, block_size_),
                                        asio::bind_executor(strand_,
                                                            make_custom_alloc_handler(read_allocator_,
                                                                                      boost::bind(&session::handle_read,
                                                                                                  this,
                                                                                                  asio::placeholders::error,
                                                                                                  asio::placeholders::bytes_transferred))));
            }
        }
    }

    void close_socket() {
        socket_.close();
    }

private:
    asio::io_context::strand strand_;
    asio::ip::tcp::socket socket_;
    size_t block_size_;
    char *read_data_;
    size_t read_data_length_;
    char *write_data_;
    int unwritten_count_;
    size_t bytes_written_;
    size_t bytes_read_;
    stats &stats_;
    handler_allocator read_allocator_;
    handler_allocator write_allocator_;


    std::chrono::time_point<std::chrono::system_clock> start_;

};

class client {
public:
    client(asio::io_context &ioc,
           const asio::ip::tcp::resolver::results_type endpoints,
           size_t block_size, size_t session_count, int timeout)
            : io_context_(ioc),
              stop_timer_(ioc),
              sessions_(),
              stats_() {
        stop_timer_.expires_after(asio::chrono::seconds(timeout));
        stop_timer_.async_wait(boost::bind(&client::handle_timeout, this));

        for (size_t i = 0; i < session_count; ++i) {
            session *new_session = new session(io_context_, block_size, stats_);
            new_session->start(endpoints);
            sessions_.push_back(new_session);
        }
    }

    ~client() {
        while (!sessions_.empty()) {
            delete sessions_.front();
            sessions_.pop_front();
        }

        stats_.print();
    }

    void handle_timeout() {
        std::for_each(sessions_.begin(), sessions_.end(),
                      boost::mem_fn(&session::stop));
    }

private:
    asio::io_context &io_context_;
    asio::steady_timer stop_timer_;
    std::list<session *> sessions_;
    stats stats_;
};

int main(int argc, char *argv[]) {
    try {
        if (argc != 7) {
            std::cerr << "Usage: client <host> <port> <threads> <blocksize> ";
            std::cerr << "<sessions> <time>\n";
            return 1;
        }

        using namespace std; // For atoi.
        const char *host = argv[1];
        const char *port = argv[2];
        int thread_count = atoi(argv[3]);
        size_t block_size = atoi(argv[4]);
        size_t session_count = atoi(argv[5]);
        int timeout = atoi(argv[6]);

        asio::io_context ioc;

        asio::ip::tcp::resolver r(ioc);
        asio::ip::tcp::resolver::results_type endpoints =
                r.resolve(host, port);

        client c(ioc, endpoints, block_size, session_count, timeout);

        std::list<asio::thread *> threads;
        while (--thread_count > 0) {
            asio::thread *new_thread = new asio::thread(
                    boost::bind(&asio::io_context::run, &ioc));
            threads.push_back(new_thread);
        }

        ioc.run();

        while (!threads.empty()) {
            threads.front()->join();
            delete threads.front();
            threads.pop_front();
        }
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
