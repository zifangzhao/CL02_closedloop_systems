using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using System.Threading;
using System.Management;
using System.IO;

using System.Net;
using System.Net.Sockets;
using System.Reflection;
using System.Diagnostics;
using ComboboxItem = System.Windows.Controls.ComboBoxItem;

namespace CL02_center
{
    enum miscDigisig_t : uint
    {
        MISC_SIG_TRIG1 = 0x01,
        MISC_SIG_STIM1 = 0x02,
        MISC_SIG_TRIG2 = 0x04,
        MISC_SIG_STIM2 = 0x08,
        MISC_SIG_AUX1 = 0x10,
        MISC_SIG_AUX2 = 0x20,
        MISC_SIG_AUX3 = 0x40,
        MISC_SIG_EXT1 = 0x80,
        MISC_SIG_EXT2 = 0x100,
        MISC_SIG_EXT3 = 0x200,
        MISC_SIG_UNDERVOLT = 0x400,
        MISC_SIG_DSP_WAIT = 0x800,
        MISC_SIG_DSP_CAL = 0x1000,
        MISC_SIG_DSP_READY = 0x2000,
        MISC_SIG_LD1 = 0x4000,
        MISC_SIG_LD2 = 0x8000
    }
    public partial class CL02_center_MainInterface : Form
    {

        private List<Panel> Panels = new List<Panel>();

        private List<RealTimeCurveMaker> CurveMakers;

        private DataSource Rawdata;

        const int DSP_count = 2;

        private CE_core DeviceParams;

        private string[] portname;

        private int sample_rate = 1000;

        private int[] DisplayLen = new int[] { 1, 2, 5, 10 };

        const double DSP_offset = 0.4;

        private decimal DSP_id_curr = 0;

        private miscDigisig_t dout_type = miscDigisig_t.MISC_SIG_STIM1;

        private List<double> Threshs = null;

        private List<bool> PaintThresh = new List<bool> { false, false, true, false };

        private int filter_ID;
        private bool[] custom_filter_loaded= new bool[] { false, false };
        System.IO.FileStream fs;
        System.IO.BinaryWriter strWR;
        System.IO.BufferedStream fs_buf;

        System.IO.FileStream fs_aux;
        System.IO.BinaryWriter strWR_aux;
        System.IO.BufferedStream fs_buf_aux;

        private double[] Disp_HP_IIR_NUM = { 0.997781024102941, -1.99556204820588, 0.997781024102941 };
        private double[] Disp_HP_IIR_DEN = { 1, -1.99555712434579, 0.995566972065975 };

        private CE32_filter[] filters = new CE32_filter[4];

        private byte[] filter_param;

        System.Timers.Timer timer_trigger;
        public delegate void SetExtTrigger(int i);

        //Socket define
        public int port = 2000;
        public string host = "127.0.0.1";
        public Socket socket;
        public CL02_center_MainInterface()
        {
            InitializeComponent();
            Initialize();
            Control.CheckForIllegalCrossThreadCalls = false;
        }
        public class Info
        {
            public double Value { get; set; }
            public string Name { get; set; }
        }

