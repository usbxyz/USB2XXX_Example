import com.toomoss.USB2XXX.USB_Device;
import com.toomoss.USB2XXX.USB2LIN_EX;
import com.toomoss.USB2XXX.USB2LIN_EX.LIN_EX_MSG;

public class USB2XXX_LIN_EX_MasterTest {
	  /** 
     * Launch the application. 
     */  
    public static void main(String[] args) {   
        int ret;
        int DevHandle = 0;
        int[] DevHandleArry = new int[20];
        boolean state;
        byte LINMasterIndex = 0;
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
        //初始化配置LIN
        ret = USB2LIN_EX.INSTANCE.LIN_EX_Init(DevHandle,LINMasterIndex,9600,USB2LIN_EX.LIN_EX_MASTER);
        if(ret != USB2LIN_EX.LIN_EX_SUCCESS){
        	System.out.println("Config LIN failed!");
            return;
        }else{
        	System.out.println("Config LIN Success!");
        }
        //主机写数据，一次性发送5帧数据出去
        LIN_EX_MSG[] LINInMsg = (LIN_EX_MSG[]) new LIN_EX_MSG().toArray(5);
        LIN_EX_MSG[] LINOutMsg = (LIN_EX_MSG[]) new LIN_EX_MSG().toArray(5);
        //第一帧为BREAK信号
        LINInMsg[0].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_BK;
        LINInMsg[0].Timestamp = 10;//发送该帧数据之后的延时时间，最小建议设置为1
        LINInMsg[0].BreakBits = 13;//设置BREAK信号为13bit
        //后面4个ID全部为主机发送数据模式
        for(int f=1;f<LINInMsg.length;f++){
        	LINInMsg[f].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_MW;//主机写数据模式
        	LINInMsg[f].DataLen = 8;
            for(int i=0;i<LINInMsg[f].DataLen;i++){
            	LINInMsg[f].Data[i] = (byte) ((f<<4)|i);
            }
            LINInMsg[f].BreakBits = 13;//设置BREAK信号为13bit
            LINInMsg[f].Timestamp = 10;//发送该帧数据之后的延时时间，最小建议设置为1
            LINInMsg[f].CheckType = USB2LIN_EX.LIN_EX_CHECK_EXT;//设置好校验类型后，适配器会根据校验类型自动计算校验数据
            LINInMsg[f].PID = (byte) (f+1);
        }
        ret = USB2LIN_EX.INSTANCE.LIN_EX_MasterSync(DevHandle,LINMasterIndex,LINInMsg,LINOutMsg,LINInMsg.length);
        if(ret < USB2LIN_EX.LIN_EX_SUCCESS){
        	System.out.println("MasterSync LIN failed!");
            return;
        }else{
        	//可以将接收回来的数据跟发送出去的数据进行对比，如此可以判断数据是否成功发送出去
        	System.out.printf("Success Write MsgNum = %d\n",ret);
            for(int i=0;i<ret;i++){
            	System.out.printf("PID[%02X] ",LINOutMsg[i].PID);
                for(int j=0;j<LINOutMsg[i].DataLen;j++){
                	System.out.printf("%02X ",LINOutMsg[i].Data[j]);
                }
                System.out.printf("[%02X] [%02d:%02d:%02d.%03d]\n",LINOutMsg[i].Check,(LINOutMsg[i].Timestamp/36000000)%60,(LINOutMsg[i].Timestamp/600000)%60,(LINOutMsg[i].Timestamp/10000)%60,(LINOutMsg[i].Timestamp/10)%1000);
            }
        }
        
        //后面4个ID全部为主机读数据模式
        for(int f=1;f<LINInMsg.length;f++){
        	LINInMsg[f].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_MR;//主机读数据模式
        	LINInMsg[f].BreakBits = 13;//设置BREAK信号为13bit
        	LINInMsg[f].Timestamp = 10;//主机读取完数据之后的延时时间，最小建议设置为1
        	LINInMsg[f].PID = (byte) (f+1);
        }
        ret = USB2LIN_EX.INSTANCE.LIN_EX_MasterSync(DevHandle,LINMasterIndex,LINInMsg,LINOutMsg,LINInMsg.length);
        if(ret < USB2LIN_EX.LIN_EX_SUCCESS){
        	System.out.printf("MasterSync LIN failed!");
            return;
        }else{
        	//若成功读取到数据，那么读到的数据字节数在LINOutMsg[i].DataLen中
        	System.out.printf("Success Read MsgNum = %d\n",ret);
            for(int i=1;i<ret;i++){
            	System.out.printf("PID[%02X] ",LINOutMsg[i].PID);
                for(int j=0;j<LINOutMsg[i].DataLen;j++){
                	System.out.printf("%02X ",LINOutMsg[i].Data[j]);
                }
                System.out.printf("[%02X] [%02d:%02d:%02d.%03d]\n",LINOutMsg[i].Check,(LINOutMsg[i].Timestamp/36000000)%60,(LINOutMsg[i].Timestamp/600000)%60,(LINOutMsg[i].Timestamp/10000)%60,(LINOutMsg[i].Timestamp/10)%1000);
            }
        }
        
        
        //配置5帧数据在适配器内部自动发送数据
        //第一帧为BREAK信号
        LINInMsg[0].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_BK;
        LINInMsg[0].Timestamp = 10;//发送该帧数据之后的延时时间，最小建议设置为1
        LINInMsg[0].BreakBits = 13;//设置BREAK信号为13bit
        //后面4个ID全部为主机发送数据模式
        for(int f=1;f<LINInMsg.length;f++){
        	LINInMsg[f].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_MW;//主机写数据模式
        	LINInMsg[f].DataLen = 8;
            for(int i=0;i<LINInMsg[f].DataLen;i++){
            	LINInMsg[f].Data[i] = (byte) ((f<<4)|i);
            }
            LINInMsg[f].BreakBits = 13;//设置BREAK信号为13bit
            LINInMsg[f].Timestamp = 10;//发送该帧数据之后的延时时间，最小建议设置为1
            LINInMsg[f].CheckType = USB2LIN_EX.LIN_EX_CHECK_EXT;//设置好校验类型后，适配器会根据校验类型自动计算校验数据
            LINInMsg[f].PID = (byte) (f+1);
        }
        ret = USB2LIN_EX.INSTANCE.LIN_EX_MasterStartSch(DevHandle,LINMasterIndex,LINInMsg,LINInMsg.length);
        if(ret != USB2LIN_EX.LIN_EX_SUCCESS){
        	System.out.printf("MasterStartSch LIN failed!");
            return;
        }
        //延时一段时间，让适配器自动发送数据
        try {
			Thread.sleep(3000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        //停止自动发送数据
        ret = USB2LIN_EX.INSTANCE.LIN_EX_MasterStopSch(DevHandle, LINMasterIndex);
        if(ret != USB2LIN_EX.LIN_EX_SUCCESS){
        	System.out.printf("MasterStopSch LIN failed!");
            return;
        }
        
        System.out.println("");
        System.out.println("USB2LIN test success!");
        //关闭设备
        USB_Device.INSTANCE.USB_CloseDevice(DevHandle);
    }  
}
