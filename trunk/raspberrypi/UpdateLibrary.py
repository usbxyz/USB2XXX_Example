import os 
import os.path 
import shutil 
import time,  datetime
import platform

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


libraryFile = ''
headerFilePath = '/home/pi/Desktop/USB2XXX/source/usb2xxx'
libusb1FilePath = '/usr/local/lib/libusb-1.0.so'
usbDeviceHeaderFile = '/home/pi/Desktop/USB2XXX/source'
libraryFile = '/home/pi/Desktop/USB2XXX/RaspberryPi/USB2XXX/bin/Release/libUSB2XXX.so'

def copyLibFiles():
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        for name in dirs:# Copy USB2XXX.dll and USB2XXX.lib
            if 'RaspberryPi' in name :
                shutil.copy(libraryFile,os.path.join(root, name))
                shutil.copy(libusb1FilePath,os.path.join(root, name))

def copyHeaderFiles():
    if os.path.exists(headerFilePath):
        headerFileList = os.listdir(headerFilePath)
        for fileName in headerFileList:
            if '.cpp' in fileName:
                headerFileList.remove(fileName)
        for fileName in headerFileList:
            if '.cpp' in fileName:
                headerFileList.remove(fileName)
        print(headerFileList)
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        for name in files:
            if name in headerFileList:
                shutil.copy(os.path.join(headerFilePath, name),os.path.join(root, name))
    headerFileList.clear()
    headerFileList.append('usb_device.h')
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        for name in files:
            if name in headerFileList:
                shutil.copy(os.path.join(usbDeviceHeaderFile, name),os.path.join(root, name))

def copyPublicFiles():
    PublicFileList = os.listdir("./PublicFiles")
    print(PublicFileList)
    for root, dirs, files in os.walk(os.getcwd(), topdown=False):
        for name in files:
            if name in PublicFileList and not "PublicFiles" in root:
                shutil.copy(os.path.join("./PublicFiles", name),os.path.join(root, name))

if __name__ == '__main__': 
    copyLibFiles()
    print('Copy File Success!')
    copyHeaderFiles()
    print('Copy Header File Success!')
    copyPublicFiles()
    print('Copy Public File Success!')



