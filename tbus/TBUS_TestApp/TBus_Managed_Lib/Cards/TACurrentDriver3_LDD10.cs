using System;
using TBus;

namespace TBus.Cards
{
	public class TACurrentDriver3_LDD10 : TBus_USB_Card
	{
		public TACurrentDriver3_LDD10(TBus_Stack stack)
			: base(stack, "LDD10")
		{ }

		public void SetCurrentBoth(double CurrentA, double CurrentB)
		{
			SetFloat("Current A", CurrentA);
			SetFloat("Current B", CurrentB);
			RegisterToBusAndExecute("Set Current Both");
		}

		public void SetLaserStatus(
			int SignalSourceA, int SignalSourceB, bool GlobalLaserEnable, bool LaserADisable, bool LaserBDisable)
		{
			SetInt("Signal Source A", SignalSourceA);
			SetInt("Signal Source B", SignalSourceB);
			SetBool("Global Laser Enable", GlobalLaserEnable);
			SetBool("Laser A Disable", LaserADisable);
			SetBool("Laser B Disable", LaserADisable);
			RegisterToBusAndExecute("Set Laser Status");
		}

		public void GetLaserCurrent(
			double ResistorA, double ResistorB, out double CurrentA, out double CurrentB, out bool TempSens, out double ReadTemp)
		{
			RegisterToBusExecuteAndBusToParameter("Get Laser Current");
			CurrentA = (GetFloat("Read_A") * 6.6) / ResistorA;
			CurrentB = (GetFloat("Read_B") * 6.6) / ResistorB;
			TempSens = GetBool("TempSens");
			ReadTemp = GetFloat("Read_Temp");
		}

		public double GetTemperature(double Voltage, out double Resistance)
		{
			double byrefResistance;
			double Temperature = VoltageToTemperature(1.0, Voltage, 1800.0, out byrefResistance);
			Resistance = byrefResistance;
			return Temperature;
		}
	}
}
