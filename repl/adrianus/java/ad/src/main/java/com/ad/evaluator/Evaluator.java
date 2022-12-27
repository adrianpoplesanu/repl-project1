package com.ad.evaluator;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.stream.Collectors;

import com.ad.ast.*;
import com.ad.builtin.BuiltinLookup;
import com.ad.environment.Environment;
import com.ad.environment.EnvironmentUtils;
import com.ad.hash.HashPair;
import com.ad.objects.*;
import com.ad.utils.FileUtils;

import static com.ad.ast.AstNodeTypeConverter.astNodeTypeMap;
import static com.ad.environment.EnvironmentUtils.newEnvironment;

public class Evaluator {
	public static AdBooleanObject TrueObject = new AdBooleanObject(true);
	public static AdBooleanObject FalseObject = new AdBooleanObject(false);
	public static AdNullObject NULLOBJECT = new AdNullObject();

    public AdObject eval(AstNode node, Environment env) {
    	switch(node.getType()) {
		case PROGRAM:
			return evalProgram((AstProgram)node, env);
		case BLOCK_STATEMENT:
			return evalBlockStatement(node, env);
		case BOOLEAN:
			return evalBoolean(node, env);
		case CALL_EXPRESSION:
			return evalCallExpression(node, env);
		case EXPRESSION_STATEMENT:
			return evalExpressionStatement(node, env);
		case FUNCTION_LITERAL:
			return evalFunctionLiteral(node, env);
		case IDENTIFIER:
			return evalIdentifier(node, env);
		case IF_EXPRESSION:
			return evalIfExpression(node, env);
		case INFIX_EXPRESSION:
			return evalInfixExpression(node, env);
		case INTEGER_LITERAL:
			return evalInteger(node, env);
		case FLOAT_LITERAL:
			return evalFloat(node, env);
		case LET_STATEMENT:
			return evalLetStatement(node, env);
		case PREFIX_EXPRESSION:
			AstPrefixExpression prefixExpression = (AstPrefixExpression)node;
			AdObject right = eval(prefixExpression.getRight(), env);
			return evalPrefixExpression(prefixExpression.getOperator(), right);
		case RETURN_STATEMENT:
			return evalReturnStatement(node, env);
		case BREAK_STATEMENT:
			return evalBreakStatement(node, env);
		case CONTINUE_STATEMENT:
			return evalContinueStatement(node, env);
		case COMMENT:
			return evalCommentStatement(node, env);
		case STRING_LITERAL:
			return evalStringLiteral(node, env);
		case WHILE_EXPRESSION:
			return evalWhileExpression(node, env);
		case ASSIGN_STATEMENT:
			return evalAssignStatement(node, env);
		case DEF_STATEMENT:
			return evalDefStatement(node, env);
		case LIST_EXPRESSION:
			return evalListExpression(node, env);
		case INDEX_EXPRESSION:
			return evalIndexExpression(node, env);
		case HASH_EXPRESSION:
			return evalHashExpression(node, env);
		case NULL_EXPRESSION:
			return evalNullExpression(node, env);
		case CLASS_STATEMENT:
			return evalClassStatement(node, env);
		case MEMBER_ACCESS:
			return evalMemberAccess(node, env);
		case FOR_EXPRESSION:
			return evalForExpression(node, env);
		case POSTFIX_INCREMENT:
			return evalPostfixIncrement(node, env);
		default:
			System.out.println("Unknown evaluation for AST node: " + astNodeTypeMap.get(node.getType()));
			break;
    	}
    	return null;
    }

    private AdObject evalProgram(AstProgram program, Environment env) {
    	for (AstNode stmt : program.statements) {
    		AdObject result = eval(stmt, env);
    		if (result != null) System.out.println(result.inspect());
    	}
    	return null;
    }

    private AdObject evalBoolean(AstNode node, Environment env) {
    	AstBoolean booleanLiteral = (AstBoolean)node;
    	return nativeBoolToBoolean(booleanLiteral.getValue());
    }

    private AdObject nativeBoolToBoolean(boolean value) {
    	if (value) return TrueObject;
    	return FalseObject;
    }

    private AdObject evalExpressionStatement(AstNode node, Environment env) {
    	AstExpressionStatement stmt = (AstExpressionStatement)node;
    	if (stmt.getExpression() != null) {
    		return eval(stmt.getExpression(), env);
    	}
    	return null;
    }

    private AdObject evalInteger(AstNode node, Environment env) {
    	AstInteger integerLiteral = (AstInteger)node;
    	return new AdIntegerObject(integerLiteral.getValue());
    }

    private AdObject evalFloat(AstNode node, Environment env) {
    	AstFloat floatLiteral = (AstFloat) node;
    	return new AdFloatObject(floatLiteral.getValue());
	}

    private AdObject evalPrefixExpression(String operator, AdObject right) {
    	if (operator == "!") {
    		return evalBangPrefixExpression(right);
    	}
    	if (operator == "-") {
    		return evalMinusPrefixExpression(right);
    	}
    	return newError("unknown prefix operator");
    }

    private AdObject evalBangPrefixExpression(AdObject right) {
    	return nativeBoolToBoolean(!((AdBooleanObject)right).getValue());
    }

    private AdObject evalMinusPrefixExpression(AdObject right) {
    	return new AdIntegerObject(-((AdIntegerObject)right).getValue());
    }

    private AdObject evalLetStatement(AstNode node, Environment env) {
		AstLetStatement letStatement = (AstLetStatement)node;
		AdObject obj = eval(letStatement.getValue(), env);
		env.set(letStatement.getName().getValue(), obj);
    	return null;
    }

	private AdObject evalCommentStatement(AstNode node, Environment env) {
		// this is just a comment, do nothing in the evaluator
		return null;
	}

    private AdObject evalIdentifier(AstNode node, Environment env) {
    	AstIdentifier ident = (AstIdentifier)node;
    	AdObject obj = null;
    	if (env.check(ident.getValue())) {
    		obj = env.get(ident.getValue());
    	} else if (BuiltinLookup.builtinMap.containsKey(ident.getValue())) {
    		obj = BuiltinLookup.builtinMap.get(ident.getValue());
    	}
    	if (obj != null) {
			return obj;
		} else {
    		// maybe this should return NULLOBJECT for consistency with cpp and python implementation
			/*AdErrorObject error = new AdErrorObject(((AstIdentifier) node).getToken().getLiteral() + " used before definition.");
			return error;*/
			return NULLOBJECT;
		}
    }

