from object_type import ObjectType

def ad_worker_async(raw_callback, params, env):
    if raw_callback.type == ObjectType.FUNCTION:
        from evaluator import Evaluator # this import sucks, but it works
        evaluator = Evaluator()
        evaluator.apply_function(raw_callback, params, env)

def ad_worker_blocking(raw_callback, params, env):
    if raw_callback.type == ObjectType.FUNCTION:
        from evaluator import Evaluator # this import sucks, but it works
        evaluator = Evaluator()
        evaluator.apply_function(raw_callback, params, env)
