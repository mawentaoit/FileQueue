#ifndef _SRSQUEUE_H_
#define _SRSQUEUE_H_


#include "fqueue.h"
#include <queue>
#include <vector>
#include <iostream>



class SrsFileQueue{
public:
    SrsFileQueue() = delete;
    SrsFileQueue(const SrsFileQueue &) = delete;
    SrsFileQueue& operator= (const SrsFileQueue&) = delete;
    SrsFileQueue(const char* fname, uint64_t records_per_file = 2, uint64_t pool_free_nums = 1);
    ~SrsFileQueue();
    bool push(const char* ptr, std::uint32_t size);

    Fqueue::record pop();

    bool empty();
private:
    Fqueue* alloc_Fqueue();
private:
    const char* get_next_filename();
    std::queue<Fqueue*> m_pool_fqueue_; //正在使用的文件队列
    //如果一个文件队列使用完毕了，那么久放入到这个空闲池中，如果空闲池中也没有了，那么就开辟新的文件队列
    std::queue<Fqueue*> m_pool_free_fqueue_; 

    std::string m_base_file_name_;

    uint64_t m_pool_free_nums_;
    uint64_t m_index_;
    uint64_t m_records_per_file_;
};

#endif