from pwn import *
context.log_level = "debug"

p = process("./bomb")

def db():
	gdb.attach(p)
#phase_1
p.sendline("Border relations with Canada have never been better.")

#phase_2
p.sendline("1 2 4 8 16 32")

#phase_3
p.sendline("1 311")

#phase_3
p.sendline("7 0")

#phase_4
p.sendline("YONUFG")
#db()

#phase_5
p.sendline("4 3 2 1 6 5")
p.interactive()