import com.usbxyz.USB_Device;

public class USB2XXXSPIW25Q32 {
	  /** 
     * Launch the application. 
     */  
    public static void main(String[] args) {   
        int ret;
        int DevHandle = 0;
        int[] DevHandleArry = new int[20];
        boolean state;
        int IICIndex = 0;
        byte[] WriteDataBuffer = new byte[1024];
        byte[] ReadDataBuffer = new byte[1024];
        //扫描设备
        ret = USB_Device.INSTANCE.USB_ScanDevice(DevHandleArry);
        if(ret > 0){
        	System.out.println("Device Num = "+ret);
        	DevHandle = DevHandleArry[0];
        }else{
        	System.out.println("No device");
        	return;
        }
        //打开设备
        state = USB_Device.INSTANCE.USB_OpenDevice(DevHandle);
        if(!state){
        	System.out.println("open device error");
        	return;
        }
        //获取设备信息
        USB_Device.DEVICE_INFO DevInfo = new USB_Device.DEVICE_INFO();
        byte[] funcStr = new byte[128];
        state = USB_Device.INSTANCE.USB_GetDeviceInfo(DevHandle,DevInfo,funcStr);
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
    }
}
