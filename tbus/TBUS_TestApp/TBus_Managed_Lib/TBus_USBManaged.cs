using System.Text;
using System.Runtime.InteropServices;

namespace TBus
{
	public static partial class TBus_USBManaged
	{
		[DllImport("tbus_usb.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int OpenUSB(StringBuilder USB_SerialNumber);

		[DllImport("tbus_usb.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int CloseUSB(int USB_Handle);

		[DllImport("tbus_usb.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int CloseAllUSB();

		[DllImport("tbus_usb.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int ExecuteBus(int USB_Handle, int DataLength, uint[] BusData);
	}
}