    private AdObject evalInfixExpression(AstNode node, Environment env) {
    	AstInfixExpression expr = (AstInfixExpression) node;
    	AdObject left = eval(expr.getLeft(), env);
    	AdObject right = eval(expr.getRight(), env);
    	AdObject result = evalInfixExpression(expr.getOperator(), left, right);
    	return result;
    }

    private AdObject evalInfixExpression(String operator, AdObject left, AdObject right) {
    	if (left.getType() == ObjectTypeEnum.INT && right.getType() == ObjectTypeEnum.INT) {
    		return evalIntegerInfixExpression(operator, left, right);
    	}
    	if (left.getType() == ObjectTypeEnum.FLOAT && right.getType() == ObjectTypeEnum.FLOAT) {
    		return evalFloatInfixExpression(operator, left, right);
		}
    	if (left.getType() == ObjectTypeEnum.BOOLEAN && right.getType() == ObjectTypeEnum.BOOLEAN) {
    		return evalBooleanInfixExpression(operator, left, right);
		}
    	if (left.getType() == ObjectTypeEnum.STRING && right.getType() == ObjectTypeEnum.STRING) {
    		return evalStringInfixExpression(operator, left, right);
    	}
    	return null;
    }

    private AdObject evalIntegerInfixExpression(String operator, AdObject left, AdObject right) {
    	int left_val = ((AdIntegerObject)left).getValue();
    	int right_val = ((AdIntegerObject)right).getValue();
    	switch (operator) {
    		case "+":
    			return new AdIntegerObject(left_val + right_val);
			case "-":
				return new AdIntegerObject(left_val - right_val);
			case "*":
				return new AdIntegerObject(left_val * right_val);
			case "/":
				return new AdIntegerObject(left_val / right_val);
			case "<":
				return nativeBoolToBoolean(left_val < right_val);
			case ">":
				return nativeBoolToBoolean(left_val > right_val);
			case "<=":
				return nativeBoolToBoolean(left_val <= right_val);
			case ">=":
				return nativeBoolToBoolean(left_val >= right_val);
			case "==":
				return nativeBoolToBoolean(left_val == right_val);
			case "!=":
				return nativeBoolToBoolean(left_val != right_val);
    	}
    	return null;
    }

    private AdObject evalFloatInfixExpression(String operator, AdObject left, AdObject right) {
    	// TODO: consolidate evalIntegerInfixExpression and evalFloatInfixExpression into one method
		float left_val = ((AdFloatObject)left).getValue();
		float right_val = ((AdFloatObject)right).getValue();
		switch (operator) {
			case "+":
				return new AdFloatObject(left_val + right_val);
			case "-":
				return new AdFloatObject(left_val - right_val);
			case "*":
				return new AdFloatObject(left_val * right_val);
			case "/":
				return new AdFloatObject(left_val / right_val);
			case "<":
				return nativeBoolToBoolean(left_val < right_val);
			case ">":
				return nativeBoolToBoolean(left_val > right_val);
			case "<=":
				return nativeBoolToBoolean(left_val <= right_val);
			case ">=":
				return nativeBoolToBoolean(left_val >= right_val);
			case "==":
				return nativeBoolToBoolean(left_val == right_val);
			case "!=":
				return nativeBoolToBoolean(left_val != right_val);
		}
		return null;
	}

    private AdObject evalBooleanInfixExpression(String operator, AdObject left, AdObject right) {
    	boolean left_val = ((AdBooleanObject) left).getValue();
		boolean right_val = ((AdBooleanObject) right).getValue();
		switch (operator) {
			case "==":
				return nativeBoolToBoolean(left_val == right_val);
			case "!=":
				return nativeBoolToBoolean(left_val != right_val);
		}
		return null;
	}

    private AdObject evalStringInfixExpression(String operator, AdObject left, AdObject right) {
    	String left_val = ((AdStringObject)left).getValue();
    	String right_val = ((AdStringObject)right).getValue();
    	switch(operator) {
			case "+":
				return new AdStringObject(left_val + right_val);
			case "==":
				return new AdBooleanObject(left_val.equals(right_val));
			case "!=":
				return new AdBooleanObject(!left_val.equals(right_val));
		}
    	return null;
    }

    private AdObject evalIfExpression(AstNode node, Environment env) {
    	AstIfExpression expr = (AstIfExpression)node;
    	AstNode condition = expr.getCondition();
    	AstNode consequence = expr.getConsequence();
    	AstNode alternative = expr.getAlternative();

    	AdObject conditionObj = eval(condition, env);
    	if (isError(conditionObj)) return conditionObj;

    	if (isTruthy(conditionObj)) {
    		return eval(consequence, env);
    	} else {
    		if (alternative != null) return eval(alternative, env);
    	}
    	return null;
    }

    private boolean isTruthy(AdObject obj) {
    	if (obj == null) return false;

    	switch (obj.getType()) {
			case NULL:
				return false;
			case INT:
				AdIntegerObject int_obj = (AdIntegerObject) obj;
				if (int_obj.getValue() == 0) return false;
				return true;
			case BOOLEAN:
				AdBooleanObject bool_obj = (AdBooleanObject) obj;
				if (bool_obj.getValue()) return true;
				return false;
			case STRING:
				AdStringObject string_obj = (AdStringObject) obj;
				if (string_obj.getValue().equals("")) return false;
				return true;
			default:
			break;
    	}
    	return false;
    }

    private AdObject evalBlockStatement(AstNode node, Environment env) {
    	AstBlockStatement blockStatement = (AstBlockStatement)node;
    	AdObject result = null;
    	for (AstNode stmt : blockStatement.getStatements()) {
    		result = eval(stmt, env);
    		if (result != null && result.getType() == ObjectTypeEnum.RETURN_VALUE) {
    			//AdReturnValueObject returnObject = (AdReturnValueObject)result;
    			//return returnObject.getReturnValue();
				return result;
    		}
			if (result != null && result.getType() == ObjectTypeEnum.BREAK) {
				return result;
			}
			if (result != null && result.getType() == ObjectTypeEnum.CONTINUE) {
				return result;
			}
    	}
    	return null; // this should return null, leaving it for now for testing purposes
    }

