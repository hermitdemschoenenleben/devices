using System;
using System.Text;

namespace TBus
{
	public static partial class TBus_USBManaged
	{
		public static int Open_USB(string USB_SerialNumber)
		{
			int handle = OpenUSB(TBusManaged.PStrFromStr(USB_SerialNumber));
			if (handle == 0)
				throw new Exception($"TBus_USB Error (Open_USB); Hndl ({handle}); Msg (Opened handle was zero)");
			return handle;
		}

		public static void Close_USB(int USB_Handle)
		{
			int err = CloseUSB(USB_Handle);
			if (err != 0)
				throw new Exception($"TBus_USB Error (Close_USB); Hndl ({err}); Msg ()");
		}

		public static void CloseAll_USB()
		{
			int err = CloseAllUSB();
			if (err != 0)
				throw new Exception($"TBus_USB Error (CloseAll_USB); Hndl ({err}); Msg ()");
		}

		public static void ExecuteBus_USB(int USB_Handle, int DataLength, ref uint[] BusData)
		{
			int err = ExecuteBus(USB_Handle, DataLength, BusData);
			if (err != 0)
				throw new Exception($"TBus_USB Error (ExecuteBus_USB); Hndl ({err}); Msg ()");
		}
    }
}
