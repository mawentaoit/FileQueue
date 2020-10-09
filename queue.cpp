#include "queue.h"
#include "stdlib.h"



SrsFileQueue::SrsFileQueue(const char*  fname, uint64_t records_per_file, uint64_t pool_free_nums) : 
                             m_base_file_name_(fname)
                            ,m_pool_free_nums_(pool_free_nums)
                            ,m_index_(0)
                            ,m_records_per_file_(records_per_file)
                            {
}

SrsFileQueue::~SrsFileQueue() {
    //释放池
    while(!m_pool_fqueue_.empty()) {
        Fqueue* tmp = m_pool_fqueue_.front();
        m_pool_fqueue_.pop();
        tmp->remove_fqueue();
        delete tmp;
    }

    while(!m_pool_free_fqueue_.empty()) {
        Fqueue* tmp = m_pool_free_fqueue_.front();
        m_pool_free_fqueue_.pop();
        tmp->remove_fqueue();
        delete tmp; 
    }

}

bool SrsFileQueue::push(const char* ptr, std::uint32_t size) {
    if(ptr == NULL){
        std::cout << "error SrsFileQueue::push ptr == NULL" << std::endl;
        return false;
    }

    //如果为空，那么就创建
    if(m_pool_fqueue_.empty()) {
        m_pool_fqueue_.push(alloc_Fqueue());
    }

    //如果这个文件队列已经满记录，那么就添加到队列中一个文件队列
    if(m_pool_fqueue_.back()->alloc_records() >= m_records_per_file_) {
        m_pool_fqueue_.push(alloc_Fqueue());
    }

    m_pool_fqueue_.back()->push(ptr, size);
    
    return true;
}

Fqueue::record SrsFileQueue::pop() {
    if(m_pool_fqueue_.front()->records() <= 0) {
        Fqueue* tmp = m_pool_fqueue_.front();
        m_pool_fqueue_.pop();
        tmp->reset();
        m_pool_free_fqueue_.push(tmp);
    }
    
    //如果空闲池大于设置的阈值，那么久删除这个文件队列，目的：为了避免文件队列太长
    if(m_pool_free_fqueue_.size() > m_pool_free_nums_) {
        Fqueue* tmp = m_pool_free_fqueue_.front();
        m_pool_free_fqueue_.pop();
        tmp->remove_fqueue();
        //释放内存空间
        delete tmp;
    }

    return m_pool_fqueue_.front()->pop();
}

bool SrsFileQueue::empty() {
    if(m_pool_fqueue_.empty()) {
        return true;
    }
    if(m_pool_fqueue_.size() <= 1 && m_pool_fqueue_.front()->records() <= 0) {
        return true;
    }
    return false;
}

Fqueue* SrsFileQueue::alloc_Fqueue() {
    //保证我的空闲池中一定有可用的伪文件队列
    if(m_pool_free_fqueue_.empty()) {
        std::cout << "新建一个文件队列" << std::endl;
        std::string filename(get_next_filename());
        m_pool_free_fqueue_.push(new Fqueue(filename.data()));
    }else {
        std::cout << "利用空闲文件队列" << std::endl;
    }
    Fqueue* tmp = m_pool_free_fqueue_.front();
    m_pool_free_fqueue_.pop();
    return tmp;    
}

const char* SrsFileQueue::get_next_filename() {
    m_index_++;
    std::string filename = m_base_file_name_ + std::to_string(m_index_);
    return filename.data();
}
