using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace CL02_center
{
    class CE_core
    {
        public uint fs;
        public uint[] formula = new uint[] { 0, 0, 0, 0 };
        public uint[] func = new uint[] { 0, 0, 0, 0 };
        public List<uint> chOrd;
        public uint[] MAOrd = new uint[] { 0, 0, 0, 0 };
        public uint[] stim_interval = new uint[] { 0, 0, 0, 0 };
        public uint[] stim_delay = new uint[] { 0, 0, 0, 0 };
        public uint[] stim_RndDelay = new uint[] { 0, 0, 0, 0 };
        public uint[] pulse_width = new uint[] { 0, 0, 0, 0 };
        public uint[] pulse_cyc = new uint[] { 0, 0, 0, 0 };
        public float[] trigger_gain = new float[] { 0, 0, 0, 0 };
        public uint trigger_trainStart;
        public uint trigger_trainDuration;
        public uint cl_mode;
        public uint stim_mode;
        public uint[] stim_ch = new uint[] { 0, 0, 0, 0 };
        public uint[] stim_intensity = new uint[] { 0, 0, 0, 0 };    
        public uint preview_channel_bank;
        public uint LED_pulse_CNT;
        private uint randtrig_min;         //324
        private uint randtrig_max;         //328
        public CE_core(int ch_num){
            chOrd = new List<uint>(ch_num);
        }
        unsafe private struct CE32_IDEN
        {
            public fixed char IDEN_STRING[256];      //unassigned
        }
        unsafe public struct CE32_systemParam
        {
            public uint fs;                   //4 Sampling Rate 
            public uint AUX_mode;             //8 AUX pin mode, 0 = unused , 1=Timer-controlled, 2=Closed-loop
            public uint Nch;                  //12 Channel number(based on one chip), eg. max(RHD2164)=32
            public fixed ushort convCmd[64];   //140 convert commands			//64B
            public uint dispCH;               //144 Select channel for HighSpeed visualization
            public uint func1;                //148
            public uint func2;                //152
            public uint cmd_ch;                //156Convert command ptr	(current SPI out)
            public uint rec_ch;                //160 Record sample ptr		(current SPI in)
            public uint stim_mode;             //164
            public uint cl_mode;                //168
            public fixed uint stim_interval[4];   //184
            public fixed uint pulse_width[4];       //200
            public fixed uint pulse_cnt[4];         //216
            public fixed uint stim_delay[4];        //232
            public fixed uint stim_RndDelay[4];     //248
            public uint trigger_trainStart;         //252
            public uint trigger_trainDuration;      //256
            public fixed float trigger_gain[4];     //272
            public uint SD_capacity;                //276
            public uint LED_pulse_CNT;              //280
            public uint preview_channel_bank;       //284
            public uint system_status;              //288
            public fixed uint stim_intensity[4];    //304
            public fixed uint stim_ch[4];           //320
            public uint randtrig_min;         //324
            public uint randtrig_max;         //328
            public fixed uint unassigned[46];      //unassigned
        };

        unsafe private struct CE32_systemLog
        {
            public uint Nrec;                 //Number of records
            public fixed uint log[127];             //Log storage locations. Format as [Start1,End1,Start2,End2....] keep size as 512B 
        };

        unsafe private struct CE32_dspParam
        {
            public fixed byte chOrd[128];           //Channel order for calculation;
            public uint formula;              //Formula type					; 132B
            public uint func1;
            public uint func2;
            public uint MAOrd1;                          //Channel number for DSP	; 144B
            public fixed float filter1[46];          //Filter1 Param
            public fixed float filter2[46];          //Filter2 Param
        };

        public void SetSysParams(uint dsp_idx,uint sampling_rate,uint stimulation_interval,uint stimulation_delay,uint stimulation_randomDelay,uint stimulation_pulseWidth, uint stimulation_pulseCyc,
            float trigger_level,uint trigger_trainStartTime,uint trigger_trainLength,uint closedLoop_mode,uint stimulation_mode,uint rand_min,uint rand_max)
        {
            fs = sampling_rate;
            stim_interval[dsp_idx] = stimulation_interval;
            stim_delay[dsp_idx] = stimulation_randomDelay;
            stim_RndDelay[dsp_idx] = stimulation_randomDelay;
            pulse_width[dsp_idx] = stimulation_pulseWidth;
            pulse_cyc[dsp_idx] = stimulation_pulseCyc;
            trigger_gain[dsp_idx] = trigger_level;
            trigger_trainStart = trigger_trainStartTime;
            trigger_trainDuration = trigger_trainLength;
            cl_mode = closedLoop_mode;
            stim_mode = stimulation_mode;
            randtrig_max = rand_max;
            randtrig_min = rand_min;
        }

        public void SetDspParams(int dsp_idx, uint Formula, uint Func1, uint MA_Ord1)
        {
            formula[dsp_idx] = Formula;
            func[dsp_idx] = Func1;
            MAOrd[dsp_idx] = MA_Ord1;
        }
        unsafe public byte[] SaveSysParam()
        {
            CE32_systemParam sys = new CE32_systemParam();
            sys.fs = fs;
            for (int i = 0; i < 2; i++)
            {
                sys.stim_interval[i] = stim_interval[i];
                sys.stim_delay[i] = stim_delay[i];
                sys.stim_RndDelay[i] = stim_RndDelay[i];
                sys.pulse_width[i] = pulse_width[i];
                sys.trigger_gain[i] = trigger_gain[i];
                sys.pulse_cnt[i] = pulse_cyc[i];
                sys.stim_intensity[i] = stim_intensity[i];
                sys.stim_ch[i] = stim_ch[i];
            }
            sys.trigger_trainStart = trigger_trainStart;
            sys.trigger_trainDuration = trigger_trainDuration;
            sys.cl_mode = cl_mode;
            sys.stim_mode = stim_mode;
            sys.LED_pulse_CNT = LED_pulse_CNT;
            sys.preview_channel_bank = preview_channel_bank;
            sys.randtrig_max = randtrig_max;
            sys.randtrig_min = randtrig_min;
            byte[] Wbuf;
            Wbuf = StructToBytes(sys);
            return Wbuf;
        }

        unsafe public byte[] SaveDSPParam(int idx)
        {
            CE32_dspParam dsp = new CE32_dspParam();

            dsp.formula = formula[idx];
            dsp.func1 = func[idx];
            for (int i = 0; i < chOrd.Count; i++)
            {
                dsp.chOrd[i] = (byte)chOrd[i];
            }
            dsp.MAOrd1 = MAOrd[idx];

            byte[] Wbuf;
            Wbuf = StructToBytes(dsp);
            return Wbuf;
        }

        public byte[] StructToBytes(object obj)
        {
            int rawsize = Marshal.SizeOf(obj);

            IntPtr buffer = Marshal.AllocHGlobal(rawsize);
            Marshal.StructureToPtr(obj, buffer, false);
            byte[] rawdatas = new byte[rawsize];
            Marshal.Copy(buffer, rawdatas, 0, rawsize);
            Marshal.FreeHGlobal(buffer);
            return rawdatas;
        }
    }
}
