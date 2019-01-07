using System;
using TBus;

namespace TBus.Cards
{
	public class TempIn2 : TBus_USB_Card
	{
		public TempIn2(TBus_Stack stack)
			: base(stack, "TemperatureIn2")
		{ }

		public double GetSignal(int channel)
		{
			if (channel < 0 || channel > 24)
				throw new Exception("Channels under zero or over 24 are not supported");
			SetInt("Channel", channel);
			RegisterToBusExecuteAndBusToParameter("Get Signal");
			return GetFloat("Signal");
		}

		public double GetTemp(int channel)
		{
			double readout = GetSignal(channel);
			double calculatedResistanceTemp;
			return VoltageToTemperature(1.0, readout, 10000.0, out calculatedResistanceTemp);
		}
	}
}
