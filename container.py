
import clone
import os

def container_main():
	print("container inside, [%d]" % (os.getpid()))
	os.execv('/bin/bash', ['/bin/bash', ])
	print("something's wrong")
	return 123


def main():
	print('parent [%d]' % os.getpid())
	pid = clone.clone(container_main)
	print('child process: [%d]' % pid)
	pidv, status = os.waitpid(pid, 0)
	print('pid: %d, status: %s' % (pidv, status))
	print('parent end.')

if __name__ == '__main__':
	main()

