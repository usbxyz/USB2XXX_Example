import com.toomoss.USB2XXX.USB_Device;
import com.toomoss.USB2XXX.USB2CAN;
public class USB2XXX_CANTest {
	  /** 
     * Launch the application. 
     */  
    public static void main(String[] args) {   
        int ret;
        int DevHandle = 0;
        int[] DevHandleArry = new int[20];
        byte CANIndex = 0;
        boolean state;
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
        //初始化配置CAN
        USB2CAN.CAN_INIT_CONFIG CANConfig = new USB2CAN.CAN_INIT_CONFIG();
        CANConfig.CAN_Mode = 1;//环回模式
        CANConfig.CAN_ABOM = 0;//禁止自动离线
        CANConfig.CAN_NART = 1;//禁止报文重传
        CANConfig.CAN_RFLM = 0;//FIFO满之后覆盖旧报文
        CANConfig.CAN_TXFP = 1;//发送请求决定发送顺序
        //配置波特率,波特率 = 100M/(BRP*(SJW+BS1+BS2))
        CANConfig.CAN_BRP = 25;
        CANConfig.CAN_BS1 = 2;
        CANConfig.CAN_BS2 = 1;
        CANConfig.CAN_SJW = 1;
        ret = USB2CAN.INSTANCE.CAN_Init(DevHandle,CANIndex,CANConfig);
        if(ret != USB2CAN.CAN_SUCCESS){
        	System.out.println("Config CAN failed!");
            return;
        }else{
        	System.out.println("Config CAN success!");
        }
        //配置过滤器，必须配置，否则可能无法收到数据
        USB2CAN.CAN_FILTER_CONFIG CANFilter = new USB2CAN.CAN_FILTER_CONFIG();
        CANFilter.Enable = 1;
        CANFilter.ExtFrame = 0;
        CANFilter.FilterIndex = 0;
        CANFilter.FilterMode = 0;
        CANFilter.MASK_IDE = 0;
        CANFilter.MASK_RTR = 0;
        CANFilter.MASK_Std_Ext = 0;
        ret = USB2CAN.INSTANCE.CAN_Filter_Init(DevHandle,CANIndex,CANFilter);
        if(ret != USB2CAN.CAN_SUCCESS){
        	System.out.println("Config CAN filter failed!");
            return;
        }else{
        	System.out.println("Config CAN filter success!");
        }
        //发送CAN数据
        USB2CAN.CAN_MSG[] CanMsg = (USB2CAN.CAN_MSG[])new USB2CAN.CAN_MSG().toArray(5);
        for(int i=0;i<5;i++){
            //CanMsg[i] =  new USB2CAN.CAN_MSG.ByValue();
            CanMsg[i].ExternFlag = 0;
            CanMsg[i].RemoteFlag = 0;
            CanMsg[i].ID = i;
            CanMsg[i].DataLen = 8;
            //CanMsg[i].Data = new byte[8];
            for(byte j=0;j<CanMsg[i].DataLen;j++){
                CanMsg[i].Data[j] = (byte)(i*0x10+j);
            }
        }

        int SendNum = USB2CAN.INSTANCE.CAN_SendMsg(DevHandle, CANIndex, CanMsg, CanMsg.length);
        if (SendNum >= 0) {
        	System.out.println(String.format("Success send frames:%d", SendNum));
        } else {
        	System.out.println("Send CAN data failed!");
        }

        //获取CAN状态
        USB2CAN.CAN_STATUS CANStatus = new USB2CAN.CAN_STATUS();
        ret = USB2CAN.INSTANCE.CAN_GetStatus(DevHandle,CANIndex,CANStatus);
        if(ret == USB2CAN.CAN_SUCCESS){
        	System.out.println(String.format("TSR = %08X",CANStatus.TSR));
        	System.out.println(String.format("ESR = %08X",CANStatus.ESR));
        }else{
        	System.out.println("Get CAN status error!");
        }
        //延时
        try {
            Thread.sleep(100);
        }catch (Exception ep){
            ep.printStackTrace();
        }
        //读取数据
        USB2CAN.CAN_MSG[] CanMsgBuffer = new USB2CAN.CAN_MSG[10];
        int CanNum = USB2CAN.INSTANCE.CAN_GetMsg(DevHandle,CANIndex,CanMsgBuffer);
        if(CanNum > 0){
            System.out.println(String.format("Get CAN CanNum = %d",CanNum));
            for(int i=0;i<CanNum;i++){
                System.out.println(String.format("CanMsg[%d].ID = %d",i,CanMsgBuffer[i].ID));
                System.out.println(String.format("CanMsg[%d].TimeStamp = %d",i,CanMsgBuffer[i].TimeStamp));
                System.out.println(String.format("CanMsg[%d].Data = ",i));
                for(int j=0;j<CanMsgBuffer[i].DataLen;j++){
                    System.out.print(String.format("%02X ",CanMsgBuffer[i].Data[j]));
                }
                System.out.println("");
            }
        }else if(CanNum == 0){
            System.out.println("No CAN data!");
        }else{
            System.out.println("Get CAN data error!");
        }
        //关闭设备
        System.out.println("CAN test success!");
    }
}
