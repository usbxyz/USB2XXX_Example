import com.usbxyz.USB_Device;
import com.usbxyz.USB2LIN;

public class USB2XXX_LIN_Test {
	  /** 
     * Launch the application. 
     */  
    public static void main(String[] args) {   
        int ret;
        int DevHandle = 0;
        int[] DevHandleArry = new int[20];
        boolean state;
        int LINMasterIndex = 0;
        int LINSlaveIndex = 0;
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
        //初始化配置LIN为主机模式
        USB2LIN.LIN_CONFIG LINConfig = new USB2LIN.IIC_CONFIG();
        LINConfig.BaudRate = 19200;
        LINConfig.BreakBits = USB2LIN.LIN_BREAK_BITS_11;
        LINConfig.CheckMode = USB2LIN.LIN_CHECK_MODE_EXT;
        LINConfig.MasterMode = USB2LIN.LIN_MASTER;
        ret = LIN_Init(DevHandle,LINMasterIndex,LINConfig);
        if(ret != LIN_SUCCESS){
            System.out.println("Config LIN failed!");
            return ;
        }else{
            System.out.println("Config LIN Success!");
        }
        //主机写数据
        for(int ID=0;ID<10;ID++){
            USB2LIN.LIN_MSG msg = new USB2LIN.LIN_MSG();
            for(int i=0;i<8;i++){
                msg.Data[i] = ID+i;
            }
            msg.DataLen = 8;
            msg.ID = ID;
            ret = LIN_Write(DevHandle,LINMasterIndex,msg,1);
            if(ret != LIN_SUCCESS){
                System.out.println("LIN write data failed!");
                return;
            }else{
                System.out.println("LIN write data success!");
            }
        }
        //主机读数据
        for(int ID=0;ID<10;ID++){
            USB2LIN.LIN_MSG msg = new USB2LIN.LIN_MSG();
            msg.ID = ID;
            ret = LIN_Read(DevHandle,LINMasterIndex,msg,1);
            if(ret < LIN_SUCCESS){
                System.out.println("LIN read data failed!");
            }else{
                System.out.println("Master LIN Read: ID = 0x%02X Data = ",ID);
                for(int i=0;i<msg.DataLen;i++){
                    System.out.print("0x%02X ",msg.Data[i]);
                }
                System.out.print("\n");
            }
        }
        //初始化配置LIN为从机模式
        USB2LIN.LIN_CONFIG LINConfig = new USB2LIN.IIC_CONFIG();
        LINConfig.BaudRate = 19200;
        LINConfig.BreakBits = USB2LIN.LIN_BREAK_BITS_11;
        LINConfig.CheckMode = USB2LIN.LIN_CHECK_MODE_EXT;
        LINConfig.MasterMode = USB2LIN.LIN_SLAVE;
        ret = LIN_Init(DevHandle,LINSlaveIndex,LINConfig);
        if(ret != LIN_SUCCESS){
            System.out.println("Config LIN failed!");
            return ;
        }else{
            System.out.println("Config LIN Success!");
        }
        //设置从机模式下所有ID都为从接收数据模式，这样就可以获取到主机发送过来的所有数据
        USB2LIN.LIN_MSG[] LINSlaveData = new USB2LIN.LIN_MSG().toArray(64);
        for(int i=0;i<64;i++){
            LINSlaveData[i].DataLen = 9;//最大8Byte数据+1Byte和校验
            LINSlaveData[i].ID = i;		//ID值
        }
        ret = LIN_SlaveSetIDMode(DevHandle,LINSlaveIndex,USB2LIN.LIN_SLAVE_READ,LINSlaveData,64);
        if(ret != LIN_SUCCESS){
            System.out.println("Set LIN operation mode failed!");
            return;
        }else{
            System.out.println("Set LIN operation mode success!");
        }
        //主机发送数据之后，可以读取从机接收到的数据
        USB2LIN.LIN_MSG[] LINSlaveDataBuffer = new USB2LIN.LIN_MSG[1024];//为了防止缓冲区溢出，可以将接收数据缓冲区设置大一点
        ret = LIN_SlaveGetData(DevHandle,LINSlaveIndex,LINSlaveDataBuffer);
        if(ret < LIN_SUCCESS){
            System.out.println("LIN slave read data error!");
            return;
        }else if(ret == 0){
            System.out.println("LIN slave read no data!");
        }else{
            System.out.println("LIN slave read data:");
            for(int i=0;i<ret;i++){
                System.out.print("Data[%d]: ",i);
                System.out.print("ID = 0x%02X ",LINSlaveDataBuffer[i].ID);
                System.out.print("Data = ");
                for(int j=0;j<LINSlaveDataBuffer[i].DataLen;j++){
                    System.out.print("0x%02X ",LINSlaveDataBuffer[i].Data[j]);
                }
                System.out.print("\n");
            }
        }
        System.out.println("");
        System.out.println("USB2LIN test success!");
        //关闭设备
        USB_Device.INSTANCE.USB_CloseDevice(DevHandle);
    }  
}