    private AdObject evalReturnStatement(AstNode node, Environment env) {
    	AstReturnStatement returnStatement = (AstReturnStatement)node;
    	AdObject result = eval(returnStatement.getValue(), env);
    	return new AdReturnValueObject(result);
    }

	private AdObject evalBreakStatement(AstNode node, Environment env) {
		return new AdBreakObject();
	}

	private AdObject evalContinueStatement(AstNode node, Environment env) {
		return new AdContinueObject();
	}

    private AdObject evalFunctionLiteral(AstNode node, Environment env) {
    	AstFunctionLiteral functionLiteral = (AstFunctionLiteral)node;
    	AstNode body = functionLiteral.getBody();
    	ArrayList<AstNode> params = functionLiteral.getParameters();
    	AdFunctionObject func = new AdFunctionObject(params, body, env);
    	return func;
    }

    private AdObject evalCallExpression(AstNode node, Environment env) {
    	AstCallExpression callExpression = (AstCallExpression) node;
    	AdObject function = eval(callExpression.getFunction(), env);
    	if (isError(function)) return function;
    	ArrayList<AdObject> arguments = (ArrayList) evalExpressions(callExpression.getArguments(), env);
    	if (arguments.size() == 1 && isError(arguments.get(0))) {
    		return arguments.get(0);
    	}
    	return applyFunction(function, arguments, env);
    }

    private List<AdObject> evalExpressions(List<AstNode> params, Environment env) {
    	/*ArrayList<AdObject> objects = new ArrayList<AdObject>();
    	for (AstNode param : params) { // TODO: use streams
    		AdObject evaluated = eval(param, env);
    		if (isError(evaluated)) {
				ArrayList<AdObject> errorResponse = new ArrayList<>();
				return errorResponse;
			}
    		objects.add(evaluated);
    	}*/
		List<AdObject> objects = params.stream().map(param -> eval(param, env)).collect(Collectors.toList());
		if (objects.stream().filter(e -> e != null && e.getType() == ObjectTypeEnum.ERROR).findAny().isPresent()) {
			return objects.stream().filter(e -> e.getType() == ObjectTypeEnum.ERROR).collect(Collectors.toList());
		}
		return objects;
    }

    private AdObject applyFunction(AdObject function, ArrayList<AdObject> arguments, Environment env) {
    	if (function.getType() == ObjectTypeEnum.FUNCTION) {
			AdFunctionObject functionObj = (AdFunctionObject) function;
			int parametersSize =  functionObj.getParameters().size();
			int argumentsSize = arguments.size();
			if (parametersSize != argumentsSize) {
				return new AdErrorObject("function signature unrecognized, different number of params");
			}
    		Environment extendedEnv = extendFunctionEnv(function, arguments);
    		AdFunctionObject functionObject = (AdFunctionObject) function;
    		AdObject evaluated = eval(functionObject.getBlock(), extendedEnv);
    		return unwrapReturnValue(evaluated);
    	}
    	if (function.getType() == ObjectTypeEnum.BUILTIN) {
    		AdBuiltinObject builtin = (AdBuiltinObject) function;
			// la builtins trebuie sa gasesc un mod de a face verificarea de semnatura, nu pot decat daca am un numar acceptat de parametri care pot fi primiti de apel.
			if (builtin.getBuiltinFunction().getAcceptedParameterSize().size() != 0 &&
					!builtin.getBuiltinFunction().getAcceptedParameterSize().contains(arguments.size())) {
				return new AdErrorObject("builtin signature unrecognized, different number of params");
			}
    		return builtin.getBuiltinFunction().call(arguments, env);
    	}
		if (function.getType() == ObjectTypeEnum.CLASS) {
			Environment instanceEnv = newEnvironment();
			AdClassObject adClassObject = (AdClassObject) function;
			String name = ((AstIdentifier) adClassObject.getName()).getValue();
			AdClassInstance adClassInstance = new AdClassInstance(name, adClassObject, instanceEnv);
			updateInstanceWithInheritedClasses(adClassInstance, env);
			adClassObject.getAttributes().forEach(attribute -> {
				if (attribute.getType() == AstNodeTypeEnum.ASSIGN_STATEMENT) {
					adClassInstance.getEnvironment().setOuter(env);
					AdObject evaluated = eval(((AstAssignStatement) attribute).getValue(), adClassInstance.getEnvironment());
					String attributeName = ((AstIdentifier)((AstAssignStatement) attribute).getName()).getValue();
					adClassInstance.getEnvironment().setLocalParam(attributeName, evaluated);
				}
				if (attribute.getType() == AstNodeTypeEnum.EXPRESSION_STATEMENT) {
					AstExpressionStatement astExpressionStatement = (AstExpressionStatement) attribute;
					if (astExpressionStatement.getExpression().getType() == AstNodeTypeEnum.ASSIGN_STATEMENT) {
						adClassInstance.getEnvironment().setOuter(env);
						AstAssignStatement astAssignStatement = (AstAssignStatement) astExpressionStatement.getExpression();
						AdObject evaluated = eval(astAssignStatement.getValue(), adClassInstance.getEnvironment());
						String attributeName = ((AstIdentifier)astAssignStatement.getName()).getValue();
						adClassInstance.getEnvironment().setLocalParam(attributeName, evaluated);
					}
				}
			});
			adClassObject.getMethods().forEach(method -> {
				AstDefStatement astDefStatement = (AstDefStatement) method;
				AdFunctionObject adFunctionObject = new AdFunctionObject(astDefStatement.getParameters(), astDefStatement.getBody(), adClassInstance.getEnvironment());
				AstIdentifier astIdentifier = (AstIdentifier) astDefStatement.getName();
				adClassInstance.getEnvironment().set(astIdentifier.getValue(), adFunctionObject);
			});
			//updateInstanceWithInheritedClasses(adClassInstance, env);
			// call class instance constructor here
			callInstanceConstructor(adClassInstance, arguments, env);
			return adClassInstance;
		}
    	return null;
    }