        private void Initialize()
        {
            int pannelnum = GetPanelNum();
            InitPanels(pannelnum);

            //initialize dsp
            filters[0] = new CE32_filter(Disp_HP_IIR_NUM, Disp_HP_IIR_DEN);
            filters[1] = new CE32_filter(Disp_HP_IIR_NUM, Disp_HP_IIR_DEN);

            string[] temp = new string[] { "1s", "2s", "5s", "10s" };
            this.comboBoxDisplayLen.DataSource = temp;
            this.comboBoxDisplayLen.SelectedIndex = temp.Length - 1;
            {
                IList<Info> infoList = new List<Info>();
                Info info = new Info() { Value = 3.3 / 65536 / 5 * 2, Name = "5V" };
                infoList.Add(info);
                info = new Info() { Value = 3.3 / 65536 * 2, Name = "1V" };
                infoList.Add(info);
                info = new Info() { Value = 3.3 / 65536 * 2 * 2, Name = "500mV" };
                infoList.Add(info);
                info = new Info() { Value = 3.3 / 65536 * 10 * 2, Name = "100mV" };
                infoList.Add(info);
                info = new Info() { Value = 3.3 / 65536 * 20 * 2, Name = "50mV" };
                infoList.Add(info);
                info = new Info() { Value = 3.3 / 65536 * 100 * 2, Name = "10mV" };
                infoList.Add(info);
                info = new Info() { Value = 3.3 / 65536 * 200 * 2, Name = "5mV" };
                infoList.Add(info);
                info = new Info() { Value = 3.3 / 65536 * 1000 * 2, Name = "1mV" };
                infoList.Add(info);
                comboBoxDisplayGain.DataSource = infoList;
                comboBoxDisplayGain.ValueMember = "Value";
                comboBoxDisplayGain.DisplayMember = "Name";
            }
            {
                IList<Info> infoList1 = new List<Info>();
                Info info1 = new Info() { Value = 3.3 / 65536 / 5 * 2, Name = "5V" };
                infoList1.Add(info1);
                info1 = new Info() { Value = 3.3 / 65536 * 2, Name = "1V" };
                infoList1.Add(info1);
                info1 = new Info() { Value = 3.3 / 65536 * 2 * 2, Name = "500mV" };
                infoList1.Add(info1);
                info1 = new Info() { Value = 3.3 / 65536 * 10 * 2, Name = "100mV" };
                infoList1.Add(info1);
                info1 = new Info() { Value = 3.3 / 65536 * 20 * 2, Name = "50mV" };
                infoList1.Add(info1);
                info1 = new Info() { Value = 3.3 / 65536 * 100 * 2, Name = "10mV" };
                infoList1.Add(info1);
                info1 = new Info() { Value = 3.3 / 65536 * 200 * 2, Name = "5mV" };
                infoList1.Add(info1);
                info1 = new Info() { Value = 3.3 / 65536 * 1000 * 2, Name = "1mV" };
                infoList1.Add(info1);
                comboBoxDSPGain.DataSource = infoList1;
                comboBoxDSPGain.ValueMember = "Value";
                comboBoxDSPGain.DisplayMember = "Name";
            }
            {
                IList<Info> infoList = new List<Info>();
                Info info = new Info() { Value = 0, Name = "Disabled" };
                infoList.Add(info);
                info = new Info() { Value = 1, Name = "Single A" };
                infoList.Add(info);
                info = new Info() { Value = 3, Name = "Cascade A->B" };
                infoList.Add(info);
                info = new Info() { Value = 4, Name = "Gated A & B" };
                infoList.Add(info);
                info = new Info() { Value = 5, Name = "Random" };
                infoList.Add(info);
                comboBoxDSPmode.DataSource = infoList;
                comboBoxDSPmode.ValueMember = "Value";
                comboBoxDSPmode.DisplayMember = "Name";
            }
            Rawdata = new DataSource(pannelnum, DisplayLen[3] * sample_rate, 100,sample_rate);//设置的缓冲区初始长度5000s

            CurveMakers = new List<RealTimeCurveMaker>();
            for (var i = 0; i < pannelnum; ++i)
            {
                CurveMakers.Add(new RealTimeCurveMaker(Rawdata.sampleRate, Panels[i].Width, this.DisplayLen[this.comboBoxDisplayLen.SelectedIndex], i));
            }

            DeviceParams = new CE_core(2);

            comboBox_CL_FilterType.SelectedIndex = 6;
            comboBox_CL_Arb.SelectedIndex = 0;
            comboBoxDSPmode.SelectedIndex = 1;
            for (int i = 0; i < DSP_count; i++)
            {
                DSP_id_curr = DSP_count - i -1;
                UpdParams();
            }

            comboBox_TrigMode.SelectedIndex = 0;
            //InitSocket();
            InitTimer();
        }

        private void InitPanels(int panelnum)
        {
            var height = this.flowLayoutPanel1.Size.Height;
            var width = this.flowLayoutPanel1.Size.Width;
            var panelgap = 3;//这个默认值是3
            height -= (4 * panelgap + 1);
            width -= (4 * panelgap + 1);

            if (panelnum == 4)
            {
                height /= 2;
                width /= 2;
                //panelgap *= 2;
            }

            string[] names = { "CH1", "CH2", "DSP", "DOUT" };

            for (int i = 0; i < panelnum; i++)
            {
                var p = new Panel() { Height = height, Width = width, BackColor = Color.LightGray };
                p.Margin = new Padding(panelgap, panelgap, panelgap, panelgap);

                var label1 = new Label();//左边的标签
                label1.Text = "Wave";
                label1.TextAlign = ContentAlignment.MiddleCenter;
                label1.Location = new Point(0, 0);
                p.Controls.Add(label1);

                var label2 = new Label();//左边的标签
                label2.Text = names[i];
                label2.TextAlign = ContentAlignment.MiddleCenter;
                label2.Location = new Point(p.Width - label2.Width, 0);
                p.Controls.Add(label2);

                var innerpanel = new Panel() { Height = p.Height - label1.Height, Width = p.Width, BackColor = Color.White };
                innerpanel.Location = new Point(0, label1.Height);
                p.Controls.Add(innerpanel);

                this.flowLayoutPanel1.Controls.Add(p);
                this.Panels.Add(innerpanel);
            }
        }

        //该方法返回需要加几个panel
        private int GetPanelNum()
        {
            return 4;
        }

        //
        private void InitTimer()
        {
            //设置定时间隔(毫秒为单位)
            int interval = 100;
            timer_trigger = new System.Timers.Timer(interval);
            //设置执行一次（false）还是一直执行(true)
            timer_trigger.AutoReset = true;
            //设置是否执行System.Timers.Timer.Elapsed事件
            timer_trigger.Enabled = false;
            //绑定Elapsed事件
            timer_trigger.Elapsed += new System.Timers.ElapsedEventHandler(TimerUp);
        }

        private void TimerUp(object sender, System.Timers.ElapsedEventArgs e)
        {
            try
            {
                Console.WriteLine("reading");
                BinaryReader br = new BinaryReader(new FileStream("d:\\cl02_control.bin",FileMode.Open,FileAccess.Read));
                int i = br.ReadInt32();
                this.Invoke(new SetExtTrigger(UpdateExtTrigger), i);
                br.Dispose();

            }
            catch (IOException err)
            {

            }
        }

        private void InitSocket()
        {
            IPAddress ip = IPAddress.Parse(host);//把ip地址字符串转换为IPAddress类型的实例
            IPEndPoint ipe = new IPEndPoint(ip, port);//用指定的端口和ip初始化IPEndPoint类的新实例

            ///创建socket并开始监听
            socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);//创建一个socket对像，如果用udp协议，则要用SocketType.Dgram类型的套接字
            socket.Bind(ipe);//绑定EndPoint对像（2000端口和ip地址）
            socket.Listen(0);//开始监听
            Console.WriteLine("Waiting for connection from client");
            SocketBeginAccept();
        }

