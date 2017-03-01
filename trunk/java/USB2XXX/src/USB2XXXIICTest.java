import com.usbxyz.USB2XXX;

public class USB2XXXIICTest {
	  /** 
     * Launch the application. 
     */  
    public static void main(String[] args) {   
        int ret;
        int DevIndex = 0;
        boolean state;
        int IICIndex = 0;
        byte[] WriteDataBuffer = new byte[1024];
        byte[] ReadDataBuffer = new byte[1024];
        //扫描设备
        ret = USB2XXX.INSTANCE.USB_ScanDevice(null);
        if(ret > 0){
        	System.out.println("Device Num = "+ret);
        }else{
        	System.out.println("No device");
        	return;
        }
        //打开设备
        state = USB2XXX.INSTANCE.USB_OpenDevice(DevIndex);
        if(!state){
        	System.out.println("open device error");
        	return;
        }
        //获取设备信息
        USB2XXX.DEVICE_INFO DevInfo = new USB2XXX.DEVICE_INFO();
        byte[] funcStr = new byte[128];
        state = USB2XXX.INSTANCE.USB_GetDeviceInfo(DevIndex,DevInfo,funcStr);
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
        //配置I2C总线相关参数
        USB2XXX.IIC_CONFIG IICConfig = new USB2XXX.IIC_CONFIG();
        IICConfig.AddrBits = 7;
        IICConfig.ClockSpeedHz = 100000;
        IICConfig.EnablePu = 1;
        IICConfig.Master = 1;
        ret = USB2XXX.INSTANCE.IIC_Init(DevIndex,IICIndex,IICConfig);
        if(ret != USB2XXX.IIC_SUCCESS){
            System.out.println("Initialize device error!\n");
            return;
        }else{
            System.out.println("Initialize device success!\n");
        }
        //写数据
        for(int i=0;i<256;i+=8){
            WriteDataBuffer[0] = (byte)i;//起始地址
            for(int j=0;j<8;j++){
                WriteDataBuffer[1+j] = (byte)(i+j);
            }
            ret = USB2XXX.INSTANCE.IIC_WriteBytes(DevIndex,IICIndex,(short)0x50,WriteDataBuffer,9,10);
            if(ret != USB2XXX.IIC_SUCCESS){
                System.out.println("Write data error!\n");
                return;
            }
            try {
                Thread.sleep(10);
            }catch (Exception ep){
                ep.printStackTrace();
            }
        }
        //读数据
        WriteDataBuffer[0] = 0x00;//起始地址
        ret = USB2XXX.INSTANCE.IIC_WriteReadBytes(DevIndex,IICIndex,(short)0x50,WriteDataBuffer,1,ReadDataBuffer,256,10);
        if(ret != USB2XXX.IIC_SUCCESS){
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
        //关闭设备
        USB2XXX.INSTANCE.USB_CloseDevice(DevIndex);
    }  
}
