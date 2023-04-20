#include "pch.h"
#include "my_file_operatoin.h"
#include<new>

#if _WIN64
#define _GET_PTR_HIGH_(_VAL_) (_VAL_ & 0xFFFFFFFFFFFF0000)
#else
#define _GET_PTR_HIGH_(_VAL_) (_VAL_ & 0xFFFF0000)
#endif // _WIN64

#define _GET_PTR_LOW_(_VAL_) (_VAL_ & 0xFFFF)

bool MyMap::TagCmp(const void* _ptr)
{
    return _GET_PTR_HIGH_(reinterpret_cast<_MY_PTR_STORE_TYPE>(_ptr)) == Tag;
}

bool MyMap::TagCmp(_MY_PTR_STORE_TYPE _ptr)
{
    return _GET_PTR_HIGH_(_ptr) == Tag;
}

void* MyMap::AddressTranslate(_MY_PTR_STORE_TYPE Virtual_address)
{
    return _GET_POINTER(BaseAddress, Virtual_address & 0xFFFF);
}

MyMap::MyMap(_MY_PTR_STORE_TYPE Virtual, void* map_address)
{
    Tag = _GET_PTR_HIGH_(Virtual);
    BaseAddress = map_address;
}

MyMap::MyMap(const MyMap&b)
{
    this->Tag = b.Tag;
    this->BaseAddress = b.BaseAddress;
}

void MyMapArray::_realloc(unsigned _new_capa)
{
    my_capacity = _new_capa;
    MyMap* _ND = (MyMap*)operator new(sizeof(MyMap) * _new_capa);
    memcpy_s(_ND, _new_capa * sizeof(MyMap), my_data, my_size * sizeof(MyMap));
    delete my_data;
    my_data = _ND;
}

void MyMapArray::_push_back(const MyMap& _x)
{
    if (my_size == my_capacity) {
        _realloc(my_size + my_size >> 2 + 1);
    }
    new (my_data + my_size) MyMap(_x);
    ++my_size;
}

void MyMapArray::erase(iterator _begin, iterator _end)
{
    auto diff = end() - _end;
    diff *= sizeof(MyMap);
    auto _size_block = end() - _begin;
    _size_block *= sizeof(MyMap);
    memmove_s(_begin, _size_block, _end, diff);
    my_size -= (_end - _begin);
}

MyMap* MyMapArray::begin()
{
    return  my_data;
}

MyMap* MyMapArray::end()
{
    return my_data + my_size;
}

unsigned MyMapArray::size()
{
    return my_size;
}

unsigned MyMapArray::capacity()
{
    return my_capacity;
}

MyMap* MyMapArray::data()
{
    return my_data;
}

void MyMapArray::clear()
{
    --my_size;
}

MyMapArray::MyMapArray()
{
    my_capacity = my_size = 0;
    my_data = NULL;
}

MyMapArray::MyMapArray(const MyMapArray&b)
{
    my_capacity = b.my_capacity;
    my_data = (MyMap*)operator new(my_capacity * sizeof(MyMap));
    my_size = b.my_size;
    memcpy_s(my_data, my_capacity * sizeof(MyMap),
        b.my_data, my_size * sizeof(MyMap));
}

MyMapArray::~MyMapArray()
{
    my_size = my_capacity = 0;
    
    delete my_data;
}

MyMap& MyMapArray::back()
{
    return my_data[my_size - 1];
}

MyMap& MyMapArray::at(unsigned indix)
{
    return my_data[indix];
}

MyFile::MyFile()
{
    hFile = hMapFile = base = NULL;
    mode = MapAll;
    this->_file_size = 0;
    this->FileOpenFlag = 0;
    io_ptr = 0;
}

MyFile::MyFile(const char* _path, unsigned OpenFlag, MapMode _mode)
{
    hFile = hMapFile =base = NULL;
    mode = _mode;
    this->_file_size = 0;
    this->FileOpenFlag = OpenFlag;
    _open(_path);
}

bool MyFile::open(const char* filename, unsigned OpenFlag, MapMode _mode)
{
    if (OpenFlag&FILE_OPEN_SUCCESSFUL)_close();
    FileOpenFlag = OpenFlag;
    mode = _mode;
    return _open(filename);
}

void MyFile::close()
{
    if (FileOpenFlag & FILE_OPEN_SUCCESSFUL)_close();
}

void MyFile::read(void* _Buffer, unsigned _count)
{
    auto ptr = GetioPointer();
    _nosafe_copy(_Buffer, ptr.ptr, _count);

}