        private void SocketBeginAccept()
        {
            socket.BeginAccept(SocketPackageReceived, socket);//为新建连接创建新的socket
        }

        private void SocketPackageReceived(IAsyncResult ar)
        {
            Console.WriteLine("Connection established");
            Socket listener = (Socket)ar.AsyncState;

            // End the operation and display the received data on the console.
            byte[] Buffer;
            int bytesTransferred;
            Socket handler = listener.EndAccept(out Buffer, out bytesTransferred, ar);
            if (Buffer[bytesTransferred-1]==0)
            {
                UpdateExtTrigger(0);
            }
            else
            {
                UpdateExtTrigger(1);
            }
            socket.BeginAccept(SocketPackageReceived, socket);//为新建连接创建新的socket

        }
        private void  UpdateExtTrigger(int i)
        {
            if (i==0)
            {
                checkBox2.Checked = false;
            }
            else
            {
                checkBox2.Checked = true;
            }
        }
    private void button1_Click(object sender, EventArgs e)
        {
            checkBox_LogFile.Enabled = false;
            comboBoxPort.Items.Clear();
            string name = "com" + GetComNum().ToString();
            //portname = System.IO.Ports.SerialPort.GetPortNames();
            //foreach (string i in portname)
            //{
            //    comboBoxPort.Items.Add(i);
            //}
            comboBoxPort.Items.Add(name);
            comboBoxPort.Text = name;

            try
            {
                if(checkBox_LogFile.Checked==true)
                {
                    saveFileDialog1.ShowDialog();
                    label_OutputFile.Text = saveFileDialog1.FileName;

                        
                    string basefilename = saveFileDialog1.FileName;
                    basefilename = basefilename.Remove(basefilename.Length - 4);
                    fs = new System.IO.FileStream(basefilename + ".dat", System.IO.FileMode.Create, System.IO.FileAccess.ReadWrite, System.IO.FileShare.ReadWrite);
                    fs_buf = new System.IO.BufferedStream(fs, 512);
                    fs.Position = 0;
                    strWR = new System.IO.BinaryWriter(fs_buf);
                }

                serialPort1.PortName = name;
                serialPort1.Open();
                serialPort1.DiscardInBuffer();

                SendSettings(); //Sync settings

                //Start recording
                byte[] cmd = { 0x3c, 0x40, 0x3e };
                serialPort1.Write(cmd, 0, cmd.Length);
                //this.timer_painter.Enabled = true;

                
                buttonStartCollection.Enabled = false;
                buttonStopCollection.Enabled = true;
            }
            catch (Exception)
            {

                //throw;
                MessageBox.Show("Failed to open COM port");
            }
            
        }

        private void DrawCurve()
        {
            for (var i = 0; i < Panels.Count; ++i)
            {
                Graphics g = Panels[i].CreateGraphics();
                int startpos = CurveMakers[i].GetNewestStartPos(Rawdata, this.DisplayLen[this.comboBoxDisplayLen.SelectedIndex]);
                PaintInfo paintinfo = new PaintInfo();
                paintinfo.PaintThreshFlag = this.PaintThresh[i];
                paintinfo.Threshs = this.Threshs;
                Image temp = CurveMakers[i].GetCurveImage(Rawdata, startpos, Panels[i].Width, this.DisplayLen[this.comboBoxDisplayLen.SelectedIndex], Panels[i].Height, Panels[i].Width,paintinfo);

                g.DrawImage(temp, 0, 0);
                g.Dispose();
                temp.Dispose();
            }
        }

        private void timer_painter_Tick(object sender, EventArgs e)
        {
            double[,] test = new double[4,100];
            Rawdata.FetchData(test, 100);
            //Rawdata.FetchData();//把这两个放在传数据的timer里就能用
            DrawCurve();
        }

        private void buttonStopCollection_Click(object sender, EventArgs e)
        {

            checkBox_LogFile.Enabled = true;
            this.timer_painter.Enabled = false;
            buttonStartCollection.Enabled = true;
            buttonStopCollection.Enabled = false;
            if (serialPort1.IsOpen)
            {
                byte[] cmd = { 0x3c, 0x41, 0x3e };
                serialPort1.Write(cmd, 0, cmd.Length);
                serialPort1.Close();
            }
            
        }

        /// <summary>
        /// Get the target com num.
        /// </summary>
        /// <returns></returns>
        public static int GetComNum()
        {
            int comNum = -1;
            string[] strArr = GetHarewareInfo(HardwareEnum.Win32_PnPEntity, "Name");
            foreach (string s in strArr)
            {
                if (s.Contains("USB Serial Device"))
                {
                    int start = s.IndexOf("(") + 3;
                    int end = s.IndexOf(")");
                    comNum = Convert.ToInt32(s.Substring(start + 1, end - start - 1));
                }
            }
            if (comNum <= 0)
            {
                foreach (string s in strArr)
                {
                    if (s.Length >= 23 && s.Contains("COM Port"))
                    {
                        int start = s.IndexOf("(") + 3;
                        int end = s.IndexOf(")");
                        comNum = Convert.ToInt32(s.Substring(start + 1, end - start - 1));
                    }
                }
            }

            return comNum;

        }

