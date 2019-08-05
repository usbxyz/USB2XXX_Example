import com.toomoss.USB2XXX.USB_Device;
import com.toomoss.USB2XXX.USB2IIC;

public class USB2XXXIICTest {
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
        //鎵弿璁惧
        ret = USB_Device.INSTANCE.USB_ScanDevice(DevHandleArry);
        if(ret > 0){
        	System.out.println("Device Num = "+ret);
        	DevHandle = DevHandleArry[0];
        }else{
        	System.out.println("No device");
        	return;
        }
        //鎵撳紑璁惧
        state = USB_Device.INSTANCE.USB_OpenDevice(DevHandle);
        if(!state){
        	System.out.println("open device error");
        	return;
        }
        //鑾峰彇璁惧淇℃伅
        USB_Device.DEVICE_INFO DevInfo = new USB_Device.DEVICE_INFO();
        byte[] funcStr = new byte[128];
        state = USB_Device.INSTANCE.DEV_GetDeviceInfo(DevHandle,DevInfo,funcStr);
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
        //閰嶇疆I2C鎬荤嚎鐩稿叧鍙傛暟
        USB2IIC.IIC_CONFIG IICConfig = new USB2IIC.IIC_CONFIG();
        IICConfig.AddrBits = 7;
        IICConfig.ClockSpeedHz = 100000;
        IICConfig.EnablePu = 1;
        IICConfig.Master = 1;
        ret = USB2IIC.INSTANCE.IIC_Init(DevHandle,IICIndex,IICConfig);
        if(ret != USB2IIC.IIC_SUCCESS){
            System.out.println("Initialize device error!\n");
            return;
        }else{
            System.out.println("Initialize device success!\n");
        }
        //鍐欐暟鎹�
        for(int i=0;i<256;i+=8){
            WriteDataBuffer[0] = (byte)i;//璧峰鍦板潃
            for(int j=0;j<8;j++){
                WriteDataBuffer[1+j] = (byte)(i+j);
            }
            ret = USB2IIC.INSTANCE.IIC_WriteBytes(DevHandle,IICIndex,(short)0x50,WriteDataBuffer,9,10);
            if(ret != USB2IIC.IIC_SUCCESS){
                System.out.println("Write data error!\n");
                return;
            }
            try {
                Thread.sleep(10);
            }catch (Exception ep){
                ep.printStackTrace();
            }
        }
        //璇绘暟鎹�
        WriteDataBuffer[0] = 0x00;//璧峰鍦板潃
        ret = USB2IIC.INSTANCE.IIC_WriteReadBytes(DevHandle,IICIndex,(short)0x50,WriteDataBuffer,1,ReadDataBuffer,256,10);
        if(ret != USB2IIC.IIC_SUCCESS){
            System.out.println("Read data error!");
            return;
        }else{
            System.out.println("Read Data:");
        }
        for(int i=0;i<256;i++){
            System.out.print(String.format("%02X ",ReadDataBuffer[i]));
            if(((i+1)%16)==0){
            	System.out.println("");
            }
        }
        System.out.println("");
        System.out.println("24C02 test success!");
        //鍏抽棴璁惧
        USB_Device.INSTANCE.USB_CloseDevice(DevHandle);
    }  
}
