
example_copystack: example_copystack.cpp
	g++ -g -std=c++11 -o example_copystack example_copystack.cpp coctx.cpp coctx_swap.S co_epoll.cpp co_hook_sys_call.cpp co_routine.cpp -ldl
example_thread: example_thread.cpp
	g++ -g -std=c++11 -o example_thread example_thread.cpp coctx.cpp coctx_swap.S co_epoll.cpp co_hook_sys_call.cpp co_routine.cpp -ldl -lpthread

example_echosvr: example_echosvr.cpp
	g++ -g -std=c++11 -o example_echosvr example_echosvr.cpp coctx.cpp coctx_swap.S co_epoll.cpp co_hook_sys_call.cpp co_routine.cpp -ldl

example_echocli: example_echocli.cpp
	g++ -g -std=c++11 -o example_echocli example_echocli.cpp coctx.cpp coctx_swap.S co_epoll.cpp co_hook_sys_call.cpp co_routine.cpp -ldl

example_echocli++: example_echocli++.cpp
	g++ -g -std=c++11 -o example_echocli++ example_echocli++.cpp event_loop.cpp connector.cpp sockets.cpp inner_pre.cpp co_epoll.cpp coctx.cpp coctx_swap.S co_routine.cpp co_hook_sys_call.cpp -ldl 

clean:
	rm -rf a.out .*.swp *.gch *.*~ example_thread example_copystack example_echocli example_echosvr example_echocli++ *.o 