        /// <summary>
        /// Get the system devices information with windows api.
        /// </summary>
        /// <param name="hardType">Device type.</param>
        /// <param name="propKey">the property of the device.</param>
        /// <returns></returns>
        private static string[] GetHarewareInfo(HardwareEnum hardType, string propKey)
        {

            List<string> strs = new List<string>();
            try
            {
                using (ManagementObjectSearcher searcher = new ManagementObjectSearcher("select * from " + hardType))
                {
                    var hardInfos = searcher.Get();
                    foreach (var hardInfo in hardInfos)
                    {
                        if (hardInfo.Properties[propKey].Value != null)
                        {
                            String str = hardInfo.Properties[propKey].Value.ToString();
                            strs.Add(str);
                        }

                    }
                }
                return strs.ToArray();
            }
            catch
            {
                return null;
            }
            finally
            {
                strs = null;
            }
        }//end of func GetHarewareInfo().

        /// <summary>
        /// 枚举win32 api
        /// </summary>
        public enum HardwareEnum
        {
            // 硬件
            Win32_Processor, // CPU 处理器
            Win32_PhysicalMemory, // 物理内存条
            Win32_Keyboard, // 键盘
            Win32_PointingDevice, // 点输入设备，包括鼠标。
            Win32_FloppyDrive, // 软盘驱动器
            Win32_DiskDrive, // 硬盘驱动器
            Win32_CDROMDrive, // 光盘驱动器
            Win32_BaseBoard, // 主板
            Win32_BIOS, // BIOS 芯片
            Win32_ParallelPort, // 并口
            Win32_SerialPort, // 串口
            Win32_SerialPortConfiguration, // 串口配置
            Win32_SoundDevice, // 多媒体设置，一般指声卡。
            Win32_SystemSlot, // 主板插槽 (ISA & PCI & AGP)
            Win32_USBController, // USB 控制器
            Win32_NetworkAdapter, // 网络适配器
            Win32_NetworkAdapterConfiguration, // 网络适配器设置
            Win32_Printer, // 打印机
            Win32_PrinterConfiguration, // 打印机设置
            Win32_PrintJob, // 打印机任务
            Win32_TCPIPPrinterPort, // 打印机端口
            Win32_POTSModem, // MODEM
            Win32_POTSModemToSerialPort, // MODEM 端口
            Win32_DesktopMonitor, // 显示器
            Win32_DisplayConfiguration, // 显卡
            Win32_DisplayControllerConfiguration, // 显卡设置
            Win32_VideoController, // 显卡细节。
            Win32_VideoSettings, // 显卡支持的显示模式。

            // 操作系统
            Win32_TimeZone, // 时区
            Win32_SystemDriver, // 驱动程序
            Win32_DiskPartition, // 磁盘分区
            Win32_LogicalDisk, // 逻辑磁盘
            Win32_LogicalDiskToPartition, // 逻辑磁盘所在分区及始末位置。
            Win32_LogicalMemoryConfiguration, // 逻辑内存配置
            Win32_PageFile, // 系统页文件信息
            Win32_PageFileSetting, // 页文件设置
            Win32_BootConfiguration, // 系统启动配置
            Win32_ComputerSystem, // 计算机信息简要
            Win32_OperatingSystem, // 操作系统信息
            Win32_StartupCommand, // 系统自动启动程序
            Win32_Service, // 系统安装的服务
            Win32_Group, // 系统管理组
            Win32_GroupUser, // 系统组帐号
            Win32_UserAccount, // 用户帐号
            Win32_Process, // 系统进程
            Win32_Thread, // 系统线程
            Win32_Share, // 共享
            Win32_NetworkClient, // 已安装的网络客户端
            Win32_NetworkProtocol, // 已安装的网络协议
            Win32_PnPEntity,//all device
        }

        private void UpdParams()
        {
            uint cl_mode = Convert.ToUInt32(comboBoxDSPmode.SelectedValue);
            uint DSP_id = (uint)DSP_id_curr;
            int ratio = 10 * sample_rate / 1000;
            //override ch0 for stim control
            DeviceParams.SetSysParams(0, 1000, (uint)(numericUpDown_TgInt.Value * ratio),
                   (uint)(numericUpDown_TgDly.Value * ratio), (uint)(numericUpDown_TgRndDly.Value * ratio),
                   (uint)(numericUpDown_TgPW.Value * ratio), (uint)numericUpDown_TgCyc.Value, DeviceParams.trigger_gain[0],
                   (uint)numericUpDown_CLSt.Value, (uint)numericUpDown_CLEd.Value, cl_mode, Convert.ToUInt16(checkBox2.Checked), (uint)numericUpDown_randTrigMin.Value, (uint)numericUpDown_randTrigMax.Value);

            DeviceParams.SetSysParams(DSP_id, 1000, (uint)(numericUpDown_TgInt.Value * ratio),
                    (uint)(numericUpDown_TgDly.Value * ratio), (uint)(numericUpDown_TgRndDly.Value * ratio),
                    (uint)(numericUpDown_TgPW.Value * ratio), (uint)numericUpDown_TgCyc.Value, (float)numericUpDown_TgGain.Value,
                    (uint)numericUpDown_CLSt.Value, (uint)numericUpDown_CLEd.Value, cl_mode, Convert.ToUInt16(checkBox2.Checked), (uint)numericUpDown_randTrigMin.Value, (uint)numericUpDown_randTrigMax.Value);
            DeviceParams.SetDspParams(DSP_id,(uint)comboBox_CL_Arb.SelectedIndex,(uint)comboBox_CL_FilterType.SelectedIndex, (uint) numericUpDown_CL_MAOrd.Value);
        }

