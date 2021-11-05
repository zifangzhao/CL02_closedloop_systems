using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CL02_center
{
    class CE32_DSP
    {
        
    }

    class CE32_filter
    {
        double[] filter_num;
        double[] filter_den;
        double[] state;
        int FIL_ord;
        int ptr;
        double output;
        double last_input;

        public CE32_filter(double[] num,double[] den)
        {
            this.filter_num = num;
            this.filter_den = den;
            FIL_ord = num.Length;
            this.state = new double[FIL_ord * 2];
        }
        public double proc_IIR(double input)
        {
            double fil_temp = filter_num[0] * input;            //b(1)*x(n)	
            int temp_ptr = ptr;
            for (int i = 1; i < FIL_ord; i++)
            {
                fil_temp += filter_num[i] * state[2 * temp_ptr] - filter_den[i] * state[2 * temp_ptr + 1];
                temp_ptr--;
                if (temp_ptr < 0)
                {
                    temp_ptr += FIL_ord;
                }
            }
            ptr = ptr + 1;                                              //increase ptr to store new filter state
            if (ptr >= FIL_ord)
            {
                ptr = 0;
            }
            state[2 * ptr] = input;
            state[2 * ptr + 1] = fil_temp;
            output = fil_temp;
            last_input = input;
            return fil_temp;
        }
    }
}