    private void callInstanceConstructor(AdClassInstance adClassInstance, List<AdObject> arguments, Environment env) {
		Environment instanceEnv = adClassInstance.getEnvironment();
		AdObject method = instanceEnv.lookupConstructor();
		if (method != null) {
			if (arguments.size() == 1 && arguments.get(0).getType() == ObjectTypeEnum.ERROR) {
				// do nothing
			}
			instanceEnv.setOuter(env);
			applyMethod(method, arguments, instanceEnv);
		}
	}

	private void updateInstanceWithInheritedClasses(AdClassInstance adClassInstance, Environment env) {
		List<AstNode> superKlasses = adClassInstance.getAdClassObject().getInheritFrom();
		for (AstNode superKlass : superKlasses) {
			String identifier = superKlass.tokenLiteral();
			adClassInstance.getInheritFrom().add(identifier);
			Environment klassEnv = EnvironmentUtils.newEnvironment(); // newEnvironment() ??? newOuterEnvironment() ???
			HashMap<String, Environment> baseEnvironments = adClassInstance.getInheritedEnvs();
			baseEnvironments.put(identifier, klassEnv);
			klassEnv.setOuter(env);
			baseEnvironments.get(identifier).setOuter(env);
			AdClassObject adClassObject = (AdClassObject) env.get(identifier);
			adClassObject.getAttributes().forEach(attribute -> {
				if (attribute.getType() == AstNodeTypeEnum.ASSIGN_STATEMENT) {
					// this adds everything to main class
					adClassInstance.getEnvironment().setOuter(env);
					AdObject evaluated = eval(((AstAssignStatement) attribute).getValue(), adClassInstance.getEnvironment());
					String attributeName = ((AstIdentifier)((AstAssignStatement) attribute).getName()).getValue();
					adClassInstance.getEnvironment().setLocalParam(attributeName, evaluated);

					// TODO: store the new env as entry in inherited map
					// this adds everything to map of parent classes envs
					klassEnv.setLocalParam(attributeName, evaluated);
					adClassInstance.getEnvironment().addSibling(superKlass.tokenLiteral(), klassEnv);
				}
				if (attribute.getType() == AstNodeTypeEnum.EXPRESSION_STATEMENT) {
					AstExpressionStatement astExpressionStatement = (AstExpressionStatement) attribute;
					if (astExpressionStatement.getExpression().getType() == AstNodeTypeEnum.ASSIGN_STATEMENT) {
						// this adds everything to main class
						adClassInstance.getEnvironment().setOuter(env);
						AstAssignStatement astAssignStatement = (AstAssignStatement) astExpressionStatement.getExpression();
						AdObject evaluated = eval(astAssignStatement.getValue(), adClassInstance.getEnvironment());
						String attributeName = ((AstIdentifier)astAssignStatement.getName()).getValue();
						adClassInstance.getEnvironment().setLocalParam(attributeName, evaluated);

						// TODO: store the new env as entry in inherited map
						// this adds everything to map of parent classes envs
						klassEnv.setLocalParam(attributeName, evaluated);
						adClassInstance.getEnvironment().addSibling(superKlass.tokenLiteral(), klassEnv);
					}
				}
			});
			adClassObject.getMethods().forEach(method -> {
				// this adds everything to main class
				AstDefStatement astDefStatement = (AstDefStatement) method;
				AdFunctionObject adFunctionObject = new AdFunctionObject(astDefStatement.getParameters(), astDefStatement.getBody(), adClassInstance.getEnvironment());
				AstIdentifier astIdentifier = (AstIdentifier) astDefStatement.getName();
				adClassInstance.getEnvironment().set(astIdentifier.getValue(), adFunctionObject);

				// TODO: store the new env as entry in inherited map
				// this adds everything to map of parent classes envs
				klassEnv.set(astIdentifier.getValue(), adFunctionObject);
				adClassInstance.getEnvironment().addSibling(superKlass.tokenLiteral(), klassEnv);
			});
			adClassInstance.getInheritedEnvs().put(identifier, klassEnv);
		}
	}

	private AdObject applyMethod(AdObject function, List<AdObject> arguments, Environment env) {
    	if (function.getType() == ObjectTypeEnum.FUNCTION) {
			Environment extendedEnv = extendMethodEnv(function, arguments, env);
			AdFunctionObject functionObject = (AdFunctionObject) function;
			AdObject evaluated = eval(functionObject.getBlock(), extendedEnv);
			return unwrapReturnValue(evaluated);
		}
    	return null;
	}

    private Environment extendFunctionEnv(AdObject func, List<AdObject> arguments) {
    	AdFunctionObject functionObject = (AdFunctionObject) func;
     	Environment extended = EnvironmentUtils.newEnclosedEnvironment(functionObject.getEnv());
     	int i = 0;
     	for (AstNode param : functionObject.getParameters()) {
     		//extended.set(param.tokenLiteral(), arguments.get(i++));
			extended.setLocalParam(param.tokenLiteral(), arguments.get(i++));
     	}
    	return extended;
    }

    private Environment extendMethodEnv(AdObject function, List<AdObject> arguments, Environment env) {
    	AdFunctionObject funcObj = (AdFunctionObject) function;
		Environment extended = EnvironmentUtils.newEnclosedEnvironment(env);
    	int i = 0;
    	for (AstNode param : funcObj.getParameters()) {
    		//env.set(param.tokenLiteral(), arguments.get(i++));
			extended.setLocalParam(param.tokenLiteral(), arguments.get(i++));
		}
		return extended;
	}

    private AdObject unwrapReturnValue(AdObject evaluated) {
    	if (evaluated == null) return null;
    	if (evaluated.getType() == ObjectTypeEnum.RETURN_VALUE) {
    		AdReturnValueObject returnValue = (AdReturnValueObject) evaluated;
    		return returnValue.getReturnValue();
    	}
    	return evaluated;
    }

    private AdObject evalStringLiteral(AstNode node, Environment env) {
    	AstStringLiteral stringLiteral = (AstStringLiteral)node;
    	return new AdStringObject(stringLiteral.getValue());
    }

    private AdObject evalWhileExpression(AstNode node, Environment env) {
		AstWhileExpression expr = (AstWhileExpression) node;
    	AdObject condition = eval(expr.getCondition(), env);
    	if (isError(condition)) {
    		// TODO: treat the error, possibly return an AdError object
		}
    	while (isTruthy(condition)) {
    		AdObject result = eval(expr.getBody(), env);
    		if (result != null && result.getType() == ObjectTypeEnum.RETURN_VALUE) return result;
			if (result != null && result.getType() == ObjectTypeEnum.BREAK) return null;
			//if (result != null && result.getType() == ObjectTypeEnum.CONTINUE) ; // do nothing ???
    		condition = eval(expr.getCondition(), env);
		}
    	return null;
	}