        private void LoadParams()
        {
            uint DSP_id = (uint)DSP_id_curr;
            uint Stim_id = 0;
            int ratio = 10 * sample_rate / 1000;
            int cl_id = (int)DeviceParams.cl_mode;
            comboBoxDSPmode.SelectedIndex = cl_id > 1 ? cl_id - 1 : cl_id;
            numericUpDown_TgInt.Value = DeviceParams.stim_interval[Stim_id] / ratio;
            numericUpDown_TgDly.Value = DeviceParams.stim_delay[Stim_id] / ratio;
            numericUpDown_TgRndDly.Value = DeviceParams.stim_RndDelay[Stim_id] / ratio;
            numericUpDown_TgPW.Value = DeviceParams.pulse_width[Stim_id] / ratio;
            numericUpDown_TgCyc.Value = DeviceParams.pulse_cyc[Stim_id];
            numericUpDown_TgGain.Value = (decimal)DeviceParams.trigger_gain[DSP_id];
            comboBox_CL_Arb.SelectedIndex = (int)DeviceParams.formula[DSP_id];
            comboBox_CL_FilterType.SelectedIndex = (int)DeviceParams.func[DSP_id];
            numericUpDown_CL_MAOrd.Value = (decimal)DeviceParams.MAOrd[DSP_id];
        }
        private void SetGain(float g)
        {
            byte[] src=BitConverter.GetBytes(g);
            //Array.Reverse(src);
            byte[] wrBuf = new byte[8];
            wrBuf[0] = 0x3c;
            wrBuf[1] = 0x10;
            wrBuf[2] = (byte)numericUpDown_DSPCH.Value;
            wrBuf[7] = 0x3e;
            Buffer.BlockCopy(src, 0, wrBuf, 3, 4);
            if (serialPort1.IsOpen)
            {
                serialPort1.Write(wrBuf, 0, wrBuf.Length);
            }
        }

        private void SetGainAbs(float g)
        {
            byte[] src = BitConverter.GetBytes(g);
            //Array.Reverse(src);
            byte[] wrBuf = new byte[8];
            wrBuf[0] = 0x3c;
            wrBuf[1] = 0x14;
            wrBuf[2] = (byte)numericUpDown_DSPCH.Value;
            wrBuf[7] = 0x3e;
            Buffer.BlockCopy(src, 0, wrBuf, 3, 4);
            if (serialPort1.IsOpen)
            {
                serialPort1.Write(wrBuf, 0, wrBuf.Length);
            }
        }
        private void SetStim(bool on)
        {
            byte[] src = BitConverter.GetBytes(on);
            //Array.Reverse(src);
            byte[] wrBuf = new byte[4];
            wrBuf[0] = 0x3c;
            wrBuf[1] = 0x11;
            wrBuf[3] = 0x3e;
            Buffer.BlockCopy(src, 0, wrBuf, 2, 1);
            if (serialPort1.IsOpen)
            {
                serialPort1.Write(wrBuf, 0, 4);
            }
        }

        private void SetStimParam()
        {
            //delay,rnd_delay,duration,interval int32
            UpdParams();
            for (uint DSP_id = 0; DSP_id < DSP_count; DSP_id++)
            {
                UInt32 delay, rnd_delay, duration, interval, cycle;
                delay = DeviceParams.stim_delay[DSP_id];
                rnd_delay = DeviceParams.stim_RndDelay[DSP_id];
                duration = DeviceParams.pulse_width[DSP_id];
                interval = DeviceParams.stim_interval[DSP_id];
                cycle = DeviceParams.pulse_cyc[DSP_id];
                UInt32[] src = new UInt32[5] { delay, rnd_delay, duration, interval, cycle };
                byte id = (byte)numericUpDown_DSPCH.Value;
                byte[] wrBuf = new byte[24];
                wrBuf[0] = 0x3c;
                wrBuf[1] = 0x20;
                wrBuf[2] = (byte)DSP_id;
                wrBuf[23] = 0x3e;
                Buffer.BlockCopy(src, 0, wrBuf, 3, 20);
                if (serialPort1.IsOpen)
                {
                    serialPort1.Write(wrBuf, 0, 24);
                }
                Thread.Sleep(50);
            }
        }

        private void SetDSPParam()
        {
            UpdParams();
            for (uint DSP_id = 0; DSP_id < DSP_count; DSP_id++)
            {
                UInt32 MA_ord, Filter_type, Formula;
                MA_ord = (UInt32)DeviceParams.MAOrd[DSP_id];
                Filter_type = (UInt32)DeviceParams.func[DSP_id];
                Formula = (UInt32)DeviceParams.formula[DSP_id];
                UInt32[] src = new UInt32[3] { MA_ord, Filter_type, Formula };
                if (serialPort1.IsOpen)
                {
                    byte[] wrBuf = new byte[16];
                    wrBuf[0] = 0x3c;
                    wrBuf[1] = 0x21;
                    wrBuf[2] = (byte)DSP_id;
                    wrBuf[15] = 0x3e;
                    Buffer.BlockCopy(src, 0, wrBuf, 3, 12);
                    serialPort1.Write(wrBuf, 0, 16);
                    Thread.Sleep(50);
                }
            }
        }

