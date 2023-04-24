#ifndef _MY_FILE_OPERATON_H_
#define _MY_FILE_OPERATON_H_

#ifndef MY_API
#ifdef _USRDLL
#define MY_API __declspec(dllexport)
#else
#define MY_API __declspec(dllimport)
#endif 
#endif // !MY_API

#ifndef _MY_PTR_STORE_TYPE
#if _WIN64 
#define _MY_PTR_STORE_TYPE unsigned long long int
#else 
#define MY_PTR_STORE_TYPE unsigned int
#endif
#endif

#define MY_MAP_PAGE_SIZE 65536

class MY_API MyMap {
public:
	_MY_PTR_STORE_TYPE Tag;
	void* BaseAddress;
	bool TagCmp(const void* _ptr);
	bool TagCmp(_MY_PTR_STORE_TYPE _ptr);
	void* AddressTranslate(_MY_PTR_STORE_TYPE Virtual_address);
	MyMap(_MY_PTR_STORE_TYPE Virtual, void* map_address);
	constexpr MyMap():Tag(0),BaseAddress(0){}
	MyMap(const MyMap&);
};

class MyRef {
private:
	int* count;
	int __cnt_;
public:
	int GetCount() {
		return *count;
	}
	MyRef(const MyRef& _ref) {
		count = const_cast<int*>(&_ref.__cnt_);
		__cnt_ = 0;
	}
	MyRef() {
		count = &__cnt_;
		__cnt_ = 1;
	}
	~MyRef() {
		--(*count);
	}
};

class MY_API MyMapArray {
public:
	using value_type = MyMap;
	using reference = MyMap&;
	using const_reference = const MyMap&;
	using size_type = unsigned int;
	using iterator = MyMap*;
	using const_iterator = const MyMap*;
	void _push_back(const MyMap& _x);
	void erase(iterator _begin, iterator _end);
	iterator begin();
	iterator end();
	unsigned size();
	unsigned capacity();
	MyMap* data();
	void clear();
	MyMapArray();
	MyMapArray(const MyMapArray&);
	~MyMapArray();
	MyMap& back();
	MyMap& at(unsigned indix);
private:
	MyMap* my_data;
	unsigned my_capacity;
	unsigned my_size;
	void _realloc(unsigned _new_capa);
};

class MY_API MyFile {
public:
	enum MapMode{
		MapAll,
		MapPart
	};
	struct MyPointer {
		void* ptr;
		void* base;
		unsigned block_size;
	};
	static constexpr unsigned FILE_READ = 1;
	static constexpr unsigned FILE_WRITE = 2;
	static constexpr unsigned FILE_OPEN_EXISISTING = 4;
	typedef unsigned long long MyFilePointer;
	MyFile();
	MyFile(const char* _path, unsigned OpenFlag, MapMode _mode = MapAll);
	MyFile(const MyFile& _b) = delete;
	~MyFile();
	bool open(const char* filename, unsigned OpenFlag, MapMode _mode = MapAll);
	void close();
	void read(char* _Buffer, unsigned _count);
	void write(const void* _src, unsigned _count);
	static void _nosafe_copy(void* dst, const void* src, unsigned cnt);
	//read, until \n NULL or EOF
	unsigned get_line(char* _buffer, unsigned buffer_size);
	void get_str(char* _Buffer, unsigned buffer_size);
	void put_str(const char* str);
	void seek(int _SeekAttribute, long long int _offset);
protected:
	static constexpr unsigned FILE_OPEN_SUCCESSFUL = 8;
	bool _open(const char* path);
	void _close();
	unsigned _write(unsigned long long write_at_where_, const char* Src, unsigned count);
	unsigned _read(unsigned long long _at, char* buffer, unsigned count);
	//the last parameter should be 0, do not add stop code!!!
	unsigned _read_with_stopcode(unsigned long long  _at, char* buffer,unsigned buffer_size, ...);
	MyPointer _desired_access(unsigned long long _Where);
private:
	HANDLE hFile;
	HANDLE hMapFile;
	MapMode mode;
	unsigned FileOpenFlag;
	MyFilePointer io_ptr;
	unsigned long long int _file_size;
	int _MyCreateFile(const char* _path);
	int _MyCreateFile(const wchar_t* _path);
	void create_file_map();
	MyMap _map_1(unsigned long long offset);
	void _space_enlarge(unsigned long long _nw_capa);
};