	private AdObject evalForExpression(AstNode node, Environment env) {
    	AstForExpression expr = (AstForExpression) node;
    	eval(expr.getInitialization(), env);
    	AdObject condition = eval(expr.getCondition(), env);
    	while(isTruthy(condition)) {
    		AdObject result = eval(expr.getBody(), env);
    		if (result != null && result.getType() == ObjectTypeEnum.RETURN_VALUE) return result;
			if (result != null && result.getType() == ObjectTypeEnum.BREAK) return null;
			//if (result != null && result.getType() == ObjectTypeEnum.CONTINUE) ; // do nothing ???
    		eval(expr.getStep(), env);
    		condition = eval(expr.getCondition(), env);
		}
    	return null;
	}

	private AdObject evalPostfixIncrement(AstNode node, Environment env) {
		AstPostfixIncrement expr = (AstPostfixIncrement) node;
		if (expr.getName().getType() == AstNodeTypeEnum.INDEX_EXPRESSION) {
			AstIndexExpression indexExpression = (AstIndexExpression) expr.getName();
			AdObject old_obj = evalIndexExpression(indexExpression, env);
			int value = ((AdIntegerObject) old_obj).getValue();
			if ("++".equals(expr.getOperation())) {
				AdIntegerObject new_obj = new AdIntegerObject(value + 1);
				AstNode left = indexExpression.getLeft();
				AstNode index = indexExpression.getIndex();
				AdObject leftObj = eval(left, env);
				AdObject indexObj = eval(index, env);
				if (leftObj.getType() == ObjectTypeEnum.LIST) {
					if (indexObj.getType() == ObjectTypeEnum.INT) {
						int i = ((AdIntegerObject) indexObj).getValue();
						AdListObject target = (AdListObject) leftObj;
						target.getElements().set(i, new_obj);
						return new AdIntegerObject(value);
					}
				}
				if (leftObj.getType() == ObjectTypeEnum.HASH) {
					if (indexObj.getType() == ObjectTypeEnum.INT) {
						System.out.println("doing a postfix ++ for a hash element");
						AdHashObject target = (AdHashObject) leftObj;
						//target.getElements().put("aaa", new HashPair(ceva, new_obj));
						// return new AdIntegerObject(value);
					}
				}
			}
			return null;
		}
		AstIdentifier ident = (AstIdentifier) expr.getName();
		AdObject old_obj = env.get(ident.getValue());
		if (old_obj.getType() == ObjectTypeEnum.INT) {
			int value = ((AdIntegerObject) old_obj).getValue();
			if (expr.getOperation().equals("++")) {
				AdIntegerObject new_obj = new AdIntegerObject(value + 1);
				env.set(ident.getValue(), new_obj);
				return new AdIntegerObject(value);
			}
			if (expr.getOperation().equals("--")) {
				AdIntegerObject new_obj = new AdIntegerObject(value - 1);
				env.set(ident.getValue(), new_obj);
				return new AdIntegerObject(value);
			}
		}
		return null;
	}

	private AdObject evalAssignStatement(AstNode node, Environment env) {
    	AstAssignStatement assignStatement = (AstAssignStatement) node;
    	if (assignStatement.getName().getType() == AstNodeTypeEnum.INDEX_EXPRESSION) {
			return evalIndexExpressionAssign(node, env);
		} else if (assignStatement.getName().getType() == AstNodeTypeEnum.MEMBER_ACCESS) {
			AstMemberAccess memberAccess = (AstMemberAccess) assignStatement.getName();
			if (memberAccess.getOwner().getType() == AstNodeTypeEnum.THIS_EXPRESSION) {
				AstIdentifier klassMember = (AstIdentifier) memberAccess.getMember();
				AdObject obj = eval(assignStatement.getValue(), env);
				env.getOuter().set(klassMember.getValue(), obj);
			} else if (memberAccess.getOwner().getType() == AstNodeTypeEnum.SUPER_EXPRESSION) { // maybe super() should be used only for methods?
				System.out.println("evaluating an assign statement involving a super expression");
			} else {
				AstIdentifier owner = (AstIdentifier) memberAccess.getOwner();
				AdClassInstance klassInstance = (AdClassInstance) env.get(owner.getValue());
				AstIdentifier klassMember = (AstIdentifier) memberAccess.getMember();
				Environment klassEnvironment = klassInstance.getEnvironment();
				AdObject obj = eval(assignStatement.getValue(), env);
				klassEnvironment.set(klassMember.getValue(), obj);
			}
		} else {
			AdObject obj = eval(assignStatement.getValue(), env);
			AstIdentifier ident = (AstIdentifier) assignStatement.getName();
			env.set(ident.getValue(), obj);
		}
    	return null;
	}

	private AdObject evalIndexExpressionAssign(AstNode node, Environment env) {
    	AstAssignStatement stmt = (AstAssignStatement) node;
    	AstIndexExpression indexExpr = (AstIndexExpression) stmt.getName();
    	AstNode left = indexExpr.getLeft();
    	AstNode index = indexExpr.getIndex();
    	AdObject leftObj = eval(left, env);
    	AdObject indexObj = eval(index, env);
    	if (leftObj.getType() == ObjectTypeEnum.LIST) {
    		if (indexObj.getType() == ObjectTypeEnum.INT) {
    			int i = ((AdIntegerObject) indexObj).getValue();
    			AdListObject target = (AdListObject) leftObj;
    			AdObject newValue = eval(stmt.getValue(), env);
    			target.getElements().set(i, newValue);
			}
		}
    	if (leftObj.getType() == ObjectTypeEnum.HASH) {
			AdObject newValue = eval(stmt.getValue(), env);
			((AdHashObject) leftObj).getElements().put(indexObj.hash(), new HashPair(indexObj, newValue));
		}
        return null;
	}

