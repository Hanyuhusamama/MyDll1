#include "pch.h"
#include "my_file_operatoin.h"
#include<xmmintrin.h>
#include<new>
#include<stdio.h>
#include<io.h>

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
    hFile = hMapFile = NULL;
    mode = MapAll;
    this->_file_size = 0;
    this->FileOpenFlag = 0;
    io_ptr = 0;
}

MyFile::MyFile(const char* _path, unsigned OpenFlag, MapMode _mode)
{
    hFile = hMapFile = NULL;
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

void MyFile::read(char* _Buffer, unsigned _count)
{
    _read(io_ptr, _Buffer, _count);
}

void MyFile::write(const void* _SRC, unsigned _count)
{
    if (io_ptr + _count >= _file_size) {
        _space_enlarge(_file_size + _count);
    }
    _write(io_ptr, (char*)_SRC,_count);
}

bool MyFile::_open(const char* path)
{
    do {
        if (!_MyCreateFile(path))break;
        create_file_map();
        if (hMapFile == NULL)break;
        FileOpenFlag |= FILE_OPEN_SUCCESSFUL;
        return true;
    } while (false);
    return false;
}

void MyFile::_close()
{
    CloseHandle(hMapFile);
    CloseHandle(hFile);
    hMapFile = hFile = NULL;
    FileOpenFlag &= (FILE_OPEN_SUCCESSFUL ^ 0xFFFFFFFF);
}

unsigned MyFile::_write(unsigned long long write_at_where, const char* Src, unsigned count)
{
    MyPointer ptr{ 0 };
    do {
        if (mode == MapAll) {
            ptr = _desired_access(write_at_where);
            if (ptr.base == NULL)break;
            memcpy_s(ptr.ptr,ptr.block_size, Src, count);
            UnmapViewOfFile(ptr.base);
            count = 0;
            break;
        }
        while (count > 0);
        {
            ptr = _desired_access(write_at_where);
            if (ptr.base == NULL)break;
            unsigned _copy_size = min(count, ptr.block_size);
            memcpy_s(ptr.ptr, ptr.block_size, Src, _copy_size);
            UnmapViewOfFile(ptr.base);
            Src += _copy_size;
            write_at_where += _copy_size;
            count -= _copy_size;
        }
    } while (false);
    return count;
}

unsigned MyFile::_read(unsigned long long read_at, char* buffer, unsigned count)
{
    MyPointer ptr;
    do {
        if (mode = MapAll) {
            ptr = _desired_access(read_at);
            if (ptr.base == NULL)break;
            memcpy_s(buffer, count, ptr.ptr, count);
            UnmapViewOfFile(ptr.base);
            count = 0;
            break;
        }
        while (count>0)
        {
            ptr = _desired_access(read_at);
            if (ptr.base == NULL)break;
            unsigned _read_size = min(ptr.block_size, count);
            memcpy_s(buffer, count, ptr.ptr, _read_size);
            UnmapViewOfFile(ptr.base);
            buffer += _read_size;
            count -= _read_size;
            read_at += _read_size;
        }
    } while (false);
    return count;
}

unsigned _my_cpy_s_code(char* buffer, const char* src, unsigned max_cnt, char* stopcode_table) {
    unsigned _count = 0;
    while (!stopcode_table[src[_count]]
        &&_count<max_cnt)
    {
        buffer[_count] = src[_count];
        ++_count;
    }
    return _count;
}

unsigned MyFile::_read_with_stopcode(unsigned long long _at, char* buffer, unsigned buffer_size, ...)
{
    char tf_table[256];
    ZeroMemory(tf_table, 256);
    va_list ap;
    va_start(ap, buffer_size);
    char _a = 0;
    do {
        _a = va_arg(ap, char);
        tf_table[_a] = 1;
    } while (_a != 0);
    va_end(ap);
    MyPointer ptr{ 0 };
    unsigned _cnt = 0;
    do
    {
        if (mode == MapAll) {
            ptr = _desired_access(_at);
            if (ptr.base == NULL)break;
            _cnt = _my_cpy_s_code(buffer,(char*)ptr.ptr, 
                min(buffer_size, ptr.block_size), tf_table);
            UnmapViewOfFile(ptr.base);
            break;
        }
        unsigned _read_size;
        do {
            ptr = _desired_access(_at);
            if (ptr.base == NULL)break;
            _read_size = _my_cpy_s_code(buffer, (char*)ptr.ptr,
                min(buffer_size, ptr.block_size), tf_table);
            buffer += _read_size;
            buffer_size -= _read_size;
            UnmapViewOfFile(ptr.base);
            _cnt += _read_size;
            _at += _read_size;
        } while (_read_size == ptr.block_size && buffer_size != 0);
    } while (false);
    return _cnt;
}

MyFile::MyPointer MyFile::_desired_access(unsigned long long _Where)
{
    MyPointer ret;
    if (mode == MapAll) {
        ret.base = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS,
            0, 0, _file_size);
        if (ret.base) { 
            ret.ptr = _GET_POINTER(ret.base, _Where); 
            ret.block_size = _file_size - _Where;
        }
        else { 
            ret.ptr = NULL; 
            ret.block_size = 0;
        }
    }
    else {
        unsigned long long _map_w = _Where - (_Where % MY_MAP_PAGE_SIZE);
        ret.base = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS,
            (unsigned)(_map_w >>32), (unsigned)_map_w
            , MY_MAP_PAGE_SIZE);
        if (ret.base) {
            ret.ptr = _GET_POINTER(ret.base, _Where % MY_MAP_PAGE_SIZE);
            ret.block_size = _map_w + MY_MAP_PAGE_SIZE - _Where;
        }
        else {
            ret.ptr = NULL;
            ret.block_size = 0;
        }
    }
    return ret;
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
    _finddata_t fd;
    auto _handle = _findfirst(_path, &fd);
    if (_handle != -1) {
        _file_size = fd.size;
        _findclose(_handle);
    }
    else {
        _file_size  = (mode == MapAll) ? 4096 : MY_MAP_PAGE_SIZE;
    }
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
    if (cnt < 32)goto _Copy_4;
    if (reinterpret_cast<_MY_PTR_STORE_TYPE>(dst) % 16 !=
        reinterpret_cast<_MY_PTR_STORE_TYPE>(src) % 16)goto _Copy_4;
    //copy 16
    while (reinterpret_cast<_MY_PTR_STORE_TYPE>(dst) % 16 != 0) {
        ((char*)dst)[0] = ((char*)src)[0];
        dst = reinterpret_cast<void*>(reinterpret_cast<_MY_PTR_STORE_TYPE>(dst) + 1);
        src = reinterpret_cast<const void*>(reinterpret_cast<_MY_PTR_STORE_TYPE>(src) + 1);
    }
    for (; cnt >= 16; cnt -= 16) {
        register __m128 _rt = _mm_load_ps((const float*)src);
        _mm_store_ps((float*)dst, _rt);
        dst = reinterpret_cast<void*>(reinterpret_cast<_MY_PTR_STORE_TYPE>(dst) + 16);
        src = reinterpret_cast<const void*>(reinterpret_cast<_MY_PTR_STORE_TYPE>(src) + 16);
    }
    goto _Normal_copy;
