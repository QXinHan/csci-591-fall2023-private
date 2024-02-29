import sys
import angr
import claripy

def main(argv):
    path_to_binary = argv[1]
    project = angr.Project(path_to_binary, auto_load_libs=False)

    if project == None:
        print("Error: the binary project can not be load in!")
    else:
        print("successfully load")

    start_addr = 0x140011c6e
    options = {angr.options.SYMBOL_FILL_UNCONSTRAINED_MEMORY, angr.options.SYMBOL_FILL_UNCONSTRAINED_REGISTERS}
    init_state = project.factory.blank_state(addr=start_addr, add_options=options)

    password_size = 8
    password0 = claripy.BVS('password0',8*password_size)
    # The password string is stored at [rbp - 0x158]
    save_rsp = init_state.regs.rsp
    save_rbp = init_state.regs.rbp
    #print(save_rsp,save_rbp) :<BV64 0x7ffffffffff0000> <BV64 reg_38_1_64{UNINITIALIZED}>
    init_state.regs.rsp = init_state.regs.rbp
    init_state.regs.rsp -= 0x158
    #rsp-0x158=rbp+0x38
    init_state.stack_push(password0)
    print(init_state.regs.rsp)
    init_state.regs.rsp = save_rsp
    init_state.regs.rbp = save_rbp

    def successfully_find(state):
        stdout_data = state.posix.dumps(sys.stdout.fileno())
        return 'trusted user!'.encode() in stdout_data
        #return state.addr == 0x140011c84


    def should_avoid(state):
        stdout_data = state.posix.dumps(sys.stdout.fileno())
        return 'Go away!'.encode() in stdout_data
        #return state.addr == 0x140011c8b


    simgr = project.factory.simgr(init_state, save_unconstrained=True)
    simgr.explore(find=0x140011c84, avoid=0x140011c8b)

    if simgr.found:
        found_state = simgr.found[0]
        # find 'Fangtian'
        # print(found_state.solver.eval(found_state.mem[0x14001e000].string.resolved,cast_to=bytes).decode())
        solution_password0 = found_state.solver.eval(password0, cast_to=bytes).decode()

        if solution_password0!=None:
            print(solution_password0)
            print("getting solution!")
        print('password: %s' % solution_password0)

    else:
        print('No solutions!')

if __name__ == '__main__':
    main(sys.argv)