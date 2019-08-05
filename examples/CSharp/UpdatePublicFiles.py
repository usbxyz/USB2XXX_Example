import os 
import os.path 
import shutil 
import time,  datetime
import platform

usb2xxx_dll_path_32 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\msvc\\win32\\USB2XXX.dll'
libusb_dll_path_32 = '..\\..\\..\\USB2XXX\\trunk\\USB2XXX\\USB2XXX\\USB2XXXLib\\msvc\\win32\\libusb-1.0.dll'

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
                    shutil.copy(os.path.join(usb2xxx_dll_path_32),os.path.join(root, name))
                    shutil.copy(os.path.join(libusb_dll_path_32),os.path.join(root, name))
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



