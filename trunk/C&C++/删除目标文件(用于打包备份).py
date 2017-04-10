import os 
import os.path 
import shutil 
import time,  datetime

# copy USB2XXX.dll to project directory
def copyFile(srcPath,rootDir):  
	if os.path.exists(srcPath):
		obj = listDir(rootDir)
		if None != obj:
			shutil.copy(srcPath,obj)  
# list all directory
def listDir(rootDir):
    filelist=os.listdir(rootDir) 
    for f in filelist:
        file = os.path.join( rootDir, f ) 
        if os.path.isdir(file):
            dllDirList = os.listdir(file)
            print(dllDirList)
            if 'usb_device.h' in dllDirList:
                return file

# remove all file exception excList
def removeFiles(rootDir,excList):
	filelist=os.listdir(rootDir) 
	for f in filelist:
		file = os.path.join( rootDir, f ) 
		if os.path.isfile(file) and not f in excList: 
			os.remove(file)  
			print (file+" removed!") 
		elif os.path.isdir(file):
			removeFiles(file,excList)
# remove directory
def removeDirs(rootDir):
	filelist=os.listdir(rootDir)  
	for f in filelist:  
		file = os.path.join( rootDir, f )
		if os.path.isdir(file):  
			shutil.rmtree(file,True)  
			print ("dir "+file+" removed!") 
	os.rmdir(rootDir)

usb2xxx_dll_path_32 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\msvc\\win32\\USB2XXX.dll'
usb2xxx_lib_path_32 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\msvc\\win32\\\\USB2XXX.lib'
libusb_dll_path_32 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\msvc\\win32\\libusb-1.0.dll'
usb2xxx_dll_path_64 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\msvc\\x64\\USB2XXX.dll'
usb2xxx_lib_path_64 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\msvc\\x64\\\\USB2XXX.lib'
libusb_dll_path_64 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\msvc\\x64\\libusb-1.0.dll'
usb2xxx_so_path_x64 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\linux\\x64\\libUSB2XXX.so'
libusb_so_path_x64 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\linux\\x64\\libusb-1.0.so'
usb2xxx_so_path_x86 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\linux\\x86\\libUSB2XXX.so'
libusb_so_path_x86 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\linux\\x86\\libusb-1.0.so'
usb2xxx_so_path_ARMv6 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\linux\\ARMv6\\libUSB2XXX.so'
libusb_so_path_ARMv6 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\linux\\ARMv6\\libusb-1.0.so'
usb2xxx_so_path_ARMv7 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\linux\\ARMv7\\libUSB2XXX.so'
libusb_so_path_ARMv7 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\linux\\ARMv7\\libusb-1.0.so'
usb2xxx_so_path_ARMv8 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\linux\\ARMv8\\libUSB2XXX.so'
libusb_so_path_ARMv8 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\linux\\ARMv8\\libusb-1.0.so'
header_file_path = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\source\\usb2xxx'
usb_header_file_path = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\source'

def copyWinLibFiles():
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        for name in files:# Copy USB2XXX.dll and USB2XXX.lib and libusb-1.0.dll
            if '.vcxproj' in name:
                lib_path = os.path.dirname(os.path.join(root, name))
                shutil.copy(os.path.join(usb2xxx_dll_path_32),os.path.join(lib_path))
                shutil.copy(os.path.join(usb2xxx_lib_path_32),os.path.join(lib_path))
                shutil.copy(os.path.join(libusb_dll_path_32),os.path.join(lib_path))

def backupProject():  
	for root, dirs, files in os.walk(os.getcwd(), topdown=False):
		for name in files:# Remove .sdf file
			if '.sdf' in name:
				os.remove(os.path.join(root, name))
			if '.cmd' in name:
				os.remove(os.path.join(root, name))
			if '.vcxproj.filters' in name:
				os.remove(os.path.join(root, name))
			if '.vcxproj.user' in name:
				os.remove(os.path.join(root, name))
		for name in dirs:# Remove buil directory
			if name == 'Debug' or name == 'ipch'or name == 'Release' or name == 'raspberrypi':
				removeFiles(os.path.join(root, name),())
				removeDirs(os.path.join(root, name))
def copyHeaderFiles():
    if os.path.exists(header_file_path):
        headerFileList = [item for item in filter(lambda file: file.endswith('.h'),os.listdir(header_file_path))]
        print(headerFileList)
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        for name in files:
            if name in headerFileList:
                shutil.copy(os.path.join(header_file_path, name),os.path.join(root, name))
    headerFileList.clear()
    headerFileList.append('usb_device.h')
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        for name in files:
            if name in headerFileList:
                shutil.copy(os.path.join(usb_header_file_path, name),os.path.join(root, name))

def creatLinuxProject():
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        for name in dirs:
            if name == 'vs2010':
                if not os.path.exists(os.path.join(root, 'linux/'+os.path.split(root)[-1]+'/lib/x86')):#创建linux工程目录
                    os.makedirs(os.path.join(root, 'linux/'+os.path.split(root)[-1]+'/lib/x86'))
                if not os.path.exists(os.path.join(root, 'linux/'+os.path.split(root)[-1]+'/lib/x64')):
                    os.makedirs(os.path.join(root, 'linux/'+os.path.split(root)[-1]+'/lib/x64'))
                if not os.path.exists(os.path.join(root, 'linux/'+os.path.split(root)[-1]+'/lib/ARMv7')):
                    os.makedirs(os.path.join(root, 'linux/'+os.path.split(root)[-1]+'/lib/ARMv7'))
    
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        #copy makefile 
        for name in dirs:
            if name == 'linux':
                shutil.copy(os.path.join(os.getcwd(),'./LinuxGccPublicFiles/Makefile'),os.path.join(root, 'linux/'+os.path.split(root)[-1]))
                shutil.copy(os.path.join(os.getcwd(),'./LinuxGccPublicFiles/runme.sh'),os.path.join(root, 'linux/'+os.path.split(root)[-1]))
        #copy lib files
        for name in files:
            if name == 'Makefile':
                lib_root_path = os.path.dirname(os.path.join(root, name))
                if os.path.exists(os.path.join(lib_root_path, 'lib')):
                    shutil.copy(os.path.join(usb2xxx_so_path_x86),os.path.join(root, lib_root_path+'/lib/x86'))
                    shutil.copy(os.path.join(libusb_so_path_x86),os.path.join(root, lib_root_path+'/lib/x86'))
                    shutil.copy(os.path.join(usb2xxx_so_path_x64),os.path.join(root, lib_root_path+'/lib/x64'))
                    shutil.copy(os.path.join(libusb_so_path_x64),os.path.join(root, lib_root_path+'/lib/x64'))
                    shutil.copy(os.path.join(usb2xxx_so_path_ARMv7),os.path.join(root, lib_root_path+'/lib/ARMv7'))
                    shutil.copy(os.path.join(libusb_so_path_ARMv7),os.path.join(root, lib_root_path+'/lib/ARMv7'))
if __name__ == '__main__': 
    backupProject()
    print('Clear Success!')
    copyWinLibFiles()
    print('Copy WinLib File Success!')
    copyHeaderFiles()
    print('Copy Header File Success!')
    creatLinuxProject()
    print('Creat linux project success!')
