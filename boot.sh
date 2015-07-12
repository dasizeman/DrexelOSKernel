#!/bin/bash

kvm -curses -kernel ../../repos/das433-cs370-summer2015/linux-2.6.22.19-cs543/arch/x86_64/boot/bzImage -append 'root=/dev/hda1 ro '"${1}"'' -drive file=local.qcow2 --redir tcp:5123::22

