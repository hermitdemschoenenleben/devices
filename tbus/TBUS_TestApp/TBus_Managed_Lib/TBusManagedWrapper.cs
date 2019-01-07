using System.Text;

namespace TBus
{
	public static partial class TBusManaged
	{
		public static int LastErrorCode = 0;
		private const int BufferSize = 255;


		public static void OpenStack(string StackName, string StackFile)
		{
			int err = OpenStack(PStrFromStr(StackName), PStrFromStr(StackFile));
			CheckForError(err, "OpenStack");
		}

		public static void SetBoolParam(string Stack, string Card, string Parameter, bool Value)
		{
			int iValue = (Value) ? 1 : 0;
			int err = SetBoolParam(PStrFromStr(Stack), PStrFromStr(Card), PStrFromStr(Parameter), iValue);
			CheckForError(err, "SetBoolParam");
		}

		public static bool GetBoolParam(string Stack, string Card, string Parameter)
		{
			int iValue = 0;
			int err = GetBoolParam(PStrFromStr(Stack), PStrFromStr(Card), PStrFromStr(Parameter), ref iValue);
			CheckForError(err, "GetBoolParam");
			return (iValue == 0) ? false : true;
		}

		public static void SetIntParam(string Stack, string Card, string Parameter, int Value)
		{
			int err = SetIntParam(PStrFromStr(Stack), PStrFromStr(Card), PStrFromStr(Parameter), Value);
			CheckForError(err, "SetIntParam");
		}

		public static int GetIntParam(string Stack, string Card, string Parameter)
		{
			int byrefValue = 0;
			int err = GetIntParam(PStrFromStr(Stack), PStrFromStr(Card), PStrFromStr(Parameter), ref byrefValue);
			CheckForError(err, "GetIntParam");
			return byrefValue;
		}

		public static void SetFloatParam(string Stack, string Card, string Parameter, double Value)
		{
			int err = SetFloatParam(PStrFromStr(Stack), PStrFromStr(Card), PStrFromStr(Parameter), Value);
			CheckForError(err, "SetFloatParam");
		}

		public static double GetFloatParam(string Stack, string Card, string Parameter)
		{
			double byrefVAlue = 0.0;
			int err = GetFloatParam(PStrFromStr(Stack), PStrFromStr(Card), PStrFromStr(Parameter), ref byrefVAlue);
			CheckForError(err, "GetFloatParam");
			return byrefVAlue;
		}

		/*public static uint[] RegisterToBusPreCalc(string Stack, string Card, string Register, out int Size)
		{
			int dataSize = 0;
			int errorCode = GetBusDataSize(PStrFromStr(Stack), PStrFromStr(Card), PStrFromStr(Register), ref dataSize);
			Size = dataSize;
			return new uint[dataSize * 4];
        }*/
		public static void RegisterToBus(string Stack, string Card, string Register, out int Size, out uint[] BusData)
		{
			int dataSize = GetBusDataSize(Stack, Card, Register, "RegisterToBus.GetBusDataSize");
			uint[] iBusData = new uint[dataSize];
			int err = RegisterToBus(
				PStrFromStr(Stack),
				PStrFromStr(Card),
				PStrFromStr(Register),
				dataSize / 4,
				iBusData
				);
			Size = dataSize / 4;
			BusData = iBusData;
			CheckForError(err, "RegisterToBus");
		}

		public static void BusToParam(string Stack, string Card, string Register, int Size, uint[] BusData)
		{
			int err =  BusToParam(
				PStrFromStr(Stack),
				PStrFromStr(Card),
				PStrFromStr(Register),
				Size,
				BusData
				);
			CheckForError(err, "BusToParam");
		}

		public static void GetStackInterface(string Stack, out string InterfaceType, out string InterfaceID)
		{
			StringBuilder TypeBuffer = new StringBuilder(BufferSize);
			StringBuilder IDBuffer = new StringBuilder(BufferSize);
			int err = GetStackInterface(PStrFromStr(Stack), TypeBuffer, IDBuffer);
			CheckForError(err, "GetStackInterface");
			InterfaceType = StrFromPStr(ref TypeBuffer);
			InterfaceID = StrFromPStr(ref IDBuffer);
		}

		public static int GetBusDataSize(string Stack, string Card, string Register, string newRegion = "GetBusDataSize")
		{
			int byrefSize = 0;
			int err = GetBusDataSize(PStrFromStr(Stack), PStrFromStr(Card), PStrFromStr(Register), ref byrefSize);
			CheckForError(err, newRegion);
			return byrefSize * 4;
		}

		/// <summary>
		/// Gets the string of an errorcode
		/// </summary>
		/// <param name="errorCode">The errorcode to get the string for</param>
		/// <returns>The error-string for the given errorCode</returns>
		public static string GetErrorString(int errorCode)
		{
			if (errorCode == 0)
				return string.Empty;
			StringBuilder buffer = new StringBuilder(BufferSize);
			GetErrorString(errorCode, buffer);
			return StrFromPStr(ref buffer);
		}

		/// <summary>
		/// Gets the TBus version
		/// </summary>
		/// <returns>The TBus version string</returns>
		public static string GetTBusVersion()
		{
			StringBuilder buffer = new StringBuilder(BufferSize);
			GetTBusVersion(buffer);
			return StrFromPStr(ref buffer);
		}

		/// <summary>
		/// Closes the stack
		/// </summary>
		/// <param name="Stack">Name of the stack to close</param>
		/// <returns>errorcode</returns>
		public static int CloseStack(string Stack)
		{
			return CloseStack(PStrFromStr(Stack));
		}
    }
}
