#include "pch.h"
#include "my_fsm.h"

MyFSM::MyFSM()
{
	my_callbacks = NULL;
	cb_cnt = 0;
	my_state = 0;
	_end_state = -2;
}

MyFSM::MyFSM(const MyFSM& b)
{
	my_callbacks = (_my_fsm_callback*) operator new(b.cb_cnt* sizeof(_my_fsm_callback));
	this->cb_cnt = b.cb_cnt;
	for (auto i = 0; i < cb_cnt; ++i) {
		my_callbacks[i] = b.my_callbacks[i];
	}
	my_state = b.my_state;
	_end_state = b._end_state;
}

MyFSM::~MyFSM()
{
	_do_tidy();
}

void MyFSM::_do_tidy(){
cb_cnt = 0;
if(my_callbacks)delete my_callbacks;
my_callbacks = 0;
}

void MyFSM::add_state(_my_fsm_callback _cb)
{
	if (_cb) {
		auto x = (_my_fsm_callback*) operator new((cb_cnt + 1) * sizeof(_my_fsm_callback));
		for (auto i = 0; i < cb_cnt; ++i) {
			x[i] = my_callbacks[i];
		}
		if(my_callbacks)operator delete(my_callbacks);
		my_callbacks = x;
		my_callbacks[cb_cnt] = _cb;
		++cb_cnt;
	}
}

void MyFSM::update(char a)
{
	if (my_state >= 0 && my_state < cb_cnt) {
		auto _next_state = my_callbacks[my_state](a);
		if (_next_state == -1)throw my_state;
		my_state = _next_state;
	}
	else {
		throw my_state;
	}
}

void MyFSM::set_state(int _state)
{
	my_state = _state;
}

int MyFSM::get_state()
{
	return my_state;
}

void MyFSM::set_end_state(int _state)
{
	_end_state = _state;
}

int MyFSM::process_str(const char* str)
{
	my_state = 0;
	unsigned indix = 0;
	while (str[indix])
	{
		try {
			update(str[indix]);
		}
		catch (int) {
			return -1;
		}
		++indix;
	}
	return my_state;
}

static char hex_inv_table[256];

static char hex_table[256];

void _my_table_initialize() {
	ZeroMemory(hex_inv_table, sizeof(char) * 256);
	ZeroMemory(hex_table, 256);
	char _val = 0;
	for (char a = '0'; a <= '9'; ++a) {
		hex_inv_table[a] = 1;
		hex_table[a] = _val;
		++_val;
	}
	for (char a = 'a'; a <= 'f'; ++a) {
		hex_inv_table[a] = 1;
		hex_table[a] = _val;
		++_val;
	}
	_val = 10;
	for (char a = 'A'; a <= 'F'; ++a) {
		hex_inv_table[a] = 1;
		hex_table[a] = _val;
		++_val;
	}
}

int my_fsm_callback_2(char a) {
	if (hex_inv_table[a] == 1)return 2;
	else return -1;
}

int my_fsm_callback_3(char a) {
	if (a == '0' || a == '1')return 3;
	else return -1;
}

int my_fsm_callback_4(char a) {
	if ('0' <= a && a <= '9')return 4;
	else return -1;
}

int my_fsm_callback_0(char a) {
	int ret;
	do {
		if (a == '0') {
			ret = 1;
			break;
		}
		if ('1' <= a && a <= '9') {
			ret = 4;
			break;
		}
		ret = -1;
		break;
	} while (0);
	return ret;
}

int my_fsm_callback_1(char a) {
	int ret;
	do
	{
		if (a == 'x' || a == 'X') {
			ret = 2;
			break;
		}
		if (a == 'b' || a == 'B') {
			ret = 3;
			break;
		}
		if ('0' <= a && a <= '9') {
			ret = 4;
			break;
		}
		ret = -1;
	} while (false);
	return ret;
}

MyFSM* MyFSM::CreateMyNumFsmint()
{
	auto ret = new MyFSM();
	ret->cb_cnt = 5;
	ret->my_callbacks = (_my_fsm_callback*)operator new(5 * sizeof(void*));
	ret->my_callbacks[0] = &my_fsm_callback_0;
	ret->my_callbacks[1] = &my_fsm_callback_1;
	ret->my_callbacks[2] = &my_fsm_callback_2;
	ret->my_callbacks[3] = &my_fsm_callback_3;
	ret->my_callbacks[4] = &my_fsm_callback_4;
	return ret;
}

long long int _my_stoi_2(const char* _str, int _dxdx) {
	long long int _ret = 0;
	while (_str[0])
	{
		_ret *= _dxdx;
		_ret += hex_table[*_str];
		++_str;
	}
	return _ret;
}

unsigned long long int _my_stoi_1(const char* _str, unsigned _dxdx) {
	unsigned long long int _ret = 0;
	while (_str[0])
	{
		_ret *= _dxdx;
		_ret += hex_table[*_str];
		++_str;
	}
	return _ret;
}

static MyFSM* _fsm_numtype=NULL;

void _number_fsm_init() {
	_fsm_numtype = MyFSM::CreateMyNumFsmint();
}

void _number_fsm_delete() {
	_fsm_numtype->_do_tidy();
	operator delete(_fsm_numtype);
}

MY_API long long int __stdcall my_string_to_int(const char* _str)
{
	long long int _ret;
	bool _sign = _str[0] == '-';
	if (_sign)++_str;
	int _type = _fsm_numtype->process_str(_str);
	switch (_type)
	{
	case 2:
		_ret = _my_stoi_2(_str, 16);
		break;
	case 3:
		_ret = _my_stoi_2(_str, 2);
		break;
	case 4:
		_ret = _my_stoi_2(_str, 10);
		break;
	default:
		_ret = 0;
	}
	if (_sign)_ret = -_ret;
	return _ret;
}

unsigned long long __stdcall my_string_to_intu(const char* _str)
{
	unsigned long long int _ret;
	int _type = _fsm_numtype->process_str(_str);
	switch (_type)
	{
	case 2:
		_ret = _my_stoi_1(_str, 16);
		break;
	case 3:
		_ret = _my_stoi_1(_str, 2);
		break;
	case 4:
		_ret = _my_stoi_1(_str, 10);
		break;
	default:
		_ret = 0;
	}
	return _ret;
}
