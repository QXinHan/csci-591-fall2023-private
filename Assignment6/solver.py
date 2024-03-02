import angr
import networkx
import sys

constrains_cnt = 0  #


def main(argv):
    global constrains_cnt

    # load the binary file
    path_to_binary = argv[1]
    project = angr.Project(path_to_binary, auto_load_libs=False)
    if project:
        print("successfully load the binary")
    else:
        print("Fail to load the binary")

    # build a state, address is the project.entry
    start_addr = project.entry
    options = {angr.options.SYMBOL_FILL_UNCONSTRAINED_REGISTERS,
               angr.options.SYMBOL_FILL_UNCONSTRAINED_MEMORY}
    state = project.factory.blank_state(addr=start_addr, add_options=options)
    state_for_solution = project.factory.blank_state()  # for add constrains and get the solution

    # symbolize the password, set the size to 8
    password_size = 8
    solution_password = state_for_solution.solver.BVS("password", 8 * password_size)

    cfg = project.analyses.CFGFast()  # get the cfg
    malicious_state_list = []  # used to store the malicious state

    def jude_function(state):
        '''
        :functionality: to judge if there is a function's space that includes the state
        :param state: <SimState @ xxxxx>, each state is a basic block
        :return: <type str> the name of a function whose space includes the state
        '''
        function_name = ' '
        for func in cfg.functions.values():
            if state.addr >= func.addr and state.addr <= func.addr + func.size:
                function_name = func.name
                break
        return function_name

    def explore(state):
        '''
        :functionality: to explore the target the state
        :param state:  <SimState @ xxxxx>, each state is a basic block
        :return: None
        '''
        global constrains_cnt
        """I have encountered a problem with angr, 
        the 'subxxx' is considered as a function but the 'locxxx' is not
        thus if the program steps into a state that labelled by 'locxxx',
        the code `function = cfg.functions.get_by_addr(state.addr)` is wrong.
        so if I use the fallowing two lines' code, the program can not successfully find the function 'strcmp'

        function = cfg.functions.get_by_addr(state.addr)
        if function.name == 'strcmp':
        """

        if (jude_function(state) == 'strcmp'):
            print('found the strcmp!')
            malicious_state_list.append(state)
            return  # find the function 'strcmp'!

        succ = state.step()
        for successor in succ.successors:

            explore(successor)
            # back to every state to get the backdoor string.
            if len(malicious_state_list) > 0:
                # get the value stored in rdx and rcx
                value_rdx = state.solver.eval(successor.regs.rdx)
                value_rcx = state.solver.eval(successor.regs.rcx)

                if (value_rdx != 0 and value_rdx <= 0x150000000 and constrains_cnt == 0):
                    constrains_cnt += 1
                    constrained_string = state.solver.eval(state.mem[value_rdx].string.resolved, cast_to=bytes).decode()
                    constrained_string = state.solver.BVV(constrained_string, 64)
                    print('the constrain is', constrained_string)
                    state_for_solution.solver.add(solution_password == constrained_string)

                if (value_rcx != 0 and value_rcx <= 0x150000000 and constrains_cnt == 0):
                    constrains_cnt += 1
                    constrained_string = state.solver.eval(state.mem[value_rcx].string.resolved, cast_to=bytes).decode()
                    constrained_string = state.solver.BVV(constrained_string, 64)
                    print('the constrain is', constrained_string)
                    state_for_solution.solver.add(solution_password == constrained_string)

                malicious_state_list.append(state)
                return

    explore(state)
    # print(state.solver.eval(password, cast_to=bytes).decode()) I can not get the correct answer!
    # print(malicious_state_list)
    solution = state_for_solution.solver.eval(solution_password, cast_to=bytes).decode()
    if solution:
        print('Get the backdoor password!')
        print('The password is %s' % solution)
    else:
        print('Error : failed to get backdoor password!')


if __name__ == '__main__':
    main(sys.argv)



