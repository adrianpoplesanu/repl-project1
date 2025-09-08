from environment import new_environment

bootstrap_files = (
    "bootstrap/file.ad",
    "bootstrap/logger.ad",
    "bootstrap/test.ad",
    "bootstrap/mat.ad",
    "bootstrap/string_utils.ad",
    "bootstrap/sys.ad",
    "bootstrap/test2.ad",
    "bootstrap/sock.ad",
    "bootstrap/net_utils.ad"
)

def load_bootstrap(program, parser, evaluator):
    #program.reset()
    bootstrap_env = new_environment()
    for bootstrap_file in bootstrap_files:
        """ execute all bootsrap file with the current env """
        filename = open(bootstrap_file, "r")
        source = filename.read()
        #print source
        parser.reset(source=source)
        program.reset()
        parser.build_program_statements(program)
        #env = new_environment()
        evaluator.eval(program, bootstrap_env)
    return bootstrap_env
