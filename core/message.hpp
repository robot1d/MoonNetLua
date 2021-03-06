/****************************************************************************

Git <https://github.com/sniper00/MoonNetLua>
E-Mail <hanyongtao@live.com>
Copyright (c) 2015-2017 moon
Licensed under the MIT License <http://opensource.org/licenses/MIT>.

****************************************************************************/

#pragma once
#include "config.h"
#include "common/buffer.hpp"

namespace moon
{
    class  message final
    {
    public:
        static buffer_ptr_t create_buffer(size_t capacity = 64, size_t headreserved = BUFFER_HEAD_RESERVED)
        {
            return std::make_shared<buffer>(capacity, headreserved);
        }

        static message_ptr_t create(size_t capacity = 64, size_t headreserved = BUFFER_HEAD_RESERVED)
        {
            return std::make_shared<message>(capacity, headreserved);
        }

        static message_ptr_t create(const buffer_ptr_t & v)
        {
            return std::make_shared<message>(v);
        }

        message(size_t capacity = 64, size_t headreserved = 0)
        {
            init();
            data_ = std::make_shared<buffer>(capacity, headreserved);
        }

        explicit message(const buffer_ptr_t & v)
        {
            init();
            data_ = v;
        }

        ~message()
        {
        }

        message(const message&) = delete;

        message& operator=(const message&) = delete;

        void set_sender(uint32_t serviceid)
        {
            sender_ = serviceid;
        }

        uint32_t sender() const
        {
            return sender_;
        }

        void set_receiver(uint32_t serviceid)
        {
            receiver_ = serviceid;
        }

        uint32_t receiver() const
        {
            return receiver_;
        }

        void set_header(string_view_t header)
        {
            if (header.size() != 0)
            {
                header_ = std::string(header.data(),header.size());
            }       
        }

        const std::string& header() const
        {
            return header_;
        }

        void set_responseid(int32_t v)
        {
            responseid_ = v;
        }

        int32_t responseid() const
        {
            return responseid_;
        }

        void set_type(uint8_t v)
        {
            flag_ &= 0xFFFF00FF;
            flag_ |= (static_cast<uint32_t>(v) << 8);
        }

        uint8_t type() const
        {
            return static_cast<uint8_t>(static_cast<uint8_t>(flag_>>8));
        }

        void set_subtype(uint8_t v)
        {
            flag_ &= 0xFFFFFF00;
            flag_ |= (static_cast<uint32_t>(v));
        }

        uint8_t subtype() const
        {
            return static_cast<uint8_t>(flag_);
        }

        string_view_t bytes() const
        {
            if (nullptr == data_)
            {
                return string_view_t(nullptr, 0);
            }
            return string_view_t(reinterpret_cast<const char*>(data_->data()),data_->size());
        }

        string_view_t subbytes(int pos, size_t len = string_view_t::npos) const
        {
            string_view_t sr(reinterpret_cast<const char*>(data_->data()), data_->size());
            return sr.substr(pos, len);
        }

        void write_data(string_view_t s)
        {
            data_->write_back(s.data(), 0, s.size());
        }

        void write_string(const std::string& s)
        {
            data_->write_back(s.data(), 0, s.size()+1);
        }

        const char* data() const
        {
            return data_->data();
        }

        size_t size() const
        {
            return data_->size();
        }

        operator const buffer_ptr_t&() const
        {
            return data_;
        }

        buffer* get_buffer()
        {
            return data_.get();
        }

        bool broadcast() const
        {
            return (((flag_ >> 16) & 0x1) !=0);
        }

        void set_broadcast(bool v)
        {
            flag_ &= 0xFFFEFFFF;
            flag_ |= ((v ? 1 : 0) << 16);
        }

        void* pointer()
        {
            return data_.get();
        }

        void reset()
        {
           init();
           header_.clear();
           data_->clear();
        }

        uint16_t check_uint16() const
        {
            if (data_->size() >= sizeof(uint16_t))
            {
                auto d =data_->data();
                if (data_->check_flag(uint8_t(buffer_flag::pack_size)))
                {
                    if (data_->size() < 2 * sizeof(uint16_t))
                    {
                        return 0;
                    }
                    d += 2;
                }
                return (*(uint16_t*)d);
            }
            return 0;
        }

    private:
        void init()
        {
            flag_ = 0;
            sender_ = 0;
            receiver_ = 0;
            responseid_ = 0;
        }

    private:
        //1-8bit subtype,9-16bit type, 17bit isbroadcast
        uint32_t flag_;
        uint32_t sender_;
        uint32_t receiver_;
        int32_t responseid_;
        std::string header_;
        buffer_ptr_t data_;
    };
};


