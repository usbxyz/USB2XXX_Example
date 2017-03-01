
import com.usbxyz.USB2XXX;

public class USB2XXXUSBDeviceTest {
    /** 
     * Launch the application. 
     */  
    public static void main(String[] args) {   
        int ret;
        int devIndex = 0;
        boolean state;
        //扫描设备
        ret = USB2XXX.INSTANCE.USB_ScanDevice(null);
        if(ret > 0){
        	System.out.println("DeviceNum = "+ret);
        }else{
        	System.out.println("No device");
        	return;
        }
        //打开设备
        state = USB2XXX.INSTANCE.USB_OpenDevice(devIndex);
        if(!state){
        	System.out.println("open device error");
        	return;
        }
        //获取设备信息
        USB2XXX.DEVICE_INFO DevInfo = new USB2XXX.DEVICE_INFO();
        byte[] funcStr = new byte[128];
        state = USB2XXX.INSTANCE.USB_GetDeviceInfo(devIndex,DevInfo,funcStr);
        if(!state){
        	System.out.println("get device infomation error");
        	return;
        }else{
            try {
            	System.out.println("Firmware Info:");
            	System.out.println("--Name:" + new String(DevInfo.FirmwareName, "UTF-8"));
            	System.out.println("--Build Date:" + new String(DevInfo.BuildDate, "UTF-8"));
            	System.out.println(String.format("--Firmware Version:v%d.%d.%d", (DevInfo.FirmwareVersion >> 24) & 0xFF, (DevInfo.FirmwareVersion >> 16) & 0xFF, DevInfo.FirmwareVersion & 0xFFFF));
            	System.out.println(String.format("--Hardware Version:v%d.%d.%d", (DevInfo.HardwareVersion >> 24) & 0xFF, (DevInfo.HardwareVersion >> 16) & 0xFF, DevInfo.HardwareVersion & 0xFFFF));
            	System.out.println("--Functions:" + new String(funcStr, "UTF-8"));
            } catch (Exception ep) {
                ep.printStackTrace();
            }
        }
        //关闭设备
        USB2XXX.INSTANCE.USB_CloseDevice(devIndex);
    }  
    
    
}
