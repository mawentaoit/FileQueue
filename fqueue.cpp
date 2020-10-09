#include "fqueue.h"
#include <iostream>


File_IO::File_IO(const char* fname) : m_fname(fname), m_fd(-1) {
	m_fd = ::open(fname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    assert(-1 != m_fd);
};

File_IO::~File_IO() {
    if (is_open()) {
        close();
    }
}

bool File_IO::is_open() {
    return m_fd != -1;
}

void File_IO::close() {
    ::close(m_fd);
    m_fd = -1;
}

std::size_t File_IO::size() {
    struct stat st;
    ::fstat(m_fd, &st);
    return st.st_size;
}

bool File_IO::resize(std::size_t size) {
    return 0 == ::ftruncate(m_fd, size);
}

bool File_IO::seek(std::size_t pos) {
    return pos == static_cast<std::size_t>(::lseek(m_fd, pos, SEEK_SET));
}

bool File_IO::write(const void* ptr, std::size_t size) {
    if (size == static_cast<std::size_t>(::write(m_fd, ptr, size)) ) {
        return 0 == ::fsync(m_fd);
    }

    return false;
}

bool File_IO::read(void* ptr, std::size_t size) {
    return size == static_cast<std::size_t>(::read(m_fd, ptr, size));
}

bool File_IO::remove_file() {
    return 0 == ::remove(m_fname.data()); 
}



//----------------------------------------------------------------
Fqueue::Fqueue(const char* fname, size_t size) : m_file(fname) , m_fsize(size) {
    if (m_file.size() == 0) {
        const queue_info qi = {
            ENUM_SIZE_OF_QUEUE_INFO,
            ENUM_SIZE_OF_QUEUE_INFO,
            0,
            0
        };

        write_info(qi);
        if(m_fsize > ENUM_SIZE_OF_QUEUE_INFO) {
            m_file.resize(m_fsize);
        }
    }
}

Fqueue::~Fqueue() {

}

std::size_t Fqueue::alloc_records() {
    queue_info qi;
    read_info(&qi);
    return qi.alloc_records;
}

std::size_t Fqueue::records()  {
    queue_info qi;
    read_info(&qi);

    return qi.records;
}

bool Fqueue::empty()  {
    return 0 == records();
}

void Fqueue::reset() {
    queue_info qi;
    read_info(&qi);
    qi.rpos = qi.wpos = ENUM_SIZE_OF_QUEUE_INFO;
    qi.records = 0;
    qi.alloc_records = 0;
    write_info(qi);
}


void Fqueue::truncate() {
    reset();
    if ( m_fsize >= ENUM_SIZE_OF_QUEUE_INFO) {
        m_file.resize(m_fsize);
    } else {
        m_file.resize(ENUM_SIZE_OF_QUEUE_INFO);
    }
}

void Fqueue::push(const void* ptr, std::uint32_t size) {
    queue_info qi;
    read_info(&qi);

    const std::uint64_t wpos = qi.wpos;
    assert(true == m_file.seek(wpos));

    assert( true == m_file.write(&size, sizeof(size)) );
    qi.wpos += sizeof(size);
    assert( true == m_file.write(ptr, size));
    qi.wpos += size;

    ++qi.records;
    ++qi.alloc_records;

    std::cout << "push(): rd: rpos= " << qi.rpos 
    << ", wpos=" << qi.wpos << ", records" << qi.records << std::endl;

    write_info(qi);
}


bool Fqueue::remove_fqueue() {
    return m_file.remove_file();
}

Fqueue::record Fqueue::read(){
    std::uint32_t block_size = 0;
    assert( true == m_file.read(&block_size, sizeof(block_size)) );
    
    std::unique_ptr<char[]> ptr(new char[block_size]);
    assert( true == m_file.read(ptr.get(), block_size));

    Fqueue::record rec = {
        std::move(ptr),
        block_size
    };

    return rec;
}

Fqueue::record Fqueue::front() {
    queue_info qi;
    read_info(&qi);

    m_file.seek(qi.rpos);

    return read();
}

Fqueue::record Fqueue::pop() {
    queue_info qi;
    read_info(&qi);

    record rec{0,0};

    m_file.seek(qi.rpos);

    rec = read();
    qi.rpos += sizeof(std::uint32_t) + rec.size;
    
    --qi.records;
    if ( 0 == qi.records ) {
        qi.rpos = qi.wpos = ENUM_SIZE_OF_QUEUE_INFO;
    }
    std::cout<< "pop():  rpos=" << qi.rpos
            << ", wpos=" << qi.wpos
            << ", records=" << qi.records << std::endl;      

    write_info(qi);

    return rec; 
}

Fqueue::record Fqueue::first_record()  {
    return front();
}

Fqueue::record Fqueue::next_record() {
    return read();
}


void Fqueue::write_info(const queue_info &qi) {
    assert(true == m_file.seek(0));
    assert(true == m_file.write(&qi, ENUM_SIZE_OF_QUEUE_INFO));
}

void Fqueue::read_info(queue_info* qi) {
    m_file.seek(0);
    m_file.read(qi, ENUM_SIZE_OF_QUEUE_INFO);
}

