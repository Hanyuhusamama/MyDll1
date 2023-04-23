#ifndef _MY_FILE_OPERATON_H_
#define _MY_FILE_OPERATON_H_

#include<iostream>

#ifndef MY_API
#ifdef _USRDLL
#define MY_API __declspec(dllexport)
#else
#define MY_API __declspec(dllimport)
#endif 
#endif // !MY_API
#if _WIN64 
#define _MY_PTR_STORE_TYPE unsigned long long int
#else 
#define MY_PTR_STORE_TYPE unsigned int
#endif

#define _GET_OFFSET(_POINTER_, _BASE_) (reinterpret_cast<_MY_PTR_STORE_TYPE>(_POINTER_)\
 - reinterpret_cast<_MY_PTR_STORE_TYPE>(_BASE_))
#define _GET_POINTER(_BASE_, _OFFSET_) reinterpret_cast<void*>(\
reinterpret_cast<_MY_PTR_STORE_TYPE>(_BASE_)+(_OFFSET_))

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
	void _Get_str_(char* buffer);
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
	static constexpr unsigned FILE_READ = 1;
	static constexpr unsigned FILE_WRITE = 2;
	static constexpr unsigned FILE_OPEN_EXISISTING = 4;
	typedef unsigned long long MyFilePointer;
	MyFile();
	MyFile(const char* _path, unsigned OpenFlag, MapMode _mode = MapAll);
	MyFile(const MyFile& _b) = delete;
	bool open(const char* filename, unsigned OpenFlag, MapMode _mode = MapAll);
	void close();
	void read(char* _Buffer, unsigned _count);
	void write(void* _src, unsigned _count);
	struct Myptr {
		void* ptr;
		unsigned _block_size;
	};
	MyFile& operator>>(char*);
	static void _nosafe_copy(void* dst, const void* src, unsigned cnt);
private:
	static constexpr unsigned FILE_OPEN_SUCCESSFUL = 8;
	bool _open(const char* path);
	void _close();
	void _write(unsigned long long write_at_where_, const char* Src, unsigned count);
	void _read(unsigned long long _at, char* buffer, unsigned count);
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

#endif // !_MY_FILE_OPERATON_H_
