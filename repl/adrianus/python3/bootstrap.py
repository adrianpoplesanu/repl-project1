bootstrap_files = ('bootstrap/file.ad', )

def load_bootstrap(program, parser, evaluator, env):
    #program.reset()
    for bootstrap_file in bootstrap_files:
        """ execute all bootsrap file with the current env """
        filename = open(bootstrap_file, "r")
        source = filename.read()
        #print source
        parser.reset(source=source)
        program.reset()
        parser.build_program_statements(program)
        #env = new_environment()
        evaluator.eval(program, env)