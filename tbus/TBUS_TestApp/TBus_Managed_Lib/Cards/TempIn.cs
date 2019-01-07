using System;
using TBus;

namespace TBus.Cards
{
	public class TempIn : TBus_USB_Card
	{
		public TempIn(TBus_Stack stack)
			: base(stack, "TempIn1")
		{ }

		public double GetADCResult(int channel)
		{
			if (channel < 0 || channel > 24)
				throw new Exception("Channels under zero or over 24 are not supported");
			SetInt("Address", channel);
			RegisterToBusExecuteAndBusToParameter("Get ADC result");
			return GetFloat("Data");
		}

		public double GetTemp(int channel)
		{
			double readout = GetADCResult(channel);
			double calculatedResistanceTemp;
			return VoltageToTemperature(1.0, readout, 10000.0, out calculatedResistanceTemp);
		}
    }
}
