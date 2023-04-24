#ifndef _MY_FSM_H_
#define _MY_FSM_H_

#ifndef MY_API
#ifdef _USRDLL
#define MY_API __declspec(dllexport)
#else
#define MY_API __declspec(dllimport)
#endif 
#endif // !MY_API

typedef int (*_my_fsm_callback) (char a);

class MY_API MyFSM {
private:
	_my_fsm_callback* my_callbacks;
	int cb_cnt;
	int my_state;
	int _end_state;
public:
	MyFSM();
	MyFSM(const MyFSM& b);
	~MyFSM();
	void _do_tidy();
	void add_state(_my_fsm_callback _cb);
	void update(char a);
	void set_state(int _state);
	int get_state();
	void set_end_state(int _state);
	int process_str(const char* str);
	static MyFSM* CreateMyNumFsmint();
};

MY_API long long int __stdcall my_string_to_int(const char* _str);

MY_API unsigned long long __stdcall my_string_to_intu(const char* _str);

#endif // !_MY_FSM_H_