        private void SendSettings()
        {
            UpdParams();
            SendSYSSettings();
            SendCustomFilter(0);
            SendCustomFilter(1);
            SendDSPSettings();
            SetDSPParam();
            SetStimParam();
            SetDACGain();
            SetTrigMode();
        }
        private void SendSYSSettings()
        {
            byte[] tmp;
            tmp = DeviceParams.SaveSysParam();
            if (serialPort1.IsOpen)
            {
                byte[] wrBuf = new byte[515];
                wrBuf[0] = 0x3c;
                wrBuf[1] = 0x01;
                wrBuf[514] = 0x3e;
                Buffer.BlockCopy(tmp, 0, wrBuf, 2, 512);
                serialPort1.Write(wrBuf, 0, wrBuf.Length);
            }
        }
        private void SendDSPSettings()
        {
            byte[] tmp;
            tmp = DeviceParams.SaveDSPParam(0);
            if (serialPort1.IsOpen)
            {
                byte[] wrBuf = new byte[515];
                wrBuf[0] = 0x3c;
                wrBuf[1] = 0x02;
                wrBuf[514] = 0x3e;
                Buffer.BlockCopy(tmp, 0, wrBuf, 2, 512);
                serialPort1.Write(wrBuf, 0, wrBuf.Length);
            }

        }
        void SetDACGain()
        {
            if (serialPort1.IsOpen)
            {
                byte[] cmd = { 0x3c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x3e };
                UInt32[] v = { (UInt32)numericUpDown_DACgain.Value * 10 };
                Buffer.BlockCopy(v, 0, cmd, 2, 4);
                serialPort1.Write(cmd, 0, cmd.Length);
            }
        }

        void SetTrigMode()
        {
            if (serialPort1.IsOpen)
            {
                byte[] cmd = { 0x3c, 0x12, 0x00, 0x3e };
                Char[] v = { (Char)comboBox_TrigMode.SelectedIndex};
                Buffer.BlockCopy(v, 0, cmd, 2, 1);
                serialPort1.Write(cmd, 0, cmd.Length);
            }
        }
        private void splitContainer1_Panel2_Paint(object sender, PaintEventArgs e)
        {

        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            const int trans_ch = 8;
            const int package_size= 256;
            int cnt = 0;
            int buf_size = package_size * 16;
            int[] threshs =  new int[2];
            int thresh = 0;
            byte[] rdBuf = new byte[buf_size];
            byte[] rdTemp = new byte[1];
            if (serialPort1.IsOpen)
            {
                while (serialPort1.BytesToRead > package_size + 3) //+3 for head and tail
                {
                    int temp = 0;
                    while (temp != 0x3C)
                    {

                        if (serialPort1.IsOpen == false)
                        {
                            return;
                        }
                        else
                        {
                            if (serialPort1.BytesToRead == 0)
                            {
                                return;
                            }
                            temp = serialPort1.ReadByte();
                        }
                    }

                    temp = 0;
                    while (temp != 0xAD)
                    {

                        if (serialPort1.IsOpen == false)
                        {
                            return;
                        }
                        else
                        {
                            if (serialPort1.BytesToRead == 0)
                            {
                                return;
                            }
                            temp = serialPort1.ReadByte();
                        }
                    }
                    for (int tidx = 0; tidx<2; tidx++)
                    { 
                        byte[] infoBuf = new byte[2];
                        serialPort1.Read(infoBuf, 0, infoBuf.Length);
                        threshs[tidx] = BitConverter.ToInt16(infoBuf, 0);
                    }
                    thresh = threshs[(int)numericUpDown_DSPCH.Value];
                    int x = serialPort1.Read(rdBuf, cnt, package_size);
                    serialPort1.Read(rdTemp, 0, 1); //Discard the 0x3e trail
                    cnt += package_size;
                    if(cnt+package_size> buf_size)
                    {
                        break;
                    }
                   
 
                }
                if(cnt > 0)
                {
                    if (checkBox_LogFile.Checked)
                    {
                        strWR.Write(rdBuf, 0, cnt); //logging to file
                    }

                    int data_cnt = cnt / 2 / trans_ch;
                    double[,] DTP = new double[4, data_cnt];
                    
                    double gain = (double)comboBoxDisplayGain.SelectedValue;
                    double DSPgain = (double)comboBoxDSPGain.SelectedValue;
                    int DSP_ch = (int)numericUpDown_DSPCH.Value +4 ;
                    int[] disp_list = { 0, 1, DSP_ch, };
                    for (int i = 0; i < 2; i++)
                    {
                        for (int ii = 0; ii < data_cnt; ii++)
                        {
                            double tmp;
                            tmp = (double)(BitConverter.ToInt16(rdBuf, 2 * disp_list[i] + ii * 2 * trans_ch)) * gain;
                            if (cbRemoveDC.Checked)
                            {
                                tmp = filters[i].proc_IIR(tmp);
                            }

                            //if (tmp < -1) tmp = -1;
                            //if (tmp > 1) tmp = 1;
                            DTP[i, ii] = tmp;
                        }
                    }

                    for (int ii = 0; ii < data_cnt; ii++)
                    {
                        double tmp;
                        tmp = (double)(BitConverter.ToInt16(rdBuf, 2 * disp_list[2] + ii * 2 * trans_ch)) * DSPgain;

                        //if (tmp < -1) tmp = -1;
                        //if (tmp > 1) tmp = 1;
                        DTP[2, ii] = tmp- DSP_offset;
                    }

                    for (int ii = 0; ii < data_cnt; ii++)
                    {
                        int tmp;
                        
                        tmp = (BitConverter.ToUInt16(rdBuf, 2 * 6 + ii * 2 * trans_ch));
                        
                        tmp = tmp & (int)dout_type; //Take the second bit out.

                        double tmp1 = tmp * 1.8 - 0.9; //set to the middle of the screen
                        if (tmp1 < -1) tmp1 = -1;
                        if (tmp1 > 1) tmp1 = 1;
                        DTP[3, ii] = tmp1;
                    }
                    Rawdata.FetchData(DTP, data_cnt); //Draw curve
                    DrawCurve();
                    label_AbsThresh.Text = thresh.ToString();
                    List<double> num = new List<double> { 0, 0, (double)(thresh* (double)numericUpDown_DACgain.Value * DSPgain-DSP_offset), 0 }; //*10 is the output gain when converting float to uint16
                    this.Threshs = num;
                }
            }
        }

