import com.toomoss.USB2XXX.USB_Device;
import com.toomoss.USB2XXX.USB2LIN;
import com.toomoss.USB2XXX.USB2LIN.LIN_MSG;

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
        //扫描当前连接的设备
        ret = USB_Device.INSTANCE.USB_ScanDevice(DevHandleArry);
        if(ret > 0){
        	System.out.println("Device Num = "+ret);
        	DevHandle = DevHandleArry[0];
        }else{
        	System.out.println("No device");
        	return;
        }
        //打开当前连接的设备
        state = USB_Device.INSTANCE.USB_OpenDevice(DevHandle);
        if(!state){
        	System.out.println("open device error");
        	return;
        }
        //读取设备信息
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
        //配置为主机模式
        USB2LIN.LIN_CONFIG LINConfig = new USB2LIN.LIN_CONFIG();
        LINConfig.BaudRate = 19200;
        LINConfig.BreakBits = USB2LIN.LIN_BREAK_BITS_11;
        LINConfig.CheckMode = USB2LIN.LIN_CHECK_MODE_EXT;
        LINConfig.MasterMode = USB2LIN.LIN_MASTER;
        ret = USB2LIN.INSTANCE.LIN_Init(DevHandle,(byte) LINMasterIndex,LINConfig);
        if(ret != USB2LIN.INSTANCE.LIN_SUCCESS){
            System.out.println("Config LIN failed!");
            return ;
        }else{
            System.out.println("Config LIN Success!");
        }
        //主机模式循环发送数据
        for(int ID=0;ID<10;ID++){
            USB2LIN.LIN_MSG[] msg = (LIN_MSG[]) new USB2LIN.LIN_MSG().toArray(1);
            for(int i=0;i<8;i++){
                msg[0].Data[i] = (byte) (ID+i);
            }
            msg[0].DataLen = 8;
            msg[0].ID = (byte) ID;
            ret = USB2LIN.INSTANCE.LIN_Write(DevHandle,(byte)LINMasterIndex,msg,1);
            if(ret != USB2LIN.INSTANCE.LIN_SUCCESS){
                System.out.println("LIN write data failed!");
                return;
            }else{
                System.out.println("LIN write data success!");
            }
        }
        //主机模式循环读取数据
        for(int ID=0;ID<10;ID++){
            USB2LIN.LIN_MSG[] msg = (LIN_MSG[]) new USB2LIN.LIN_MSG().toArray(1);
            msg[0].ID = (byte) ID;
            ret = USB2LIN.INSTANCE.LIN_Read(DevHandle,(byte)LINMasterIndex,msg,1);
            if(ret < USB2LIN.INSTANCE.LIN_SUCCESS){
                System.out.println("LIN read data failed!");
            }else{
                System.out.printf("Master LIN Read: ID = 0x%02X Data = ",ID);
                for(int i=0;i<msg[0].DataLen;i++){
                    System.out.printf("0x%02X ",msg[0].Data[i]);
                }
                System.out.print("\n");
            }
        }
        //鍒初始化配置为从机模式
        LINConfig.BaudRate = 19200;
        LINConfig.BreakBits = USB2LIN.LIN_BREAK_BITS_11;
        LINConfig.CheckMode = USB2LIN.LIN_CHECK_MODE_EXT;
        LINConfig.MasterMode = USB2LIN.LIN_SLAVE;
        ret = USB2LIN.INSTANCE.LIN_Init(DevHandle,(byte)LINSlaveIndex,LINConfig);
        if(ret != USB2LIN.INSTANCE.LIN_SUCCESS){
            System.out.println("Config LIN failed!");
            return ;
        }else{
            System.out.println("Config LIN Success!");
        }
        //将从机模式下的所有帧配置为从机读数据模式，该模式下可以监听LIN总线上的数据
        USB2LIN.LIN_MSG[] LINSlaveData = (LIN_MSG[]) new USB2LIN.LIN_MSG().toArray(64);
        for(int i=0;i<64;i++){
            LINSlaveData[i].DataLen = 9;	//最大8Byte数据+1Byte和校验
            LINSlaveData[i].ID = (byte) i;	//ID值
        }
        ret = USB2LIN.INSTANCE.LIN_SlaveSetIDMode(DevHandle,(byte)LINSlaveIndex,USB2LIN.LIN_SLAVE_READ,LINSlaveData,64);
        if(ret != USB2LIN.INSTANCE.LIN_SUCCESS){
            System.out.println("Set LIN operation mode failed!");
            return;
        }else{
            System.out.println("Set LIN operation mode success!");
        }
        //获取从机模式下接收到的数据
        USB2LIN.LIN_MSG[] LINSlaveDataBuffer = (LIN_MSG[]) new USB2LIN.LIN_MSG().toArray(1024);//接收数据缓冲区尽量大一点，防止缓冲区溢出
        ret = USB2LIN.INSTANCE.LIN_SlaveGetData(DevHandle,(byte)LINSlaveIndex,LINSlaveDataBuffer);
        if(ret < USB2LIN.INSTANCE.LIN_SUCCESS){
            System.out.println("LIN slave read data error!");
            return;
        }else if(ret == 0){
            System.out.println("LIN slave read no data!");
        }else{
            System.out.println("LIN slave read data:");
            for(int i=0;i<ret;i++){
                System.out.printf("Data[%d]: ",i);
                System.out.printf("ID = 0x%02X ",LINSlaveDataBuffer[i].ID);
                System.out.printf("Data = ");
                for(int j=0;j<LINSlaveDataBuffer[i].DataLen;j++){
                    System.out.printf("0x%02X ",LINSlaveDataBuffer[i].Data[j]);
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
