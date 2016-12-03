import os 
import os.path 
import shutil 
import time,  datetime
import platform

libraryFile = ''
libusb0FilePath = '/usr/local/lib/libusb.so'
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

def copyLibFiles():
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        for name in dirs:# Copy USB2XXX.dll and libusb-1.0.dll
            allDirName = os.path.join(root, name)
            if(platform.system()=="Windows"):
                if 'bin' in allDirName and ('Debug' in allDirName or 'Release' in allDirName):
                    shutil.copy(USB2XXXDllFile,os.path.join(root, name))
                    shutil.copy(libusbDllFile,os.path.join(root, name))
            else:
                print("system error")
                exit()

def copyPublicFiles():
    # shutil.copy(usbDeviceHeaderFile,'./PublicFiles');# copy the header file to ./PublicFiles
    PublicFileList = os.listdir("./PublicFiles")
    print(PublicFileList)
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        for name in files:
            if name in PublicFileList and not "PublicFiles" in root:
                shutil.copy(os.path.join("./PublicFiles", name),os.path.join(root, name))

def backupProject():  
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        for name in files:# Remove .sdf file
            if '.exe' in name:
                os.remove(os.path.join(root, name))
            if '.pdb' in name:
                os.remove(os.path.join(root, name))
        for name in dirs:# Remove buil directory
            if name == 'obj':
                removeFiles(os.path.join(root, name),())
                removeDirs(os.path.join(root, name))

if __name__ == '__main__': 
    backupProject()
    print('Backup File Success!')
    copyLibFiles()
    print('Copy Lib File Success!')
    copyPublicFiles()
    print('Copy Public File Success!')



