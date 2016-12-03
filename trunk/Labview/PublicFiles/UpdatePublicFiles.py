import os 
import os.path 
import shutil 
import time,  datetime
import platform

libraryFile = ''
libusb1FilePath = '/usr/local/lib/libusb-1.0.so'
if(platform.system()=="Linux"):
    if "64bit" in platform.architecture():
        libraryFile = '/home/wdluo/USB2XXX/trunk/USB2XXX/USB2XXX/ubuntu-64bit/USB2XXX/bin/Release/libUSB2XXX.so'
    else:
        libraryFile = '/home/wdluo/USB2XXX/trunk/USB2XXX/USB2XXX/ubuntu-32bit/USB2XXX/bin/Release/libUSB2XXX.so'
elif(platform.system()=="Windows"):
    USB2XXXDllFile = 'F:\\OtherWork\\STM32\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\vs2010\\Release\\USB2XXX.dll'
    libusbDllFile = 'F:\\OtherWork\\STM32\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\libusb-1.0.20\\Win32\\Release\\dll\\libusb-1.0.dll'
else:
    print("system error")
    exit()

def copyLibFiles():
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        if(platform.system()=="Windows"):
            for name in files:# Copy USB2XXX.dll and USB2XXX.lib
                if("USB2XXX.dll" in name):
                    shutil.copy(USB2XXXDllFile,os.path.join(root, name))
                if("libusb" in name):
                    shutil.copy(libusbDllFile,os.path.join(root, name))
        else:
            print("system error")
            exit()

def copyPublicFiles():
    # shutil.copy(usbDeviceHeaderFile,'./PublicFiles');# copy the header file to ./PublicFiles
    PublicFileList = os.listdir("../PublicFiles")
    print(PublicFileList)
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        for name in files:
            if name in PublicFileList and not "PublicFiles" in root:
                shutil.copy(os.path.join("../PublicFiles", name),os.path.join(root, name))

if __name__ == '__main__': 
    copyLibFiles()
    print('Copy Lib File Success!')
    #copyPublicFiles()
    print('Copy Public File Success!')