void MyFile::write(void* _SRC, unsigned _count)
{
    if (io_ptr + _count >= _file_size) {
        _space_enlarge(_file_size + _count);
    }
    _write(io_ptr, _SRC,_count);
}

MyFile::Myptr MyFile::GetioPointer()
{
    Myptr ret;
    ret.ptr = _GET_POINTER(base, io_ptr);
    if (mode == MapAll) {
        ret._block_size = _file_size - io_ptr;
    }
    else {
        ret._block_size = MY_MAP_PAGE_SIZE - _GET_PTR_LOW_(io_ptr);
    }
    return ret;
    MyFile FILE;
}

MyFile& MyFile::operator>>(char*)
{
    return *this;
}

bool MyFile::_open(const char* path)
{
    do {
        if (!_MyCreateFile(path))break;
        create_file_map();
        if (hMapFile == NULL)break;
        if (mode == MapAll) {
            base = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, _file_size);
            if (!base) {
                CloseHandle(hFile);
                hFile = NULL;
                break;
            }
        }
        else {
            base = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, MY_MAP_PAGE_SIZE);
        }
        FileOpenFlag |= FILE_OPEN_SUCCESSFUL;
        return true;
    } while (false);
    return false;
}

void MyFile::_close()
{
    UnmapViewOfFile(base);
    CloseHandle(hMapFile);
    CloseHandle(hFile);
    hMapFile = hFile = NULL;
    FileOpenFlag &= (FILE_OPEN_SUCCESSFUL ^ 0xFFFFFFFF);
}

void MyFile::_write(unsigned long long write_at_where, const void* Src, unsigned count)
{
    if (mode == MapAll) {
        auto _size = _file_size - write_at_where;
        memcpy_s(
            _GET_POINTER(base, write_at_where), _size,
            Src, count);
        return;
    }
    void* ptr = 0;
    for(; count > MY_MAP_PAGE_SIZE; count -= MY_MAP_PAGE_SIZE) {
        ptr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS,
            (unsigned)(write_at_where >> 32), (unsigned)write_at_where,
            MY_MAP_PAGE_SIZE);
        if (ptr == NULL)return;
        memcpy_s(ptr, MY_MAP_PAGE_SIZE,
            Src, MY_MAP_PAGE_SIZE);
        UnmapViewOfFile(ptr);
        write_at_where += MY_MAP_PAGE_SIZE;
    }
    ptr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS,
        (unsigned)(write_at_where >> 32), (unsigned)write_at_where,
        MY_MAP_PAGE_SIZE);
    if (ptr == NULL)return;
    memcpy_s(ptr, MY_MAP_PAGE_SIZE,
        Src, count);
    UnmapViewOfFile(ptr);
    //for (; write_at_where < _write_end; write_at_where += MY_MAP_PAGE_SIZE);
}

int MyFile::_MyCreateFile(const char* _path)
{
    DWORD _open_para = (FileOpenFlag & FILE_OPEN_EXISISTING) ? OPEN_EXISTING : OPEN_ALWAYS;
    hFile = CreateFileA(_path,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        _open_para,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING,
        NULL);
    return hFile!=NULL;
}

int MyFile::_MyCreateFile(const wchar_t* _path)
{
    DWORD _open_para = (FileOpenFlag & FILE_OPEN_EXISISTING) ? OPEN_EXISTING : OPEN_ALWAYS;
    hFile = CreateFileW(_path,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        _open_para,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING,
        NULL);
    return hFile != NULL;
}

void MyFile::create_file_map()
{
    if ((this->hMapFile))CloseHandle(hMapFile);
    hMapFile = CreateFileMapping(hFile, NULL,
        PAGE_READWRITE,
        unsigned(_file_size >> 32), unsigned(_file_size),
        NULL);
}

MyMap MyFile::_map_1(unsigned long long offset)
{
    return MyMap(offset,
        MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS,
            unsigned(offset >> 32), (unsigned)offset, MY_MAP_PAGE_SIZE));
}

void MyFile::_nosafe_copy(void* dst, const void* src, unsigned cnt)
{
    for (unsigned i = 0; i < cnt; ++i) {
        ((char*)dst)[i] = ((const char*)src)[i];
    }
}

void MyFile::_space_enlarge(unsigned long long _nw_capa)
{
    _file_size = _nw_capa;
    UnmapViewOfFile(base);
    create_file_map();
    if (mode == MapAll) {
        base = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, _file_size);
    }
    else {
        base = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, MY_MAP_PAGE_SIZE);
    }
}
