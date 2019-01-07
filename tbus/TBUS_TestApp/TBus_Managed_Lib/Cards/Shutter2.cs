using System;
using TBus;

namespace TBus.Cards
{
	public class Shutter2 : TBus_USB_Card
	{
		public Shutter2(TBus_Stack stack)
			: base(stack, "Shutter2")
		{ }

		public void SetMemoryByte(int Mode, int Address = 0, int Data = 0xFF)
		{
			SetInt("Mode", Mode);
			SetInt("Address", Address);
			SetInt("Data", Data);
			RegisterToBusAndExecute("SetMemoryByte");
		}

		public void SetMemoryByte(int Mode, int Address, bool[] Data)
		{
			int dataInt = 0;
			for (int i = 0; i < 8; ++i)
			{
				if (Data[i])
					dataInt |= (1 << i);
			}
            SetMemoryByte(Mode, Address, dataInt);
		}

		public void SetMode(int Mode)
		{
			SetInt("Mode", Mode);
			RegisterToBusAndExecute("SetMode");
		}

		public void SetSignalSource(int Mode, int ClockSource = 0, int ResetSource = 1)
		{
			SetInt("Mode", Mode);
			SetInt("ClockSource", ClockSource);
			SetInt("ResetSource", ResetSource);
			RegisterToBusAndExecute("SetSignalSource");
		}
    }
}
