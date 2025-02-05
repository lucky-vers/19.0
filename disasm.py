def crash(data, message):
    print(f"CRASH: {data} {message}")
    exit(1)

def describe_register(reg):
    register_names = {
        16: "1",
        32: "2",
        2:  "3",
        1:  "4",
        8:  "st",
        4:  "ip",
        64: "flag"
    }
    return register_names.get(reg, "?" if reg else "NONE")

def read_register(data, reg):
    if reg == 16:
        return data[1024]
    elif reg == 32:
        return data[1025]
    elif reg == 2:
        return data[1026]
    elif reg == 1:
        return data[1027]
    elif reg == 8:
        return data[1028]
    elif reg == 4:
        return data[1029]
    elif reg == 64:
        return data[1030]
    else:
        crash(reg, "unknown register")

def write_register(data, reg, value):
    if reg == 16:
        data[1024] = value
    elif reg == 32:
        data[1025] = value
    elif reg == 2:
        data[1026] = value
    elif reg == 1:
        data[1027] = value
    elif reg == 8:
        data[1028] = value
    elif reg == 4:
        data[1029] = value
    elif reg == 64:
        data[1030] = value
    else:
        crash(reg, "unknown register")
    return data

def write_memory(data, address, value):
    if address < len(data):
        data[address] = value
    else:
        crash(data, "memory write out of bounds")

def read_memory(data, address):
    if address < len(data):
        return data[address]
    else:
        crash(data, "memory read out of bounds")

def interpret_add(instruction_and_args):
    reg1 = instruction_and_args[1]
    reg2 = instruction_and_args[2]

    reg1_name = describe_register(reg1)
    reg2_name = describe_register(reg2)

    value1 = read_register(data, reg1)
    value2 = read_register(data, reg2)

    print(f"[s] ADD {reg1_name} {reg2_name} ({value1} + {value2})")

    result = value1 + value2
    write_register(data, reg1, result)

def interpret_stk(instruction_and_args):
    reg1 = instruction_and_args[1]
    reg2 = instruction_and_args[2]

    reg1_name = describe_register(reg1)
    reg2_name = describe_register(reg2)

    print(f"[s] STK {reg1_name} {reg2_name}")

    if reg2:
        print(f"[s] ... pushing {reg2_name}")
        data[1028] += 1  # Increment stack pointer
        value = read_register(data, reg2)
        write_memory(data, data[1028], value)  # Push value onto the stack

    if reg1:
        print(f"[s] ... popping {reg1_name}")
        value = read_memory(data, data[1028])  # Pop value from the stack
        write_register(data, reg1, value)
        data[1028] -= 1  # Decrement stack pointer

def interpret_imm(instruction_and_args):
    print(f"Set register {instruction_and_args[1]} to {instruction_and_args[2]}")
    if instruction_and_args[1] == 16:
        data[1024] = instruction_and_args[2]
    elif instruction_and_args[1] == 32:
        data[1025] = instruction_and_args[2]
    elif instruction_and_args[1] == 2:
        data[1026] = instruction_and_args[2]
    elif instruction_and_args[1] == 1:
        data[1027] = instruction_and_args[2]
    elif instruction_and_args[1] == 8:
        data[1028] = instruction_and_args[2]
    elif instruction_and_args[1] == 4:
        data[1029] = instruction_and_args[2]
    elif instruction_and_args[1] == 64:
        data[1030] = instruction_and_args[2]
    else:
        exit("unknown register!")

def interpret_instruction(instruction_and_args):
    if instruction_and_args[0] == 0x01:
        interpret_imm(instruction_and_args)
    elif instruction_and_args[0] == 0x02:
        interpret_add(instruction_and_args)
    elif instruction_and_args[0] == 0x08:
        interpret_stk(instruction_and_args)  # Call interpret_stk for stack operations
    elif instruction_and_args[0] == 0x40:
        pass  # interpret_stm(instruction_and_args)
    elif instruction_and_args[0] == 0x80:
        pass  # interpret_ldm(instruction_and_args)
    elif instruction_and_args[0] == 0x20:
        pass  # interpret_cmp(instruction_and_args)
    elif instruction_and_args[0] == 0x10:
        pass  # interpret_jmp(instruction_and_args)
    elif instruction_and_args[0] == 0x04:
        pass  # interpret_sys(instruction_and_args)

def interpreter_loop(data):
    while True:
        ip = data[1029]  # Fetch the current instruction index
        instruction_and_args = [data[ip], data[ip + 1], data[ip + 2]]
        data[1029] = ip + 3  # Increment the instruction pointer

        interpret_instruction(instruction_and_args)  # Execute the instruction

# Load the shellcode
with open('./shellcode.bin', 'rb') as f:
    data = list(bytearray(f.read()))

# Extend the data to 1030 bytes
data.extend([0x00] * (1030 - len(data)))

# Start the interpreter loop
interpreter_loop(data)
