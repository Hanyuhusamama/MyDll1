#ifndef _MY_FSM_H_
#define _MY_FSM_H_

#ifndef MY_API
#ifdef _USRDLL
#define MY_API __declspec(dllexport)
#else
#define MY_API __declspec(dllimport)
#endif 
#endif // !MY_API

template<class... ArgT>
class my_fsm_callback_function {
	ArgT my_arguments;

};

#endif // !_MY_FSM_H_

