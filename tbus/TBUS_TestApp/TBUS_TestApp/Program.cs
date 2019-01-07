using System.Threading;
using TBus;
using TBus.Cards;

using static System.Console;
using static System.Threading.Thread;

namespace TBUS_TestApp
{
	class Program
	{
		static void Main(string[] args)
		{
			WriteLine(TBusManaged.GetTBusVersion());
			string StackXMLFile = @"C:\Users\Julien\Desktop\" + 
				@"121025_BLASTEK_Control+FreqRegler_USB_BLASTEK\" + 
				@"xmls+rbf\Example_Stack_FTUSVM5J.stack.xml";

			using (var stack = new TBus_Stack("testStack", StackXMLFile))
			{
				var tempIn = new TempIn2(stack);
				
				while (!KeyAvailable)
				{
					WriteLine(tempIn.GetTemp(0));
				}
			}
		}
	}
}

/*
// SHUTTER PROGRAM
using (var stack = new TBus_Stack("qwerty", StackXMLFile))
{
	Shutter2 shutter2 = new Shutter2(stack);
	shutter2.SetMemoryByte(0);
	for (int i = 0; i < 5; ++i)
	{
		WriteLine("Strom " + ((i % 2 == 0) ? "Aus" :  "An"));
		shutter2.SetMode(i % 2);
		Thread.Sleep(2000);
	}
}
*/

/* //TADriver Program
var taDriver = new TACurrentDriver3_LDD10(stack);

for (int i = 0; i < 5; ++i)
{
	taDriver.SetCurrentBoth(0.05, 0.05);
	taDriver.SetLaserStatus(14, 14, false, false, false);
	double currA, currB, readTemp, Resistance;
	bool tempSens;
	taDriver.GetLaserCurrent(0.45, 3.33, out currA, out currB, out tempSens, out readTemp);
	double temperature = taDriver.GetTemperature(readTemp, out Resistance);

	Clear();
	WriteLine((i + 1).ToString());
	WriteLine($"CurrentA: {currA} | CurrentB: {currB}");
	WriteLine($"TempSens: {((tempSens) ? "true" : "false")} | ReadTemp: {readTemp}");
	WriteLine($"Temperature: {temperature} | Resistance: {Resistance}");
	Sleep(500);
}
*/
