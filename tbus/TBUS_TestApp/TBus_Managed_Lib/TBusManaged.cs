using System;
using System.Text;
using System.Runtime.InteropServices;

namespace TBus
{
    public static partial class TBusManaged
    {
		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int OpenStack(StringBuilder StackName, StringBuilder StackFile);


		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int GetStackInterface(StringBuilder Stack, StringBuilder InterfaceType, StringBuilder InterfaceID);


		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int SetBoolParam(StringBuilder Stack, StringBuilder Card, StringBuilder Parameter, int Value);

		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int GetBoolParam(StringBuilder Stack, StringBuilder Card, StringBuilder Parameter, ref int Value);


		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int SetIntParam(StringBuilder Stack, StringBuilder Card, StringBuilder Parameter, int Value);

		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int GetIntParam(StringBuilder Stack, StringBuilder Card, StringBuilder Parameter, ref int Value);


		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int SetFloatParam(StringBuilder Stack, StringBuilder Card, StringBuilder Parameter, double Value);

		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int GetFloatParam(StringBuilder Stack, StringBuilder Card, StringBuilder Parameter, ref double Value);


		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int RegisterToBus(StringBuilder Stack, StringBuilder Card, StringBuilder Register, int Size, uint[] BusData);

		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int BusToParam(StringBuilder Stack, StringBuilder Card, StringBuilder Register, int Size, uint[] BusData);

		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int GetBusDataSize(StringBuilder Stack, StringBuilder Card, StringBuilder Register, ref int Size);


		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern void GetErrorString(int ErrorCode, StringBuilder ErrorString);

		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern void GetTBusVersion(StringBuilder VersionString);

		[DllImport("tbus.dll", CallingConvention = CallingConvention.StdCall)]
		private static extern int CloseStack(StringBuilder Stack);

		public static StringBuilder PStrFromStr(string str)
		{
			if (str.Length > (BufferSize - 1))
				throw new System.ArgumentOutOfRangeException("str", $"str is bigger than {BufferSize - 1} characters");
			StringBuilder strB = new StringBuilder(BufferSize);
			strB.Insert(0, (char)str.Length);
			strB.Insert(1, str);
			return strB;
		}

		public static string StrFromPStr(ref StringBuilder strB)
		{
			int length = strB[0];
			if (length < 1)
				return string.Empty;
			else
				return strB.ToString().Substring(1, length);
		}

		public static void CheckForError(int errorCode, string region = "")
		{
			LastErrorCode = errorCode;
			if (errorCode == 0)
				return;
			string errorString;
			try
			{
				StringBuilder buffer = new StringBuilder(BufferSize);
				GetErrorString(errorCode, buffer);
				errorString = StrFromPStr(ref buffer);
			}
			catch (Exception e)
			{
				errorString = $"Error while catching Error-String [{e.Message}]";
            }
			throw new Exception($"TBus Error ({region}); Code ({errorCode}); Msg ({errorString})");
        }
	}
}
