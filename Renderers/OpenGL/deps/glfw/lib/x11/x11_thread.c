//========================================================================
// GLFW - An OpenGL framework
// File:        x11_thread.c
// Platform:    X11 (Unix)
// API version: 2.5
// Author:      Marcus Geelnard (marcus.geelnard at home.se)
// WWW:         http://glfw.sourceforge.net
//------------------------------------------------------------------------
// Copyright (c) 2002-2005 Marcus Geelnard
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
// Marcus Geelnard
// marcus.geelnard at home.se
//------------------------------------------------------------------------
// $Id: x11_thread.c,v 1.6 2005/03/14 20:26:17 marcus256 Exp $
//========================================================================

#include "internal.h"



//************************************************************************
//****                  GLFW internal functions                       ****
//************************************************************************

#ifdef _GLFW_HAS_PTHREAD

//========================================================================
// _glfwNewThread() - This is simply a "wrapper" for calling the user
// thread function.
//========================================================================

void * _glfwNewThread( void * arg )
{
    GLFWthreadfun threadfun;
    _GLFWthread   *t;
    pthread_t   posixID;

    // Get current thread ID
    posixID = pthread_self();

    // Enter critical section
    ENTER_THREAD_CRITICAL_SECTION

    // Loop through entire list of threads to find the matching POSIX
    // thread ID
    for( t = &_glfwThrd.First; t != NULL; t = t->Next )
    {
        if( t->PosixID == posixID )
        {
            break;
        }
    }
    if( t == NULL )
    {
        LEAVE_THREAD_CRITICAL_SECTION
        return NULL;
    }

    // Get user thread function pointer
    threadfun = t->Function;

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION

    // Call the user thread function
    threadfun( arg );

    // Remove thread from thread list
    ENTER_THREAD_CRITICAL_SECTION
    _glfwRemoveThread( t );
    LEAVE_THREAD_CRITICAL_SECTION

    // When the thread function returns, the thread will die...
    return NULL;
}

#endif // _GLFW_HAS_PTHREAD



//************************************************************************
//****               Platform implementation functions                ****
//************************************************************************

//========================================================================
// _glfwPlatformCreateThread() - Create a new thread
//========================================================================

GLFWthread _glfwPlatformCreateThread( GLFWthreadfun fun, void *arg )
{
#ifdef _GLFW_HAS_PTHREAD

    GLFWthread  ID;
    _GLFWthread *t;
    int         result;

    // Enter critical section
    ENTER_THREAD_CRITICAL_SECTION

    // Create a new thread information memory area
    t = (_GLFWthread *) malloc( sizeof(_GLFWthread) );
    if( t == NULL )
    {
        // Leave critical section
        LEAVE_THREAD_CRITICAL_SECTION
        return -1;
    }

    // Get a new unique thread id
    ID = _glfwThrd.NextID ++;

    // Store thread information in the thread list
    t->Function = fun;
    t->ID       = ID;

    // Create thread
    result = pthread_create(
        &t->PosixID,      // Thread handle
        NULL,             // Default thread attributes
        _glfwNewThread,   // Thread function (a wrapper function)
        (void *)arg       // Argument to thread is user argument
    );

    // Did the thread creation fail?
    if( result != 0 )
    {
        free( (void *) t );
        LEAVE_THREAD_CRITICAL_SECTION
        return -1;
    }

    // Append thread to thread list
    _glfwAppendThread( t );

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION

    // Return the GLFW thread ID
    return ID;

#else

    return -1;

#endif // _GLFW_HAS_PTHREAD
}


//========================================================================
// _glfwPlatformDestroyThread() - Kill a thread. NOTE: THIS IS A VERY
// DANGEROUS OPERATION, AND SHOULD NOT BE USED EXCEPT IN EXTREME
// SITUATIONS!
//========================================================================