        private void checkBoxSetThresh_CheckedChanged(object sender, EventArgs e)
        {

        }

       
        private void buttonSetThresh_Click(object sender, EventArgs e)
        {

        }

        private void label29_Click(object sender, EventArgs e)
        {

        }

        private void label12_Click(object sender, EventArgs e)
        {

        }

        private void comboBoxPort_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void comboBoxPort_MouseClick(object sender, MouseEventArgs e)
        {
            int comport = GetComNum();
            comboBoxPort.Items.Clear();
            comboBoxPort.Items.Add("com" + comport.ToString());
        }

        private void numericUpDown_TgInt_ValueChanged(object sender, EventArgs e)
        {
            if (numericUpDown_TgPW.Value > numericUpDown_TgInt.Value)
            {
                numericUpDown_TgPW.Value = numericUpDown_TgInt.Value;
            }
            SetStimParam();
        }

        private void button_sendparams_Click(object sender, EventArgs e)
        {
            SendSettings();
        }

        private void numericUpDown_TgGain_ValueChanged(object sender, EventArgs e)
        {
            SetGain((float)numericUpDown_TgGain.Value);
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            SetStim(checkBox2.Checked);
        }

        private void numericUpDown_TgPW_ValueChanged(object sender, EventArgs e)
        {
            if(numericUpDown_TgPW.Value> numericUpDown_TgInt.Value)
            {
                numericUpDown_TgInt.Value = numericUpDown_TgPW.Value;
            }
            SetStimParam();
        }

        private void numericUpDown_TgDly_ValueChanged(object sender, EventArgs e)
        {
            SetStimParam();
        }

        private void numericUpDown_TgRndDly_ValueChanged(object sender, EventArgs e)
        {
            SetStimParam();
        }

        private void numericUpDown_CL_MAOrd_ValueChanged(object sender, EventArgs e)
        {
            //SetDSPParam();
        }

        private void comboBox_CL_FilterType_SelectedIndexChanged(object sender, EventArgs e)
        {
            int[] freq= new int[] { 4, 8, 13, 30, 80, 110, 250, 60, 200, 40 ,100};
            decimal v= (decimal)(int)(1.0 / freq[comboBox_CL_FilterType.SelectedIndex] * sample_rate * 3);
            v = v > numericUpDown_CL_MAOrd.Maximum ? numericUpDown_CL_MAOrd.Maximum : v;
            numericUpDown_CL_MAOrd.Value = v;

            if (comboBox_CL_FilterType.SelectedIndex == (comboBox_CL_FilterType.Items.Count-custom_filter_loaded.Length))
            {
                if (custom_filter_loaded[0]==false)
                {
                    load_CustomFilter(0);
                }
                if (custom_filter_loaded[0])
                {
                    filter_ID = comboBox_CL_FilterType.SelectedIndex;
                }
                else
                {
                    comboBox_CL_FilterType.SelectedIndex = filter_ID; //restore to previous selection if custom filter is not loaded
                }
            }
            else
            {
                if (comboBox_CL_FilterType.SelectedIndex == (comboBox_CL_FilterType.Items.Count - custom_filter_loaded.Length+1))
                {
                    if (custom_filter_loaded[1] == false)
                    {
                        load_CustomFilter(1);
                    }
                    if (custom_filter_loaded[1])
                    {
                        filter_ID = comboBox_CL_FilterType.SelectedIndex;
                    }
                    else
                    {
                        comboBox_CL_FilterType.SelectedIndex = filter_ID; //restore to previous selection if custom filter is not loaded
                    }
                }
                else
                {
                    filter_ID = comboBox_CL_FilterType.SelectedIndex;
                }
            }         
            //SetDSPParam();
        }

        private void comboBox_CL_Arb_SelectedIndexChanged(object sender, EventArgs e)
        {
            //SetDSPParam();
        }

        private void labelPort_Click(object sender, EventArgs e)
        {

        }

        private void splitContainer1_SplitterMoved(object sender, SplitterEventArgs e)
        {

        }

        private void flowLayoutPanel1_Paint(object sender, PaintEventArgs e)
        {

        }

        private void splitContainer3_Panel1_Paint(object sender, PaintEventArgs e)
        {

        }

        private void checkBox_LogFile_CheckedChanged(object sender, EventArgs e)
        {
            //if(checkBox_LogFile.Checked)
            //{
            //    saveFileDialog1.ShowDialog();
            //    label_OutputFile.Text= saveFileDialog1.FileName;
            //}
        }