	private AdObject evalDefStatement(AstNode node, Environment env) {
    	AstDefStatement stmt = (AstDefStatement) node;
    	ArrayList<AstNode> parameters = stmt.getParameters();
    	AstNode body = stmt.getBody();
    	AstIdentifier identifier = (AstIdentifier) stmt.getName();
    	AdFunctionObject function = new AdFunctionObject(parameters, body, env);
    	env.set(identifier.getValue(), function);
    	return null;
	}

	private AdObject evalListExpression(AstNode node, Environment env) {
    	AstListExpression expr = (AstListExpression) node;
    	List<AstNode> listElements = expr.getElements();
    	AdListObject listObject = new AdListObject();
    	listObject.setElements(evalExpressions(listElements, env));
    	return listObject;
	}

	private AdObject evalIndexExpression(AstNode node, Environment env) {
		AstIndexExpression expr = (AstIndexExpression) node;
    	AdObject left = eval(expr.getLeft(), env);
    	AdObject index = eval(expr.getIndex(), env);

    	if (left.getType() == ObjectTypeEnum.LIST && index.getType() == ObjectTypeEnum.INT) {
    		return evalListIndexExpression(left, index);
		}
    	if (left.getType() == ObjectTypeEnum.HASH) {
    		return evalHashIndexExpression(left, index);
		}
    	return null;
	}

	private AdObject evalListIndexExpression(AdObject left, AdObject index) {
    	int max = ((AdListObject) left).getElements().size();
    	int idx = ((AdIntegerObject) index).getValue();
    	if (idx < 0 || idx >= max) return null; // this should be an error object
    	return ((AdListObject) left).getElements().get(idx);
	}

	private AdObject evalHashIndexExpression(AdObject left, AdObject index) {
		AdObject result = ((AdHashObject)left).getElements().get(index.hash()).getValue();
		return result;
	}

	private AdObject evalHashExpression(AstNode node, Environment env) {
    	AstHashExpression expr = (AstHashExpression) node;
    	HashMap<String, HashPair<AdObject>> pairs = new HashMap<>();

    	expr.getPairs().entrySet().forEach(e -> {
    		AdObject key = eval(e.getKey(), env);
    		AdObject value = eval(e.getValue(), env);
    		pairs.put(key.hash(), new HashPair<>(key, value));
		});

    	AdHashObject obj = new AdHashObject(pairs);
    	return obj;
	}

	private AdObject evalNullExpression(AstNode node, Environment env) {
    	return NULLOBJECT;
	}

	private AdObject evalClassStatement(AstNode node, Environment env) {
		AstClassStatement stmt = (AstClassStatement) node;
		AdClassObject obj = new AdClassObject();
		obj.setKlassNode(stmt);
		obj.setName(stmt.getName());
		obj.setAttributes(stmt.getAttributes());
		obj.setMethods(stmt.getMethods());
		obj.setInheritFrom(stmt.getInheritFrom());
		env.set(((AstIdentifier)stmt.getName()).getValue(), obj);
		return null;
	}

	private AdObject evalMemberAccess(AstNode node, Environment env) {
		AstMemberAccess stmt = (AstMemberAccess) node;

		AdObject evaluated = evalFileObjectMethod(node, stmt.getArguments(), env);
		if (evaluated != null) {
			return evaluated;
		}

		evaluated = evalSocketObjectMethod(node, stmt.getArguments(), env);
		if (evaluated != null) {
			return evaluated;
		}

		if (stmt.getOwner().getType() == AstNodeTypeEnum.THIS_EXPRESSION) {
			if (stmt.isMethod()) {
				AstIdentifier member = (AstIdentifier) stmt.getMember();
				AdFunctionObject klassMethod = (AdFunctionObject) env.get(member.getValue());
				List<AdObject> argObjs = evalExpressions(stmt.getArguments(), env);
				if (argObjs.size() == 1 && argObjs.get(0).getType() == ObjectTypeEnum.ERROR) {
					return argObjs.get(0);
				}
				return applyMethod(klassMethod, argObjs, env); // nu sunt sigur de env aici, cred ca null e ok
			} else {
				AdObject result = eval(stmt.getMember(), env); // nu sunt sigur de env aici, cred ca null e ok
				return result;
			}
		} else if (stmt.getOwner().getType() == AstNodeTypeEnum.SUPER_EXPRESSION) {
			if (stmt.isMethod()) {
				List<AdObject> argObjs = evalExpressions(stmt.getArguments(), env);
				if (argObjs.size() == 1 && argObjs.get(0).getType() == ObjectTypeEnum.ERROR) {
					return argObjs.get(0);
				}

				//i need the current class context so i can parentKlassEnv = siblings[stmt.getOwner().tokenLiteral]
				AstSuperExpression superExpression = (AstSuperExpression) stmt.getOwner();
				Environment parentKlassEnv = env.getOuter().getSibling(superExpression.getTarget().tokenLiteral());
				AstIdentifier member = (AstIdentifier) stmt.getMember();
				AdFunctionObject parentMethod = (AdFunctionObject) parentKlassEnv.get(member.getValue());
				AdObject result = applyMethod(parentMethod, argObjs, env.getOuter());
				return result;
			} else {
				AstSuperExpression superExpression = (AstSuperExpression) stmt.getOwner();
				Environment parentKlassEnv = env.getOuter().getSibling(superExpression.getTarget().tokenLiteral());
				AstIdentifier member = (AstIdentifier) stmt.getMember();
				AdObject result = eval(member, parentKlassEnv);
				return result;
			}
		} else {
			if (stmt.isMethod()) {
				if (stmt.getOwner().getType() == AstNodeTypeEnum.MEMBER_ACCESS) {
					return recursiveMemberAccessCall(stmt, env);
				}
				if (stmt.getOwner().getType() == AstNodeTypeEnum.CALL_EXPRESSION) {
					return recursiveMemberAccessCall(stmt, env);
				}
				AstIdentifier owner = (AstIdentifier) stmt.getOwner();
				AstIdentifier member = (AstIdentifier) stmt.getMember();
				AdClassInstance klassInstance = (AdClassInstance) env.get(owner.getValue());
				Environment old = klassInstance.getEnvironment().getOuter();
				klassInstance.getEnvironment().setOuter(env);
				AdObject klassMethod = klassInstance.getEnvironment().get(member.getValue());
				if (klassMethod == null) {
					return NULLOBJECT;
				}
				List<AdObject> argObjs = evalExpressions(stmt.getArguments(), env);
				if (argObjs.size() == 1 && argObjs.get(0).getType() == ObjectTypeEnum.ERROR) {
					return argObjs.get(0);
				}
				AdObject result = applyMethod(klassMethod, argObjs, klassInstance.getEnvironment());
				klassInstance.getEnvironment().setOuter(old);
				return result;
			} else {
				if (stmt.getOwner().getType() == AstNodeTypeEnum.IDENTIFIER) {
					AstIdentifier owner = (AstIdentifier) stmt.getOwner(); // stmt: "AstMemberAccess"
					AstIdentifier member = (AstIdentifier) stmt.getMember();
					AdClassInstance klassInstance = (AdClassInstance) env.get(owner.getValue());
					//klassInstance.getEnvironment().setOuter(env);
					Environment old = klassInstance.getEnvironment().getOuter();
					klassInstance.getEnvironment().setOuter(null);
					AdObject result = eval(member, klassInstance.getEnvironment());
					klassInstance.getEnvironment().setOuter(old);
					return result;
				}
				if (stmt.getOwner().getType() == AstNodeTypeEnum.MEMBER_ACCESS) {
					return recursiveMemberAccessCall(stmt, env);
				}
				if (stmt.getOwner().getType() == AstNodeTypeEnum.CALL_EXPRESSION) {
					return recursiveMemberAccessCall(stmt, env);
				}
			}
		}
		return null;
	}