void _glfwPlatformDestroyThread( GLFWthread ID )
{
#ifdef _GLFW_HAS_PTHREAD

    _GLFWthread *t;

    // Enter critical section
    ENTER_THREAD_CRITICAL_SECTION

    // Get thread information pointer
    t = _glfwGetThreadPointer( ID );
    if( t == NULL )
    {
        LEAVE_THREAD_CRITICAL_SECTION
        return;
    }

    // Simply murder the process, no mercy!
    pthread_kill( t->PosixID, SIGKILL );

    // Remove thread from thread list
    _glfwRemoveThread( t );

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION

#endif // _GLFW_HAS_PTHREAD
}


//========================================================================
// _glfwPlatformWaitThread() - Wait for a thread to die
//========================================================================

int _glfwPlatformWaitThread( GLFWthread ID, int waitmode )
{
#ifdef _GLFW_HAS_PTHREAD

    pthread_t   thread;
    _GLFWthread *t;

    // Enter critical section
    ENTER_THREAD_CRITICAL_SECTION

    // Get thread information pointer
    t = _glfwGetThreadPointer( ID );

    // Is the thread already dead?
    if( t == NULL )
    {
        LEAVE_THREAD_CRITICAL_SECTION
        return GL_TRUE;
    }

    // If got this far, the thread is alive => polling returns FALSE
    if( waitmode == GLFW_NOWAIT )
    {
        LEAVE_THREAD_CRITICAL_SECTION
        return GL_FALSE;
    }

    // Get thread handle
    thread = t->PosixID;

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION

    // Wait for thread to die
    (void) pthread_join( thread, NULL );

    return GL_TRUE;

#else

    return GL_TRUE;

#endif // _GLFW_HAS_PTHREAD
}


//========================================================================
// _glfwPlatformGetThreadID() - Return the thread ID for the current
// thread
//========================================================================

GLFWthread _glfwPlatformGetThreadID( void )
{
#ifdef _GLFW_HAS_PTHREAD

    _GLFWthread *t;
    GLFWthread  ID = -1;
    pthread_t   posixID;

    // Get current thread ID
    posixID = pthread_self();

    // Enter critical section
    ENTER_THREAD_CRITICAL_SECTION

    // Loop through entire list of threads to find the matching POSIX
    // thread ID
    for( t = &_glfwThrd.First; t != NULL; t = t->Next )
    {
        if( t->PosixID == posixID )
        {
            ID = t->ID;
            break;
        }
    }

    // Leave critical section
    LEAVE_THREAD_CRITICAL_SECTION

    // Return the found GLFW thread identifier
    return ID;

#else

    return 0;

#endif // _GLFW_HAS_PTHREAD
}


//========================================================================
// _glfwPlatformCreateMutex() - Create a mutual exclusion object
//========================================================================

GLFWmutex _glfwPlatformCreateMutex( void )
{
#ifdef _GLFW_HAS_PTHREAD

    pthread_mutex_t *mutex;

    // Allocate memory for mutex
    mutex = (pthread_mutex_t *) malloc( sizeof( pthread_mutex_t ) );
    if( !mutex )
    {
        return NULL;
    }

    // Initialise a mutex object
    (void) pthread_mutex_init( mutex, NULL );

    // Cast to GLFWmutex and return
    return (GLFWmutex) mutex;

#else

    return (GLFWmutex) 0;

#endif // _GLFW_HAS_PTHREAD
}


//========================================================================
// _glfwPlatformDestroyMutex() - Destroy a mutual exclusion object
//========================================================================

void _glfwPlatformDestroyMutex( GLFWmutex mutex )
{
#ifdef _GLFW_HAS_PTHREAD

    // Destroy the mutex object
    pthread_mutex_destroy( (pthread_mutex_t *) mutex );

    // Free memory for mutex object
    free( (void *) mutex );

#endif // _GLFW_HAS_PTHREAD
}


//========================================================================
// _glfwPlatformLockMutex() - Request access to a mutex
//========================================================================

void _glfwPlatformLockMutex( GLFWmutex mutex )
{
#ifdef _GLFW_HAS_PTHREAD

    // Wait for mutex to be released
    (void) pthread_mutex_lock( (pthread_mutex_t *) mutex );

#endif // _GLFW_HAS_PTHREAD
}


