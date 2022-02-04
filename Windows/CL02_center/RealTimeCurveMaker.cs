using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

namespace CL02_center
{
    class RealTimeCurveMaker
    {
        private int DataFs;
        private int PanelWidth;
        private int DisplayLength;//单位是秒

        private int DisplayPointNum;
        private int DisplayPointGap = 0;//隔几个像素显示
        private int DataGap;//现在代表图片中一个点代表了原始数据中多长的数据

        private int RawDataCntPerPixel;//数据显示一个点在原始数据上代表多少个点,计算方式和datagap相等

        private int DataIdx;//本panel显示的曲线在datasource中的索引,通道数

        private List<Point> LastPoints;

        public RealTimeCurveMaker(int datafs, int panelwidth, int displaylength, int idx)
        {
            this.DataFs = datafs;
            this.PanelWidth = panelwidth;
            this.DisplayLength = displaylength;
            this.DataIdx = idx;
            CalDisplayPointNum();
        }

        private void CalDisplayPointNum()
        {
            int data_count = DataFs * DisplayLength;
            this.DisplayPointNum = Convert.ToInt32( Math.Floor((double)(PanelWidth+ DisplayPointGap) / (DisplayPointGap+1)));//公式   n+2(n-1)<all
            this.DataGap = data_count / DisplayPointNum;
            
        }

        private int ConvertRawData2Yval(int height, double val)
        {
            int zero = height / 2;
            return (int)(zero - zero * val);
        }
        /// <summary>
        /// 画阈值曲线
        /// </summary>
        /// <param name="g"></param>
        /// <param name="val"></param>
        /// <param name="height"></param>
        /// <param name="width"></param>
        private void PaintThreshs(ref Graphics g, List<double> val, int height, int width)
        {
            int y = ConvertRawData2Yval(height, val[DataIdx]);
            int xbegin = 1;
            int xend = width - 1;
            Point begin = new Point(xbegin, y);
            Point end = new Point(xend, y);
            g.DrawLine(new Pen(Color.Red), begin, end);
        }

        private void FindMaxMin(ref double max, ref double min, ref double mean,DataSource data, int begin, int end)
        {
            max = data.Buffer[DataIdx][begin];
            min = data.Buffer[DataIdx][begin];
            double temp = 0;
            end = end >= data.Datalen ? data.Datalen : end;
            for (int i = begin; i < end; ++i)
            {
                if (max < data.Buffer[DataIdx][i])
                {
                    max = data.Buffer[DataIdx][i];
                    continue;
                }
                if (min > data.Buffer[DataIdx][i])
                {
                    min = data.Buffer[DataIdx][i];
                    continue;
                }
                temp += data.Buffer[DataIdx][i];
            }
            mean=temp/(end - begin+1);
        }