	private AdObject recursiveMemberAccessCall(AstNode node, Environment env) {
		// TODO: recursive member access assign statement should work this way also, not only retrievals
		List<AstMemberAccess> chainedMemberAccesses = new ArrayList<>();
		while (node.getType() == AstNodeTypeEnum.MEMBER_ACCESS) {
			chainedMemberAccesses.add((AstMemberAccess) node);
			node = ((AstMemberAccess) node).getOwner();
		}
		Environment currentEnv = env;

		// initialize env
		AstNode initialMemberAccess = chainedMemberAccesses.get(0);
		while (initialMemberAccess.getType() == AstNodeTypeEnum.MEMBER_ACCESS) {
			initialMemberAccess = ((AstMemberAccess)initialMemberAccess).getOwner();
		}

		if (initialMemberAccess.getType() == AstNodeTypeEnum.CALL_EXPRESSION) {
			AdObject obj = eval(initialMemberAccess, currentEnv);
			if (obj.getType() == ObjectTypeEnum.INSTANCE) {
				currentEnv = ((AdClassInstance) obj).getEnvironment();
			}
		}

		if (initialMemberAccess.getType() == AstNodeTypeEnum.IDENTIFIER) {
			AdObject obj = eval(initialMemberAccess, currentEnv);
			if (obj.getType() == ObjectTypeEnum.INSTANCE) {
				currentEnv = ((AdClassInstance) obj).getEnvironment();
			}
		}
		// end initialize env

		for (int i = chainedMemberAccesses.size() - 1; i >= 0; i--) {
            AstMemberAccess currentMemberAccess = chainedMemberAccesses.get(i);
			if (currentMemberAccess.isMethod()) {
				// am de a face cu un call
				AdObject obj = eval(currentMemberAccess.getMember(), currentEnv);
				if (obj.getType() == ObjectTypeEnum.FUNCTION) {
					List<AdObject> argObjs = evalExpressions(chainedMemberAccesses.get(i).getArguments(), env);
					AdObject obj2 = applyMethod(obj, argObjs, ((AdFunctionObject) obj).getEnv());
					if (i == 0) {
						// i have reached the end, i need to return
						return obj2;
					}
					if (obj2.getType() == ObjectTypeEnum.INSTANCE) {
						currentEnv = ((AdClassInstance) obj2).getEnvironment();
					}
				}
			} else {
				// am de a face cu un identificator
				AdObject obj = eval(currentMemberAccess.getMember(), currentEnv);
				if (i == 0) {
					// i have reached the end, i need to return
					return obj;
				}
				if (obj.getType() == ObjectTypeEnum.INSTANCE) {
					currentEnv = ((AdClassInstance) obj).getEnvironment();
				}
			}
		}
		// daca am ajuns aici atunci nu cred ca e ok
		return null;
	}

