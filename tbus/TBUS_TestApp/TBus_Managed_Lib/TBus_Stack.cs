using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TBus
{
	public class TBus_Stack : IDisposable
	{
		private bool StackOpen = false;
		private bool USBOpen = false;

		public string Stack = string.Empty;
		public string StackFile = string.Empty;

		public int USBHandle = 0;

		public TBus_Stack(string StackName, string StackFileName)
		{
			Stack = StackName;
			StackFile = StackFileName;
			OpenStack();
			OpenUSB();
		}

		public void OpenStack()
		{
			if (StackOpen)
				return;
			TBusManaged.OpenStack(Stack, StackFile);
		}

		public void OpenUSB()
		{
			if (USBOpen)
				return;
			string InterfaceType, InterfaceID;
			TBusManaged.GetStackInterface(Stack, out InterfaceType, out InterfaceID);
			USBHandle = TBus_USBManaged.Open_USB(InterfaceID);
		}

		public void CloseUSB()
		{
			TBus_USBManaged.Close_USB(USBHandle);
		}

		public void CloseStack()
		{
			TBusManaged.CloseStack(Stack);
		}

		public void Dispose()
		{
			CloseUSB();
			CloseStack();
		}
	}
}