        public Image GetCurveImage(DataSource data, int startpos, int panelwidth, int displaylength, int height, int width, PaintInfo paintinfo)
        {
            lock (this)
            {
                if (this.PanelWidth != panelwidth || this.DisplayLength != displaylength)
                {
                    this.PanelWidth = panelwidth;
                    this.DisplayLength = displaylength;
                    CalDisplayPointNum();
                }

                CurveModel curve1 = new CurveModel();

                Image currentimage = new Bitmap(width, height);

                Graphics g = Graphics.FromImage(currentimage);
                g.Clear(Color.White);
                int cnt = 0;
                int overridearea = 0;
                int splitpoint = 0;
                if (startpos != 0)
                {
                    overridearea = (data.thisTimeDataLen) % (displaylength * DataFs);
                    splitpoint = data.thisTimeDataLen - overridearea;
                    double max = 0;
                    double min = 0;
                    double mean = 0;
                    int pixelx = 0;
                    int step = DisplayPointGap + 1;
                    for (var i = splitpoint; i <= data.thisTimeDataLen; i += this.DataGap)
                    {
                        FindMaxMin(ref max, ref min, ref mean,data, i, i + this.DataGap-1);
                        //curve1.yvalues.Add(ConvertRawData2Yval(height, data.Buffer[DataIdx][i]));
                        //curve1.xvalues.Add(pixelx);
                        curve1.yvalues.Add(ConvertRawData2Yval(height, max));
                        curve1.yvalues1.Add(ConvertRawData2Yval(height, min));
                        curve1.xvalues.Add(pixelx);
                        pixelx += step;
                        ++cnt;
                    }
                    if(cnt< DisplayPointNum)
                    {
                        int thresh = pixelx - step;
                        for (var i = 0; i < LastPoints.Count; ++i)
                        {
                            if (LastPoints[i].X > thresh)
                            {
                                curve1.yvalues.Add(LastPoints[i].Y);
                                curve1.yvalues1.Add(LastPoints[i].Y);
                                curve1.xvalues.Add(LastPoints[i].X);
                            }
                        }
                    }
                }
                else
                {
                    double max = 0;
                    double min = 0;
                    double mean = 0;
                    int i = 0;
                    int pixelx = 0;
                    int step = DisplayPointGap + 1;
                    for (i = startpos; i < data.thisTimeDataLen; i += this.DataGap)
                    {
                        FindMaxMin(ref max, ref min, ref mean, data, i, i + this.DataGap-1);
                        //curve1.yvalues.Add(ConvertRawData2Yval(height, data.Buffer[DataIdx][i]));
                        //curve1.xvalues.Add(pixelx);
                        curve1.yvalues.Add(ConvertRawData2Yval(height, max));
                        curve1.yvalues1.Add(ConvertRawData2Yval(height, min));
                        curve1.xvalues.Add(pixelx);
                        pixelx += step;
                        ++cnt;
                    }
                    ////显示最后一小块不够一个datagap的点
                    //FindMaxMin(ref max, ref min, data, i-this.DataGap, data.thisTimeDataLen);
                    //curve1.yvalues.Add(ConvertRawData2Yval(height, max));
                    //curve1.xvalues.Add(cnt * DisplayPointGap);
                    //curve1.yvalues.Add(ConvertRawData2Yval(height, min));
                    //curve1.xvalues.Add(cnt * DisplayPointGap);
                }
                
                List<Point> points = new List<Point>();
                List<Point> points1 = new List<Point>();

                for (int i = 1; i < curve1.yvalues.Count; i++) //Connecting lines at different X
                {
                    if (curve1.yvalues1[i - 1] < curve1.yvalues[i]) // if max[i-1] < min [i], connect them
                    {
                        Point pt_0 = new Point(curve1.xvalues[i - 1], curve1.yvalues1[i - 1]);
                        Point pt_1 = new Point(curve1.xvalues[i], curve1.yvalues[i]);
                        g.DrawLine(new Pen(Color.Blue), pt_0, pt_1);
                    }
                    else
                    {
                        if (curve1.yvalues[i - 1] > curve1.yvalues1[i]) // if min[i-1] > max [i], connect them
                        {
                            Point pt_0 = new Point(curve1.xvalues[i - 1], curve1.yvalues[i - 1]);
                            Point pt_1 = new Point(curve1.xvalues[i], curve1.yvalues1[i]);
                            g.DrawLine(new Pen(Color.Blue), pt_0, pt_1);
                        }

                    }
                    if (curve1.yvalues[i] == curve1.yvalues1[i])
                    {
                        Point pt = new Point(curve1.xvalues[i-1], curve1.yvalues[i-1]);
                        Point pt1 = new Point(curve1.xvalues[i], curve1.yvalues[i]);
                        g.DrawLine(new Pen(Color.Blue), pt, pt1);
                    }
                    else
                    {
                        Point pt = new Point(curve1.xvalues[i], curve1.yvalues[i]);
                        Point pt1 = new Point(curve1.xvalues[i], curve1.yvalues1[i]);
                        g.DrawLine(new Pen(Color.Blue), pt, pt1);
                    }

                    //points.Add(pt);
                    //points.Add(pt1);
                }
                this.LastPoints = points;
                //if (points.Count > 1)
                //{
                //    //Point[] curvePoints = points.ToArray();
                //    //g.FillPolygon(new SolidBrush(Color.Blue), curvePoints);
                //    //g.DrawLines(new Pen(Color.Blue), points.ToArray());
                //    //g.DrawLines(new Pen(Color.Blue), points1.ToArray());
                //}
                if (paintinfo.PaintThreshFlag == true && paintinfo.Threshs != null)
                {
                    PaintThreshs(ref g, paintinfo.Threshs, height, width);
                }
                g.Dispose();
                return currentimage;
            }

        }

        public int GetNewestStartPos(DataSource data, int displaylength)
        {
            int startpos = 0;
            if (data.thisTimeDataLen >= DataFs * displaylength)
            {
                startpos = data.thisTimeDataLen - DataFs * displaylength + 1;
            }
            return startpos;
        }

    }


    class CurveModel
    {
        public List<int> yvalues = new List<int>();
        public List<int> yvalues1 = new List<int>();
        public List<int> xvalues = new List<int>();
    }

    class PaintInfo
    {
        public bool PaintThreshFlag = false;
        public List<double> Threshs = null;
    }
}
