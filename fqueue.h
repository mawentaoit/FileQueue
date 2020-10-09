#ifndef _FQUEUE_H_
#define _FQUEUE_H_

#include <memory>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstddef>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

class File_IO {
public:
	File_IO() = delete;
	File_IO(const File_IO &) = delete;
	File_IO& operator= (const File_IO&) = delete;
	File_IO(const char* fname);
	~File_IO();
	bool is_open();
	void close();
	std::size_t size();
	bool resize(std::size_t size);
	bool seek(std::size_t pos);
	bool write(const void* ptr, std::size_t size);
	bool read(void* ptr, std::size_t size);
	bool remove_file();
private:
	std::string m_fname;
	int m_fd;
};

class Fqueue {
public:
	Fqueue() = delete;
	Fqueue(const Fqueue &) = delete;
	Fqueue operator= (const Fqueue &) = delete;
	Fqueue(const char* fname, size_t size = 32);
	virtual ~Fqueue();

	//get them number of records in the data-file
	std::size_t records() ;


	std::size_t alloc_records() ; 

	bool empty() ;

	void reset();

	//clear records 
	void truncate();

	void push(const void* ptr, std::uint32_t size);

	//一个记录的结构
	struct record{
		std::unique_ptr<char[]> ptr;
		std::uint32_t size;
	};

	record front();

	record pop();

	record first_record() ;

	record next_record();

	//删除文件队列，即删除文件
	bool remove_fqueue();
private:
	//每个文件队列头部都有这个这样的信息对文件进行记录并控制
	struct queue_info {
		std::uint64_t rpos;
		std::uint64_t wpos;
		std::uint64_t records;
		std::uint64_t alloc_records;
	};
private: 
	File_IO m_file;
	const std::size_t m_fsize;

private:
	Fqueue::record read();
	void write_info(const queue_info &qi);
	void read_info(queue_info *qi);
private:
	enum { ENUM_SIZE_OF_QUEUE_INFO = sizeof(queue_info) };

	static_assert(ENUM_SIZE_OF_QUEUE_INFO == sizeof(std::uint64_t)*4, "bad sizeof(queue_info)");
};




#endif