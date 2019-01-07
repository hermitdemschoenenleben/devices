using System;

namespace TBus
{
	public class TBus_USB_Card
	{
		public string Stack = string.Empty;
		public string Card = string.Empty;

		public int USBHandle = 0;

		public TBus_USB_Card(TBus_Stack stack, string CardName)
		{
			Stack = stack.Stack;
			USBHandle = stack.USBHandle;
			Card = CardName;
		}

		public void SetBool(string Parameter, bool Value)
		{
			TBusManaged.SetBoolParam(Stack, Card, Parameter, Value);
		}

		public void SetInt(string Parameter, int Value)
		{
			TBusManaged.SetIntParam(Stack, Card, Parameter, Value);
		}

		public void SetFloat(string Parameter, double Value)
		{
			TBusManaged.SetFloatParam(Stack, Card, Parameter, Value);
		}

		public bool GetBool(string Parameter)
		{
			return TBusManaged.GetBoolParam(Stack, Card, Parameter);
		}

		public int GetInt(string Parameter)
		{
			return TBusManaged.GetIntParam(Stack, Card, Parameter);
		}

		public double GetFloat(string Parameter)
		{
			return TBusManaged.GetFloatParam(Stack, Card, Parameter);
		}

		/// <summary>
		/// Executes RegisterToBus
		/// </summary>
		/// <param name="Register">The Register to send</param>
		/// <param name="BusData">out: The BusData to execute</param>
		/// <returns>The bus size</returns>
		public int RegisterToBus(string Register, out uint[] BusData)
		{
			int BusSize;
			uint[] _BusData;
			TBusManaged.RegisterToBus(Stack, Card, Register, out BusSize, out _BusData);
			BusData = _BusData;
			return BusSize;
		}

		public void Execute(int BusSize, uint[] BusData)
		{
			TBus_USBManaged.ExecuteBus_USB(USBHandle, BusSize, ref BusData);
		}

		public void RegisterToBusAndExecute(string Register)
		{
			int BusSize;
			uint[] BusData;
			TBusManaged.RegisterToBus(Stack, Card, Register, out BusSize, out BusData);
			TBus_USBManaged.ExecuteBus_USB(USBHandle, BusSize, ref BusData);
		}

		public void RegisterToBusExecuteAndBusToParameter(string Register)
		{
			int BusSize;
			uint[] BusData;
			TBusManaged.RegisterToBus(Stack, Card, Register, out BusSize, out BusData);
			TBus_USBManaged.ExecuteBus_USB(USBHandle, BusSize, ref BusData);
			TBusManaged.BusToParam(Stack, Card, Register, BusSize, BusData);
		}

		public void BusToParam(string Register, int BusSize, uint[] BusData)
		{
			TBusManaged.BusToParam(Stack, Card, Register, BusSize, BusData);
		}

		public int GetBusDataSize(string Register)
		{
			return TBusManaged.GetBusDataSize(Stack, Card, Register);
		}

		public void GetStackInterface(out string Type, out string ID)
		{
			string InterfaceType, InterfaceID;
			TBusManaged.GetStackInterface(Stack, out InterfaceType, out InterfaceID);
			Type = InterfaceType;
			ID = InterfaceID;
		}

		public double VoltageToTemperature(double SensorSupply, double Voltage, double ResistanceRef, out double Resistance)
		{
			const double B = 3650.0, T0 = 273.15, TR = 25.0;

			Resistance = (ResistanceRef * Voltage) / (SensorSupply - Voltage);

			double Te = -B / (TR + T0);
			double TDs = Resistance / (ResistanceRef * Math.Exp(Te));

			return B / Math.Log(TDs) - T0;
		}
	}
}
