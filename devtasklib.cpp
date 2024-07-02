#include "stdafx.h"

#include "devtasklib.h"
#include "devloglib.h"

#define COUNT_THREADS		    10
#define TASK_SLEEP_TIME			50

#define ACTIVE_BUSY				1
#define INACTIVE_BUSY			0

extern wchar_t app[ 260 ];

namespace wm_tasklib
{	
	static pthread_mutex_t tasks_mutex = PTHREAD_MUTEX_INITIALIZER;

	typedef struct callback {
		void(*func)(void*);
		void* param;
	} *ptrcallback;

	static std::vector<ptrcallback> queuefunc;

	struct paramthread {
		pthread_t tid;
		volatile int busy;
	};

	paramthread tid[ COUNT_THREADS ];

	void *executethread( void *arg )
	{
		volatile int* busy = (int*)arg;
		while ( *busy ) {
			ptrcallback ptr = 0;
			void  (*func)(void*) = 0;
			void* param = 0;
			pthread_mutex_lock( &tasks_mutex );
			if ( queuefunc.size() > 0 ) {
				ptr = queuefunc.back();
				queuefunc.pop_back();
			}
			if ( ptr == 0 ) {
				// msleep( TASK_SLEEP_TIME );
				pthread_mutex_unlock( &tasks_mutex );
				continue;
			}
			func = ptr->func;
			param = ptr->param;
			if ( func > 0 ) func( param );
			::free( ptr );
			pthread_mutex_unlock( &tasks_mutex );
		}
		pthread_exit(0);
		return 0;
	}
	
	int create()
	{
		int rc = 0;
		for ( int i = 0; i < COUNT_THREADS; i++ ) 
		{
			tid[i].busy = ACTIVE_BUSY;
			int rpc = pthread_create( &tid[i].tid, 0, executethread, (void*)&tid[i].busy );
			if ( rpc ) {
				rc = 0xff;
				wm_loglib::log( LOG_ERROR, L"asyncquery thread failed" );
				break;
			} else {
				rc = 0x00;
				wm_loglib::log( LOG_INFO, L"[ %s ] asyncquery thread:%02d created", app, i + 1 );
			}
		}
		return rc;
	}

	void destroy()
	{
		for ( int i = COUNT_THREADS; --i >= 0; ) {
			wm_loglib::log( LOG_DEBUG, L"[ %s ] asyncquery thread:%02d exit", app, i + 1 );
			tid[i].busy = INACTIVE_BUSY;
			pthread_join( tid[i].tid, 0 );
		}
	}

	void execute( void(*func)(void*), void* param )
	{
		ptrcallback ptr = (ptrcallback)::malloc( sizeof(callback) );
		if ( ptr ) {
			ptr->func = func;
			ptr->param = param;
			pthread_mutex_lock(&tasks_mutex);
			queuefunc.push_back(ptr);
			pthread_mutex_unlock(&tasks_mutex);
		}
	}
}