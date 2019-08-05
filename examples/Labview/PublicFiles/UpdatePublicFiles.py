import os 
import os.path 
import shutil 
import time,  datetime
import platform

usb2xxx_dll_path_32 = '..\\..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\msvc\\win32\\USB2XXX.dll'
libusb_dll_path_32 = '..\\..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\msvc\\win32\\libusb-1.0.dll'

def copyLibFiles():
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        if(platform.system()=="Windows"):
            for name in files:# Copy USB2XXX.dll and USB2XXX.lib
                if(".lvlib" in name):
                    dll_path = os.path.dirname(os.path.join(root, name))
                    shutil.copy(os.path.join(usb2xxx_dll_path_32),os.path.join(dll_path))
                    shutil.copy(os.path.join(libusb_dll_path_32),os.path.join(dll_path))
        else:
            print("system error")
            exit()

if __name__ == '__main__': 
    shutil.copy(os.path.join(usb2xxx_dll_path_32),os.path.join(os.getcwd()+'\\USB2XXX.dll'))
    shutil.copy(os.path.join(libusb_dll_path_32),os.path.join(os.getcwd()+'\\libusb-1.0.dll'))
    print('Copy Lib File Success!')



