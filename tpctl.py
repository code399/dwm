#!/usr/bin/python3.11
# -*-coding:utf-8-*-
import subprocess
import re
dev_id='16'
dev_status=None
output=subprocess.run('xinput list-props '+dev_id+' |grep "Device Enabled" ',shell=True,stdout=subprocess.PIPE).stdout.decode()
dev_status=int(re.findall(':.*(\d)',output)[0])
#print(dev_status,type(dev_status))
if dev_status==0:
    #print("开启")
    subprocess.run('xinput enable '+dev_id,shell=True)
elif dev_status==1:
    #print("关闭")
    subprocess.run('xinput disable '+dev_id,shell=True)