	private AdObject recursiveMemberAccessCall2(AstNode node, Environment env) {
		// TODO: review and cleanup this
		// this is the old way of doing recursive member access calls, the new version is much more elegant
		List<AstMemberAccess> chainedMemberAccesses = new ArrayList<>();
		while (node.getType() == AstNodeTypeEnum.MEMBER_ACCESS) {
			chainedMemberAccesses.add((AstMemberAccess) node);
			node = ((AstMemberAccess) node).getOwner();
		}

		Environment currentEnv = env;
		for (int i = chainedMemberAccesses.size() - 1; i >= 0; i--) {
			AstMemberAccess recursiveMemberAccess = new AstMemberAccess();
			if (i == chainedMemberAccesses.size() - 1) {
				AstMemberAccess current = chainedMemberAccesses.get(i);

				if (current.getOwner().getType() == AstNodeTypeEnum.CALL_EXPRESSION) {
					AdObject obj2 = eval(current.getOwner(), currentEnv);
					Environment newEnv = ((AdClassInstance) obj2).getEnvironment();
					currentEnv = newEnv;

					if (i == 0) {
						if (current.isMethod() && obj2.getType() == ObjectTypeEnum.INSTANCE) {
							AdObject obj3 = eval(current.getMember(), ((AdClassInstance) obj2).getEnvironment());
							if (obj3.getType() == ObjectTypeEnum.FUNCTION) {
								List<AdObject> argObjs = evalExpressions(chainedMemberAccesses.get(i).getArguments(), env);
								AdObject obj4 = applyMethod(obj3, argObjs, ((AdFunctionObject) obj3).getEnv());
								return obj4;
							}
							return obj3;
						}
						if (!current.isMethod() && obj2.getType() == ObjectTypeEnum.INSTANCE) {
							if (current.getMember().getType() == AstNodeTypeEnum.IDENTIFIER) {
								AdObject obj3 = evalIdentifier(current.getMember(), currentEnv);
								return obj3;
							}
						}
					} else {
						if (current.isMethod() && obj2.getType() == ObjectTypeEnum.INSTANCE) {
							AdObject obj3 = eval(current.getMember(), ((AdClassInstance) obj2).getEnvironment());
							if (obj3.getType() == ObjectTypeEnum.FUNCTION) {
								List<AdObject> argObjs = evalExpressions(chainedMemberAccesses.get(i).getArguments(), env);
								AdObject obj4 = applyMethod(obj3, argObjs, ((AdFunctionObject) obj3).getEnv());
								//currentEnv = ((AdClassInstance) obj4).getEnvironment();
								System.out.println(obj4);
							}
						}
					}
					continue;
				}

				AstIdentifier member = (AstIdentifier) current.getMember();
				AstIdentifier owner = (AstIdentifier) current.getOwner();

				recursiveMemberAccess.setMember(new AstIdentifier(null, member.getValue()));
				recursiveMemberAccess.setOwner(new AstIdentifier(null, owner.getValue()));
			} else {
				AstMemberAccess current = chainedMemberAccesses.get(i);
				if (current.getType() == AstNodeTypeEnum.MEMBER_ACCESS &&
						current.isMethod() &&
						current.getOwner().getType() == AstNodeTypeEnum.MEMBER_ACCESS &&
						((AstMemberAccess)current.getOwner()).getOwner().getType() == AstNodeTypeEnum.CALL_EXPRESSION) {
					AstCallExpression astCallExpression = (AstCallExpression) ((AstMemberAccess)current.getOwner()).getOwner();
					AdObject obj2 = eval(astCallExpression, env);
					AdObject obj3 = eval(((AstMemberAccess)current.getOwner()).getOwner(), ((AdClassInstance) obj2).getEnvironment());
					if (obj3.getType() == ObjectTypeEnum.FUNCTION) {
						List<AdObject> argObjs = evalExpressions(chainedMemberAccesses.get(i).getArguments(), env);
						AdObject obj4 = applyMethod(obj3, argObjs, ((AdFunctionObject) obj3).getEnv());
						Environment newEnv = ((AdClassInstance) obj3).getEnvironment();
						currentEnv = newEnv;

					}
					continue;
				}
				AstIdentifier member = (AstIdentifier) current.getMember();
				AstMemberAccess owner = (AstMemberAccess) current.getOwner();
				AstIdentifier newOwner = (AstIdentifier) owner.getMember();
				String memberValue = member.getValue();
				String ownerValue = newOwner.getValue();
				recursiveMemberAccess.setMember(new AstIdentifier(null, memberValue));
				recursiveMemberAccess.setOwner(new AstIdentifier(null, ownerValue));
			}
			AdObject obj2 = eval(recursiveMemberAccess, currentEnv);
			AdObject obj3 = null;
			if (obj2.getType() == ObjectTypeEnum.FUNCTION) {
				// i need to eval it and either return on i == 0 or, set the newEnv to the returning obj env
				List<AdObject> argObjs = evalExpressions(chainedMemberAccesses.get(i).getArguments(), env);
				if (i == 0) {
					obj3 = applyMethod(obj2, argObjs, ((AdFunctionObject) obj2).getEnv());
					return obj3;
				} else {
					obj3 = applyMethod(obj2, argObjs, currentEnv);
				}
			}
			if (i == 0) {
				return obj2;
			}
			if (obj2.getType() == ObjectTypeEnum.FUNCTION) {
				Environment newEnv = ((AdClassInstance) obj3).getEnvironment();
				currentEnv = newEnv;
				i--;
				if (i == 0) {
					return currentEnv.get(((AstIdentifier)chainedMemberAccesses.get(i).getMember()).getValue());
				}
			} else {
				Environment newEnv = ((AdClassInstance) obj2).getEnvironment();
				currentEnv = newEnv;
			}
		}
		return null;
	}

	private AdObject evalFileObjectMethod(AstNode node, List<AstNode> args, Environment env) {
    	AstMemberAccess memberAccess = (AstMemberAccess) node;
    	if (memberAccess.getOwner().getType() != AstNodeTypeEnum.IDENTIFIER) {
    		return null;
		}
    	AstIdentifier ownerIdentifier = (AstIdentifier) memberAccess.getOwner();
    	AstIdentifier memberIdentifier = (AstIdentifier) memberAccess.getMember();
    	AdObject rawObject = env.get(ownerIdentifier.getValue());
    	if (rawObject.getType() == ObjectTypeEnum.FILE) {
			AdFileObject fileObject = (AdFileObject) rawObject;
			String memberName = memberIdentifier.getValue();
			if (memberName.equals("read")) {
				if (fileObject.getOperator().contains("r")) {
					String text = FileUtils.readFile(fileObject.getFilename());
					AdStringObject result = new AdStringObject(text);
					return result;
				}
			}
			if (memberName.equals("write")) {
				if (fileObject.getOperator().contains("w")) {
					List<AdObject> argObjs = evalExpressions(args, env);
					// TODO: fix inspect() or use toString() like method that generates the string without ""
					FileUtils.writeToFile((fileObject.getFilename()), argObjs.get(0).inspect());
					return NULLOBJECT;
				} else
				if (fileObject.getOperator().contains("a")) {
					List<AdObject> argObjs = evalExpressions(args, env);
					// TODO: fix inspect() or use toString() like method that generates the string without ""
					FileUtils.appendToFile((fileObject.getFilename()), argObjs.get(0).inspect());
					return NULLOBJECT;
				}
			}
		}
    	return null;
	}

	private AdObject evalSocketObjectMethod(AstNode node, List<AstNode> args, Environment env) {
		//System.out.println("evaluating a socket instance member access");
		AstMemberAccess memberAccess = (AstMemberAccess) node;
		if (memberAccess.getOwner().getType() != AstNodeTypeEnum.IDENTIFIER) {
			return null;
		}
		AstIdentifier ownerIdentifier = (AstIdentifier) memberAccess.getOwner();
		AstIdentifier memberIdentifier = (AstIdentifier) memberAccess.getMember();
		AdObject rawObject = env.get(ownerIdentifier.getValue());
		if (rawObject.getType() == ObjectTypeEnum.SOCKET) {
			return NULLOBJECT;
		}
		return null;
	}

    private AdObject newError(String msg) {
        return new AdErrorObject(msg);
    }

    private boolean isError(AdObject obj) {
		return (obj != null && obj.getType() == ObjectTypeEnum.ERROR);
    }
}