//========================================================================
// _glfwPlatformUnlockMutex() - Release a mutex
//========================================================================

void _glfwPlatformUnlockMutex( GLFWmutex mutex )
{
#ifdef _GLFW_HAS_PTHREAD

    // Release mutex
    pthread_mutex_unlock( (pthread_mutex_t *) mutex );

#endif // _GLFW_HAS_PTHREAD
}


//========================================================================
// _glfwPlatformCreateCond() - Create a new condition variable object
//========================================================================

GLFWcond _glfwPlatformCreateCond( void )
{
#ifdef _GLFW_HAS_PTHREAD

    pthread_cond_t *cond;

    // Allocate memory for condition variable
    cond = (pthread_cond_t *) malloc( sizeof(pthread_cond_t) );
    if( !cond )
    {
        return NULL;
    }

    // Initialise condition variable
    (void) pthread_cond_init( cond, NULL );

    // Cast to GLFWcond and return
    return (GLFWcond) cond;

#else

    return (GLFWcond) 0;

#endif // _GLFW_HAS_PTHREAD
}


//========================================================================
// _glfwPlatformDestroyCond() - Destroy a condition variable object
//========================================================================

void _glfwPlatformDestroyCond( GLFWcond cond )
{
#ifdef _GLFW_HAS_PTHREAD

    // Destroy the condition variable object
    (void) pthread_cond_destroy( (pthread_cond_t *) cond );

    // Free memory for condition variable object
    free( (void *) cond );

#endif // _GLFW_HAS_PTHREAD
}


//========================================================================
// _glfwPlatformWaitCond() - Wait for a condition to be raised
//========================================================================

void _glfwPlatformWaitCond( GLFWcond cond, GLFWmutex mutex,
    double timeout )
{
#ifdef _GLFW_HAS_PTHREAD

    struct timeval  currenttime;
    struct timespec wait;
    long dt_sec, dt_usec;

    // Select infinite or timed wait
    if( timeout >= GLFW_INFINITY )
    {
        // Wait for condition (infinite wait)
        (void) pthread_cond_wait( (pthread_cond_t *) cond,
                  (pthread_mutex_t *) mutex );
    }
    else
    {
        // Set timeout time, relatvie to current time
        gettimeofday( &currenttime, NULL );
        dt_sec  = (long) timeout;
        dt_usec = (long) ((timeout - (double)dt_sec) * 1000000.0);
        wait.tv_nsec = (currenttime.tv_usec + dt_usec) * 1000L;
        if( wait.tv_nsec > 1000000000L )
        {
            wait.tv_nsec -= 1000000000L;
            dt_sec ++;
        }
        wait.tv_sec  = currenttime.tv_sec + dt_sec;

        // Wait for condition (timed wait)
        (void) pthread_cond_timedwait( (pthread_cond_t *) cond,
                   (pthread_mutex_t *) mutex, &wait );
    }

#endif // _GLFW_HAS_PTHREAD
}


//========================================================================
// _glfwPlatformSignalCond() - Signal a condition to one waiting thread
//========================================================================

void _glfwPlatformSignalCond( GLFWcond cond )
{
#ifdef _GLFW_HAS_PTHREAD

    // Signal condition
    (void) pthread_cond_signal( (pthread_cond_t *) cond );

#endif // _GLFW_HAS_PTHREAD
}


//========================================================================
// _glfwPlatformBroadcastCond() - Broadcast a condition to all waiting
// threads
//========================================================================

void _glfwPlatformBroadcastCond( GLFWcond cond )
{
#ifdef _GLFW_HAS_PTHREAD

    // Broadcast condition
    (void) pthread_cond_broadcast( (pthread_cond_t *) cond );

#endif // _GLFW_HAS_PTHREAD
}


//========================================================================
// _glfwPlatformGetNumberOfProcessors() - Return the number of processors
// in the system.
//========================================================================

int _glfwPlatformGetNumberOfProcessors( void )
{
    int n;

    // Get number of processors online
    _glfw_numprocessors( n );
    return n;
}