        private int load_CustomFilter(int customfilter_id)
        {
            openFileDialog1.ShowDialog();
            if(File.Exists(openFileDialog1.FileName))
            {
                filter_param = new Byte[512];
                filter_param = File.ReadAllBytes(openFileDialog1.FileName);
                labelCustomFilter.Text = openFileDialog1.FileName;
                custom_filter_loaded[customfilter_id] = true;
                comboBox_CL_FilterType.SelectedIndex = comboBox_CL_FilterType.Items.Count-custom_filter_loaded.Length+customfilter_id;
                SendCustomFilter(customfilter_id);
            }
            
            return 0;
        }

        private void SendCustomFilter(int customfilter_id)
        {
            //delay,rnd_delay,duration,interval int32
            byte[] wrBuf = new byte[516];
            wrBuf[0] = 0x3c;
            wrBuf[1] = 0x06;
            wrBuf[2] = (byte) customfilter_id;
            wrBuf[515] = 0x3e;
            if (filter_param != null)
            {
                if (filter_param.Length <= 512)
                {
                    Buffer.BlockCopy(filter_param, 0, wrBuf, 3, filter_param.Length);
                    if (serialPort1.IsOpen)
                    {
                        serialPort1.Write(wrBuf, 0, wrBuf.Length);
                    }
                }
            }
        }
        private void button1_Click_1(object sender, EventArgs e)
        {
            load_CustomFilter(0);
            
        }

        private void button2_Click(object sender, EventArgs e)
        {
            load_CustomFilter(1);
        }
        private void button_forceTrig_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                byte[] cmd = { 0x3c, 0x83, 0x00, 0x3e };
                serialPort1.Write(cmd, 0, cmd.Length);
            }
        }

        private void numericUpDown_TgCyc_ValueChanged(object sender, EventArgs e)
        {
            SetStimParam();
        }

        private void numericUpDownThresh_ValueChanged(object sender, EventArgs e)
        {
            SetGainAbs((float)numericUpDownThresh.Value);
        }

        private void label8_Click(object sender, EventArgs e)
        {

        }

        private void numericUpDown_DACgain_ValueChanged(object sender, EventArgs e)
        {
            SetDACGain();
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked)
                timer_trigger.Start();
            else
                timer_trigger.Stop();

        }

        private void label9_Click(object sender, EventArgs e)
        {

        }

        private void numericUpDown_randTrigMax_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown_randTrigMax.Value = ((int)numericUpDown_randTrigMax.Value * 100) / 100;
            if (numericUpDown_randTrigMin.Value > numericUpDown_randTrigMax.Value)
                numericUpDown_randTrigMin.Value = numericUpDown_randTrigMax.Value;
            //if (checkBox_clmode.Checked == true)
            //    SendSettings();
        }

        private void numericUpDown_randTrigMin_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown_randTrigMin.Value = ((int)numericUpDown_randTrigMin.Value * 100) / 100;
            if (numericUpDown_randTrigMin.Value > numericUpDown_randTrigMax.Value)
                numericUpDown_randTrigMax.Value = numericUpDown_randTrigMin.Value;
            //if (checkBox_clmode.Checked == true)
            //    SendSettings();
        }

        private void splitContainer2_SplitterMoved(object sender, SplitterEventArgs e)
        {
            
        }

        private void CL04_center_MainInterface_Load(object sender, EventArgs e)
        {
            Assembly assembly = Assembly.GetExecutingAssembly();
            FileVersionInfo fileVersionInfo = FileVersionInfo.GetVersionInfo(assembly.Location);
            string version = fileVersionInfo.ProductVersion;
            this.Text = "CL02 closed-loop system V" + version;

            var dout_signal_type = new BindingList<KeyValuePair<string,miscDigisig_t>>();

            dout_signal_type.Add(new KeyValuePair<string, miscDigisig_t>("Stimulator1(output)",miscDigisig_t.MISC_SIG_STIM1));
            dout_signal_type.Add(new KeyValuePair<string, miscDigisig_t>("Stimulator2(output)", miscDigisig_t.MISC_SIG_STIM2));
            dout_signal_type.Add(new KeyValuePair<string, miscDigisig_t>("Internal Trigger1",miscDigisig_t.MISC_SIG_TRIG1));
            dout_signal_type.Add(new KeyValuePair<string, miscDigisig_t>("Internal Trigger2", miscDigisig_t.MISC_SIG_TRIG2));
            comboBoxDoutType.DataSource = dout_signal_type;
            comboBoxDoutType.ValueMember = "Value";
            comboBoxDoutType.DisplayMember = "Key";
            comboBoxDoutType.SelectedIndex = 0;
        }

        private void checkBox_clmode_CheckedChanged(object sender, EventArgs e)
        {
            SendSettings();
        }

        private void labelDisplayGain_Click(object sender, EventArgs e)
        {

        }

        private void comboBoxDisplayGain_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void label18_Click(object sender, EventArgs e)
        {

        }

        private void comboBoxDoutType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!String.IsNullOrEmpty(comboBoxDoutType.Text))
            {
                dout_type = ((KeyValuePair<string, miscDigisig_t>)comboBoxDoutType.SelectedItem).Value;
            }
        }

        private void numericUpDown_DSPCH_ValueChanged(object sender, EventArgs e)
        {
            UpdParams(); //Save current setting
            DSP_id_curr = numericUpDown_DSPCH.Value;
            LoadParams();
        }

        private void comboBoxDSPmode_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void comboBox_TrigMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            SetTrigMode();
        }
    }
}