#define OVERRIDE_OPERATION

#ifdef OVERRIDE_OPERATION
#define _MY_OVERRIDE_OPERATORS_ 1
#else
#ifdef _IOSTREAM_
#define _MY_OVERRIDE_OPERATORS_ 1
#endif // _IOSTREAM_
#endif // !OVERRIDE_OPERATION
#if _MY_OVERRIDE_OPERATORS_
template<size_t N>
MyFile& operator>>(MyFile& _File, char(&_Buffer)[N]) {
	_File.get_str(_Buffer, N);
	return _File;
}

MyFile& operator>>(MyFile& _File, char* _Buffer) {
	_File.get_str(_Buffer, 0xFFFFFFFF);
	return _File;
}

MyFile& operator<< (MyFile& _File, const char* _str) {
	_File.put_str(_str);
	return _File;
}

#define _My_OVERRIDE_OPERA_LS(__TYPE__) MyFile& operator<< (MyFile& _File,__TYPE__ _val)

#define _My_OVERRIDE_OPERA_RS(__TYPE__) MyFile& operator>> (MyFile& _File,__TYPE__& _var)

#include<stdio.h>
#include<stdlib.h>

_My_OVERRIDE_OPERA_LS(int) {
	char _buffer[32];
	sprintf_s(_buffer, "%d", _val);
	_File.put_str(_buffer);
	return _File;
}

_My_OVERRIDE_OPERA_LS(unsigned int) {
	char _buffer[32];
	sprintf_s(_buffer, "%ud", _val);
	_File.put_str(_buffer);
	return _File;
}

_My_OVERRIDE_OPERA_LS(long long int) {
	char _buffer[64];
	sprintf_s(_buffer, "%lld", _val);
	_File.put_str(_buffer);
	return _File;
}

_My_OVERRIDE_OPERA_LS(unsigned long long int) {
	char _buffer[64];
	sprintf_s(_buffer, "%llu", _val);
	_File.put_str(_buffer);
	return _File;
}

_My_OVERRIDE_OPERA_LS(double) {
	char _buffer[32];
	sprintf_s(_buffer, "%f", _val);
	_File.put_str(_buffer);
	return _File;
}

_My_OVERRIDE_OPERA_LS(const void*) {
	char _buffer[32];
	sprintf_s(_buffer, "%p", _val);
	_File.put_str(_buffer);
	return _File;
}

MY_API long long int __stdcall my_string_to_int(const char* _str);

MY_API unsigned long long __stdcall my_string_to_intu(const char* _str);

_My_OVERRIDE_OPERA_RS(int) {
	char _buffer[32];
	_File >> _buffer;
	_var = (int)my_string_to_int(_buffer);
	return _File;
}

_My_OVERRIDE_OPERA_RS(long long) {
	char _buffer[64];
	_File >> _buffer;
	_var = my_string_to_int(_buffer);
	return _File;
}

_My_OVERRIDE_OPERA_RS(unsigned int) {
	char _buffer[32];
	_File >> _buffer;
	_var = (unsigned int)my_string_to_intu(_buffer);
	return _File;
}

_My_OVERRIDE_OPERA_RS(unsigned long long) {
	char _buffer[64];
	_File >> _buffer;
	_var = my_string_to_intu(_buffer);
	return _File;
}

_My_OVERRIDE_OPERA_RS(double) {
	char _buffer[64];
	_File >> _buffer;
	_var = atof(_buffer);
	return _File;
}

_My_OVERRIDE_OPERA_RS(void*) {
	char _buffer[64];
	_File >> _buffer;
	_var = reinterpret_cast<void*>(my_string_to_intu(_buffer));
	return _File;
}

#endif // !MY_OVERRIDE_OPERATORS




#endif // !_MY_FILE_OPERATON_H_
