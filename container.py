# encoding: utf-8

import os
import sys
from clone import *


def mount_proc(private=True):
    if private:
        print('宿主mount')
        os.system('umount /proc')
        os.system('mount --make-private -t proc proc /proc')
    else:
        print('容器 mount')
        os.system('mount -t proc proc /proc')


def container_main():
    # 挂载proc
    print("container inside, [%d]" % (os.getpid()))
    os.chroot('rootfs')
    os.environ['PATH'] = '/bin:/sbin:/usr/bin:/usr/sbin'
    os.chdir('/bin')
    mount_proc(private=False)
    os.execv('/bin/sh', ['/bin/sh', ])
    print("something's wrong")
    return 123


def main():
    print('parent [%d]' % os.getpid())
    # 权限检查
    if not sys.platform.startswith('linux'):
        print('不支持的平台')
        return sys.exit(1)
    if os.getuid() != 0:
        print('需要使用root用户')
        return sys.exit(2)
    # 宿主配置
    mount_proc(private=True)
    os.system('read -p "Press any key to continue"')
    pid = clone(container_main, SIGCHLD | CLONE_NEWPID | CLONE_NEWIPC | CLONE_NEWUTS| CLONE_NEWNS)
    print('child process: [%d]' % pid)
    pidv, status = os.waitpid(pid, 0)
    print('pid: %d, status: %s' % (pidv, status))
    print('parent end.')
    return sys.exit(0)


if __name__ == '__main__':
    main()