_Copy_4:
    if (reinterpret_cast<_MY_PTR_STORE_TYPE>(dst) % 4 !=
        reinterpret_cast<_MY_PTR_STORE_TYPE>(src) % 4)goto _Copy_4;
    while (reinterpret_cast<_MY_PTR_STORE_TYPE>(dst) % 4 != 0) {
        ((char*)dst)[0] = ((char*)src)[0];
        dst = reinterpret_cast<void*>(reinterpret_cast<_MY_PTR_STORE_TYPE>(dst) + 1);
        src = reinterpret_cast<const void*>(reinterpret_cast<_MY_PTR_STORE_TYPE>(src) + 1);
    }
    for (; cnt >= 4; cnt -= 4) {
        *((unsigned int*)dst) = *((const unsigned int*)src);
        dst = reinterpret_cast<void*>(reinterpret_cast<_MY_PTR_STORE_TYPE>(dst) + 4);
        src = reinterpret_cast<const void*>(reinterpret_cast<_MY_PTR_STORE_TYPE>(src) + 5);
    }
_Normal_copy:
    for (unsigned i = 0; i < cnt; ++i) {
        ((char*)dst)[i] = ((const char*)src)[i];
    }
}

unsigned MyFile::get_line(char* _buffer, unsigned buffer_size)
{
    unsigned _ret = (io_ptr==_file_size-1)?0:
    _read_with_stopcode(io_ptr, _buffer, buffer_size - 1, '\n', 0);
    _buffer[_ret] = '\0';
    io_ptr += _ret + 1;
    return _ret;
}

void MyFile::get_str(char* _Buffer, unsigned buffer_size)
{
    if (io_ptr == _file_size - 1)return;
    //ignore_spaces
    auto my_ptr = _desired_access(io_ptr);
    if (my_ptr.base == NULL)return;
    while (((char*)my_ptr.ptr)[0]==' '&&my_ptr.block_size>0)
    {
        ++io_ptr;
        my_ptr.ptr = reinterpret_cast<void*>(
            reinterpret_cast<_MY_PTR_STORE_TYPE>(my_ptr.ptr) + 1
            );
    }
    UnmapViewOfFile(my_ptr.base);
    unsigned cnt = _read_with_stopcode(io_ptr, _Buffer, buffer_size - 1,' ', '\n', ',' ,0);
    _Buffer[cnt] = 0;
    io_ptr += cnt + 1;
}

void MyFile::put_str(const char* str)
{
    unsigned len = strlen(str);
    write(str, len);
}

void MyFile::_space_enlarge(unsigned long long _nw_capa)
{
    _file_size = _nw_capa;
    create_file_map();
}

void MyFile::seek(int _seek_type, long long int offset) {
    switch (_seek_type)
    {
    case SEEK_SET:
        io_ptr = offset;
        break;
    case SEEK_CUR:
        io_ptr += offset;
        break;
    case SEEK_END:
        io_ptr = _file_size - offset;
    default:
        break;
    }
}
