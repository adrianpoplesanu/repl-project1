import time
import threading
from thread_worker import ad_worker_async, ad_worker_blocking

def thread_callback(thread_obj, arg_objs):
    thread_obj.callback = arg_objs[0]
    thread_obj.params = arg_objs[1:]

def thread_async_run(thread_obj, env):
    th1 = threading.Thread(target=ad_worker_async, args=(thread_obj.callback, thread_obj.params, thread_obj, env))
    thread_obj.thread = th1
    th1.start()

def thread_blocking_run(thread_obj, env):
    th1 = threading.Thread(target=ad_worker_blocking, args=(thread_obj.callback, thread_obj.params, thread_obj, env))
    thread_obj.thread = th1
    th1.start()
    th1.join()

def thread_await(thread_obj, env):
    thread_obj.thread.join()
    return thread_obj.result

def sleep_builtin_executor(value):
    time.sleep(value / 1000)
