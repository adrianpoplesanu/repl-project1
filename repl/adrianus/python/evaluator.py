from objects import Ad_Null_Object, Ad_Integer_Object, Ad_Boolean_Object, \
                    Ad_String_Object, Ad_ReturnValue_Object, Ad_Function_Object, \
                    Ad_Error_Object, Ad_List_Object, Ad_Hash_Object, Hash_Pair, \
                    Ad_Class_Object, Ad_Class_Instance
from object_type import ObjectType
from ast import StatementType
from environment import new_environment, new_enclosed_environment
from builtins import builtins_map
from utils import print_ast_nodes

TRUE = Ad_Boolean_Object(value=True)
FALSE = Ad_Boolean_Object(value=False)


class Evaluator(object):
    def __init__(self):
        pass

    def eval(self, node, env):
        if node.type == StatementType.PROGRAM:
            return self.eval_program(node, env)
        elif node.type == StatementType.LET_STATEMENT:
            obj = self.eval(node.value, env)
            env.set(node.name.value, obj)
            return None
        elif node.type == StatementType.ASSIGN_STATEMENT:
            return self.eval_assign_statement(node, env)
        elif node.type == StatementType.RETURN_STATEMENT:
            val = self.eval(node.value, env)
            obj = Ad_ReturnValue_Object(value=val)
            return obj
        elif node.type == StatementType.EXPRESSION_STATEMENT:
            if node.expression:
                return self.eval(node.expression, env)
            return None
        elif node.type == StatementType.IDENTIFIER:
            return self.eval_identifier(node, env)
        elif node.type == StatementType.INTEGER:
            return self.eval_integer(node, env)
        elif node.type == StatementType.BOOLEAN:
            return self.eval_boolean(node, env)
        elif node.type == StatementType.INFIX_EXPRESSION:
            left = self.eval(node.left, env)
            right = self.eval(node.right, env)
            result = self.eval_infix_expression(node.operator, left, right)
            return result
        elif node.type == StatementType.PREFIX_EXPRESSION:
            right = self.eval(node.right, env)
            return self.eval_prefix_expression(node.operator, right)
        elif node.type == StatementType.IF_EXPRESSION:
            return self.eval_if_expression(node, env)
        elif node.type == StatementType.BLOCK_STATEMENT:
            return self.eval_block_statement(node, env)
        elif node.type == StatementType.FUNCTION_LITERAL:
            obj = Ad_Function_Object(parameters=node.parameters, body=node.body, env=env)
            return obj
        elif node.type == StatementType.CALL_EXPRESSION:
            # TODO: i need to check if this is a class constructor
            func = self.eval(node.func, env)
            if self.is_error(func):
                return func
            args_objs = self.eval_expressions(node.arguments, env)
            if len(args_objs) == 1 and self.is_error(args_objs[0]):
                return args_objs[0]
            return self.apply_function(func, args_objs, env)
        elif node.type == StatementType.WHILE_EXPRESSION:
            return self.eval_while_expression(node, env)
        elif node.type == StatementType.STRING_LITERAL:
            return self.eval_string(node, env)
        elif node.type == StatementType.LIST_LITERAL:
            elements = self.eval_expressions(node.elements, env)
            if len(elements) == 1 and self.is_error(elements[0]):
                return elements[0]
            list_obj = Ad_List_Object(elements=elements)
            return list_obj
        elif node.type == StatementType.INDEX_EXPRESSION:
            left = self.eval(node.left, env)
            if self.is_error(left):
                return left
            index = self.eval(node.index, env)
            if self.is_error(index):
                return index
            return self.eval_index_expression(left, index)
        elif node.type == StatementType.HASH_LITERAL:
            return self.eval_hash_literal(node, env)
        elif node.type == StatementType.DEF_STATEMENT:
            return self.eval_def_statement(node, env)
        elif node.type == StatementType.CLASS_STATEMENT:
            return self.eval_class_statement(node, env)
        elif node.type == StatementType.MEMBER_ACCESS:
            return self.eval_member_access(node, env)
        elif node.type == StatementType.COMMENT:
            # this is just a comment, do nothing
            return None
        elif node.type == StatementType.PREFIX_INCREMENT:
            return self.eval_prefix_increment(node, env)
        else:
            print 'unknown AST node: ' + node.type

    def eval_program(self, node, env):
        for statement in node.statements:
            result = self.eval(statement, env)
            if result:
                print result.inspect()

    def eval_identifier(self, node, env):
        if env.check(node.token.literal):
            return env.get(node.token.literal)
        if node.token.literal in builtins_map:
            return builtins_map[node.value]
        return None

    def eval_integer(self, node, env):
        obj = Ad_Integer_Object(value=node.value)
        return obj

    def eval_boolean(self, node, env):
        obj = Ad_Boolean_Object(value=node.value)
        return obj

    def eval_string(self, node, env):
        obj = Ad_String_Object(value=node.value)
        return obj

    def eval_infix_expression(self, operator, left, right):
        if left.type == ObjectType.INTEGER and right.type == ObjectType.INTEGER:
            return self.eval_integer_infix_expression(operator, left, right)
        if left.type == ObjectType.STRING and right.type == ObjectType.STRING:
            return self.eval_string_infix_expression(operator, left, right)
        if left.type == ObjectType.BOOLEAN and right.type == ObjectType.BOOLEAN:
            return self.eval_boolean_infix_expression(operator, left, right)
        #if operator == '==':
        #    return self.native_bool_to_boolean_object(left == right)
        #if operator == '!=':
        #    return self.native_bool_to_boolean_object(left != right)

    def eval_integer_infix_expression(self, operator, left, right):
        left_val = left.value
        right_val = right.value
        if operator == '+':
            return Ad_Integer_Object(value=left_val + right_val)
        if operator == '-':
            return Ad_Integer_Object(value=left_val - right_val)
        if operator == '*':
            return Ad_Integer_Object(value=left_val * right_val)
        if operator == '/':
            return Ad_Integer_Object(value=left_val / right_val)
        if operator == '<':
            return self.native_bool_to_boolean_object(left_val < right_val)
        if operator == '>':
            return self.native_bool_to_boolean_object(left_val > right_val)
        if operator == '<=':
            return self.native_bool_to_boolean_object(left_val <= right_val)
        if operator == '>=':
            return self.native_bool_to_boolean_object(left_val >= right_val)
        if operator == '==':
            return self.native_bool_to_boolean_object(left_val == right_val)
        if operator == '!=':
            return self.native_bool_to_boolean_object(left_val != right_val)
        return None

    def native_bool_to_boolean_object(self, value):
        if value:
            return TRUE
        return FALSE

    def eval_prefix_expression(self, operator, right):
        if operator == '!':
            return self.eval_bang_operator_expression(right)
        if operator == '-':
            return self.eval_minus_operator_expression(right)
        return self.new_error("unkowm prefix expression for this type")

    def eval_bang_operator_expression(self, right):
        if not right:
            return TRUE
        if right.type == ObjectType.BOOLEAN:
            if right == TRUE:
                return FALSE
            if right == FALSE:
                return TRUE
        return FALSE

    def eval_minus_operator_expression(self, right):
        if right.type != ObjectType.INTEGER:
            return None
        val = right.value
        return Ad_Integer_Object(value=-val)

    def eval_if_expression(self, node, env):
        condition = self.eval(node.condition, env)
        if self.is_error(condition):
            return None
        if self.is_truthy(condition):
            return self.eval(node.consequence, env)
        else:
            if node.alternative:
                return self.eval(node.alternative, env)
        return None

    def eval_block_statement(self, node, env):
        result = None
        for statement in node.statements:
            result = self.eval(statement, env)
            if result and result.type == ObjectType.RETURN_VALUE:
                return result
        return result

    def eval_while_expression(self, node, env):
        condition = self.eval(node.condition, env)
        if self.is_error(condition):
            return None
        while self.is_truthy(condition):
            self.eval(node.block, env)
            condition = self.eval(node.condition, env)

    def is_truthy(self, obj):
        if not obj:
            return False
        if obj == TRUE:
            return True
        if obj == FALSE:
            return False
        if obj.type == ObjectType.NULL:
            return False
        if obj.type == ObjectType.INTEGER:
            if obj.value != 0:
                return True
            else:
                return False
        if obj.type == ObjectType.STRING:
            if obj.value != '':
                return True
            else:
                return False
        return True

    def eval_expressions(self, args, env):
        res = []
        for arg in args:
            evaluated = self.eval(arg, env)
            if self.is_error(evaluated):
                error_res = [evaluated]
                return error_res
            res.append(evaluated)
        return res

    def apply_function(self, func, args_objs, env):
        if func.type == ObjectType.FUNCTION:
            extended_env = self.extend_function_env(func, args_objs)
            evaluated = self.eval(func.body, extended_env)
            return self.unwrap_return_value(evaluated)
        if func.type == ObjectType.BUILTIN:
            return func.builtin_function(args_objs, env) # asta ar putea fi si func.builtin_function(*args_objs)
            # intrebarea e prefer sa pasez o lista de argumente catre bultin, sau argumente pozitionale, explodate in apelul functiei
        if func.type == ObjectType.CLASS:
            instance_environment = new_environment()
            klass_instance = Ad_Class_Instance(name=func.name.value, class_object=func, instance_environment=instance_environment)
            for attribute in func.attributes:
                if attribute.type == StatementType.ASSIGN_STATEMENT:
                    instance_environment.outer = env.store
                    evaluated = self.eval(attribute.value, klass_instance.instance_environment)
                    key = attribute.name.value
                    klass_instance.instance_environment.set(key, evaluated)
                if attribute.type == StatementType.EXPRESSION_STATEMENT:
                    if attribute.expression.type == StatementType.ASSIGN_STATEMENT:
                        instance_environment.outer = env.store
                        evaluated = self.eval(attribute.expression.value, klass_instance.instance_environment)
                        key = attribute.expression.name.value
                        klass_instance.instance_environment.set(key, evaluated)
            for method in func.methods:
                func_obj = Ad_Function_Object(parameters=method.parameters, body=method.body, env=klass_instance.instance_environment)
                klass_instance.instance_environment.set(method.name.value, func_obj)
            # i also need to call the class constructor, if one is present
            return klass_instance
        return None

    def apply_method(self, func, args_objs, env):
        if func.type == ObjectType.FUNCTION:
            self.extend_method_env(func, args_objs, env)
            evaluated = self.eval(func.body, env)
            return self.unwrap_return_value(evaluated)
        if func.type == ObjectType.BUILTIN:
            return func.builtin_function(args_objs, env) # asta ar putea fi si func.builtin_function(*args_objs)
            # intrebarea e prefer sa pasez o lista de argumente catre bultin, sau argumente pozitionale, explodate in apelul functiei
        if func.type == ObjectType.CLASS:
            instance_environment = new_environment()
            klass_instance = Ad_Class_Instance(name=func.name.value, class_object=func, instance_environment=instance_environment)
            for attribute in func.attributes:
                if attribute.type == StatementType.ASSIGN_STATEMENT:
                    instance_environment.outer = env.store
                    evaluated = self.eval(attribute.value, klass_instance.instance_environment)
                    key = attribute.name.value
                    klass_instance.instance_environment.set(key, evaluated)
                if attribute.type == StatementType.EXPRESSION_STATEMENT:
                    if attribute.expression.type == StatementType.ASSIGN_STATEMENT:
                        instance_environment.outer = env.store
                        evaluated = self.eval(attribute.expression.value, klass_instance.instance_environment)
                        key = attribute.expression.name.value
                        klass_instance.instance_environment.set(key, evaluated)
            for method in func.methods:
                func_obj = Ad_Function_Object(parameters=method.parameters, body=method.body, env=klass_instance.instance_environment)
                klass_instance.instance_environment.set(method.name.value, func_obj)
            # i also need to call the class constructor, if one is present
            return klass_instance
        return None

    def unwrap_return_value(self, obj):
        if obj and obj.type == ObjectType.RETURN_VALUE:
            return obj.value
        return obj

    def extend_function_env(self, func, args_objs):
        extended = new_enclosed_environment(func.env)
        for i, param in enumerate(func.parameters):
            extended.set(param.token_literal(), args_objs[i])
        return extended

    def extend_method_env(self, func, args_objs, env):
        for i, param in enumerate(func.parameters):
            env.set(param.token_literal(), args_objs[i])

    def eval_boolean_infix_expression(self, operator, left, right):
        left_val = left.value
        right_val = right.value
        if operator == 'and':
            return Ad_Boolean_Object(value=left_val and right_val)
        if operator == 'or':
            return Ad_Boolean_Object(value=left_val or right_val)

    def eval_string_infix_expression(self, operator, left, right):
        if operator == "+":
            val = left.value + right.value
            return Ad_String_Object(value=val)

    def new_error(self, msg):
        return Ad_Error_Object(message=msg)

    def is_error(self, obj):
        return obj.type == ObjectType.ERROR

    def eval_index_expression(self, left, index):
        if left.type == ObjectType.LIST and index.type == ObjectType.INTEGER:
            return self.eval_list_index_expression(left, index)
        if left.type == ObjectType.HASH:
            return self.eval_hash_index_expressions(left, index)
        return self.new_error("index expression error for type: {0}".format(left.type))

    def eval_list_index_expression(self, left, index):
        list_obj = left
        idx = index.value
        max = len(list_obj.elements) - 1
        if idx < 0 or idx > max:
            return None
        return list_obj.elements[idx]

    def eval_hash_literal(self, node, env):
        pairs = {}
        for node_key in node.pairs.keys():
            node_value = node.pairs[node_key]
            key = self.eval(node_key, env)
            if self.is_error(key):
                return key
            hashed = key.hash_key()
            value = self.eval(node_value, env)
            if self.is_error(value):
                return value
            pairs[hashed.value] = Hash_Pair(key=key, value=value)
        return Ad_Hash_Object(pairs=pairs)

    def eval_hash_index_expressions(self, left, index):
        hashed = index.hash_key()
        pair = left.pairs[hashed.value]
        return pair.value

    def eval_assign_statement(self, node, env):
        if node.name.type == StatementType.INDEX_EXPRESSION:
            self.eval_index_expression_assign(node, env)
        elif node.name.type == StatementType.MEMBER_ACCESS:
            klass_instance = env.get(node.name.owner.value)
            klass_member = node.name.member
            klass_environment = klass_instance.instance_environment
            obj = self.eval(node.value, env)
            klass_environment.set(klass_member.value, obj)
        else:
            obj = self.eval(node.value, env)
            env.set(node.name.value, obj)
        return None

    def eval_index_expression_assign(self, node, env):
        obj = self.eval(node.name.left, env)
        if self.is_error(obj):
            return obj
        index = self.eval(node.name.index, env)
        if self.is_error(index):
            return index
        if obj.type == ObjectType.LIST:
            idx = index.value
            value = self.eval(node.value, env)
            obj.elements[idx] = value
        elif obj.type == ObjectType.HASH:
            value = self.eval(node.value, env)
            hashed = index.hash_key()
            obj.pairs[hashed.value] = Hash_Pair(key=index, value=value)
        return None

    def eval_def_statement(self, node, env):
        obj = Ad_Function_Object(parameters=node.parameters, body=node.body, env=env)
        env.set(node.name.value, obj)
        return None

    def eval_class_statement(self, node, env):
        obj = Ad_Class_Object(name=node.name, attributes=node.attributes, methods=node.methods)
        env.set(node.name.value, obj)
        return None

    def eval_member_access(self, node, env):
        evaluated = None
        if node.is_method:
            klass_instance = env.get(node.owner.value)
            klass_method = klass_instance.instance_environment.get(node.member.value)
            args_objs = self.eval_expressions(node.arguments, env)
            if len(args_objs) == 1 and self.is_error(args_objs[0]):
                return args_objs[0]
            return self.apply_method(klass_method, args_objs, klass_instance.instance_environment)
        else:
            klass_instance = env.get(node.owner.value)
            klass_environment = klass_instance.instance_environment
            evaluated = self.eval(node.member, klass_environment)
        return evaluated

    def eval_prefix_increment(self, node, env):
        obj = env.get(node.name.value)
        if obj.type == ObjectType.INTEGER:
            obj.value += 1
        env.set(node.name.value, obj)
        return obj