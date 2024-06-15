from object_type import ObjectType

def ad_worker_async(raw_callback, params, thread_obj, env):
    if raw_callback.type == ObjectType.FUNCTION:
        from evaluator import Evaluator # this import sucks, but it works
        evaluator = Evaluator()
        # TODO: fix empty dict passed as kw_objs
        result = evaluator.apply_function(raw_callback, params, {}, env)
        thread_obj.result = result

def ad_worker_blocking(raw_callback, params, thread_obj, env):
    if raw_callback.type == ObjectType.FUNCTION:
        from evaluator import Evaluator # this import sucks, but it works
        evaluator = Evaluator()
        # TODO: fix empty dict passed as kw_objs
        result = evaluator.apply_function(raw_callback, params, {}, env)
        thread_obj.result = result
