using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CL02_center
{
    class DataSource
    {
        private List<double[]> buffer;
        

        private int channelnum;
        private int datalen;
        public int Channelnum
        {
            get
            {
                return channelnum;
            }
        }

        public int Datalen
        {
            get
            {
                return datalen;
            }
        }

        public List<double[]> Buffer
        {
            get
            {
                return buffer;
            }
        }

        //private int testdatalen = 2000000;//测试中，程序运行时间不要超过1000s

        public int thisTimeDataLen = -1;

        private bool overflow_flag = false;
        private int Max_Display_len;

        public int sampleRate = 1000;

        public DataSource(int channelnum , int max_displaylen, int buff_time_len, int sample_rate)//通道数，缓冲区长度，这里用的list，回来应该会改为定长数组
        {
            this.channelnum = channelnum;
            this.datalen = max_displaylen* buff_time_len;//buff_time_len是几倍于最大显示时长
            this.buffer = new List<double[]>();
            this.Max_Display_len = max_displaylen;
            this.sampleRate = sample_rate;
            for(int i=0;i<channelnum;++i)
            {
                double[] temp = new double[datalen];
                this.buffer.Add(temp);
            }
            //GenerateData();
        }
        /*for test*/
        //private void GenerateData()
        //{
        //    int limit = 1000;
        //    Random rd = new Random();
        //    for(int i=0;i<channelnum;++i)
        //    {
        //        for(int j=0;j<testdatalen;++j)
        //        {
        //            var temp = rd.Next(1, limit);
        //            var temp1 = limit / 2;
        //            this.data[i].Add(((double)temp - temp1) / temp1);//归一化到1  -1
        //        }
        //    }
        //}

        public void FetchData(double [,] data, int datalen)
        {
            //this.thisTimeDataLen += this.sampleRate / 10;//相当于每隔0.1s取数据了
            //if (this.thisTimeDataLen >= testdatalen)
            //{
            //    int i = 3;
            //    i += 3;//test
            //}
            if (thisTimeDataLen + datalen  <= this.datalen - 1)
            {
                int begin = thisTimeDataLen + 1;
                for (int i = 0; i < channelnum; ++i)
                {
                    for (int j = 0; j < datalen; ++j)
                    {
                        buffer[i][begin+j] = data[i, j];
                    }
                }
                thisTimeDataLen += datalen;
            }
            else
            {
                //full buffer
                //int commen_multiple = 10 * sampleRate;//各种显示时长的公倍数
                //int diff = thisTimeDataLen - commen_multiple;
                //for (int i =0;i<channelnum;++i)
                //{
                //    int cnt = 0;
                //    for(int j=thisTimeDataLen-diff+1;j<= thisTimeDataLen; ++j)
                //    {
                //        buffer[i][cnt++] = buffer[i][j];
                //    }
                //}
                //thisTimeDataLen = diff - 1;
                thisTimeDataLen = -1;
                int begin = thisTimeDataLen + 1;
                for (int i = 0; i < channelnum; ++i)
                {
                    for (int j = 0; j < datalen; ++j)
                    {
                        buffer[i][begin + j] = data[i, j];
                    }
                }
                thisTimeDataLen += datalen;
            }
        }
    }
}
