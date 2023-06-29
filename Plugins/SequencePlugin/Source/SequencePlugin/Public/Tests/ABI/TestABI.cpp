#include "ABI/ABI.h"
#include "HexUtility.h"
#include "Misc/AutomationTest.h"
#include "ABI/ABITypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestABI, "Public.TestABI",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool EncoderTest(FABIProperty* MProperty, const FString& MethodSig, const FString& CorrectVal)
{
	TArray<FABIProperty*> Properties;
	Properties.Push(MProperty);
	const FString encoding = ABI::Encode(MethodSig, Properties).ToHex();
	UE_LOG(LogTemp, Display, TEXT("String encoding: %s"), *encoding);
	return encoding.Equals(CorrectVal);
}

bool TestABI::RunTest(const FString& Parameters)
{
	//Test 0 (bad)
	//TestInt256(int256)
	//TestInt256(15)
	//0x42166d6a000000000000000000000000000000000000000000000000000000000000000f
	auto P0 = FABIIntProperty(15);
	//if(!EncoderTest(&P0, FString("TestInt256(int256)"), FString("42166d6a000000000000000000000000000000000000000000000000000000000000000f"))) return false;
	
	
	 //Test 1 
	 //TestUint256(uint256)
	 //TestUint256(15)
	 //0x6a6040e8000000000000000000000000000000000000000000000000000000000000000f
	auto P1 = FABIUInt32Property(15);
	//if(!EncoderTest(&P1, FString("TestUint256(uint256)"), FString("6a6040e8000000000000000000000000000000000000000000000000000000000000000f"))) return false;
	
	//Test 2 (wrong)
	//TestInt256(int256)
	//TestInt256(-15)
	//0x42166d6afffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1
	auto P2 = FABIIntProperty(-15);
	//if(!EncoderTest(&P2, FString("TestInt256(int256)"), FString("42166d6afffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1"))) return false;
	
	//Test 3 
	//TestUint(uint)
	//TestUint(15)
	//0x949df7cd000000000000000000000000000000000000000000000000000000000000000f
	auto P3 = FABIUInt32Property(15);
	//if(!EncoderTest(&P3, FString("TestUint(uint)"), FString("949df7cd000000000000000000000000000000000000000000000000000000000000000f"))) return false;

	//Test 4
	//Testint(int)
	//Testint(-15)
	//0x87698e9ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1
	auto P4 = FABIIntProperty(-15);
	//if(!EncoderTest(&P4, FString("Testint(int)"), FString("87698e9ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1"))) return false;
	
	//Test 5
	//TestUint256(uint256)
	//TestUint256(150000000000000)
	//0x6a6040e80000000000000000000000000000000000000000000000000000886c98b76000
	//auto P5 = FABIUInt32Property(150000000000000);
	//if(!EncoderTest(&P5, FString("TestUint256(uint256)"), FString("6a6040e80000000000000000000000000000000000000000000000000000886c98b76000"))) return false;
	
	//Test 6
	//TestInt256(int256)
	//TestInt256(-150000000000000)
	//0x42166d6affffffffffffffffffffffffffffffffffffffffffffffffffff77936748a000
	//auto P6 = FABIIntProperty(-150000000000000);
	//if(!EncoderTest(&P6, FString("TestInt256(int256)"), FString("42166d6affffffffffffffffffffffffffffffffffffffffffffffffffff77936748a000"))) return false;
	
	//Test 7
	//TestAddress(address)
	//TestAddress(0x71C7656EC7ab88b098defB751B7401B5f6d8976F)
	//0xb447d16100000000000000000000000071c7656ec7ab88b098defb751b7401b5f6d8976f
	auto P7 = FABIAddressProperty(FAddress::From("71C7656EC7ab88b098defB751B7401B5f6d8976F"));
	if(!EncoderTest(&P7, FString("TestAddress(address)"), FString("b447d16100000000000000000000000071c7656ec7ab88b098defb751b7401b5f6d8976f"))) return false;

	//Test 8
	//TestBool(bool)
	//TestBool(true)
	//0x05aca3060000000000000000000000000000000000000000000000000000000000000001
	auto P8 = FABIBooleanProperty(true);
	if(!EncoderTest(&P8, FString("TestBool(bool)"), FString("05aca3060000000000000000000000000000000000000000000000000000000000000001"))) return false;

	//Test 9
	//TestBool(bool)
	//TestBool(false)
	//0x05aca3060000000000000000000000000000000000000000000000000000000000000000
	auto P9 = FABIBooleanProperty(false);
	if(!EncoderTest(&P9, FString("TestBool(bool)"), FString("05aca3060000000000000000000000000000000000000000000000000000000000000000"))) return false;

	//Test 10
	//TestFixedByte(bytes10)
	//TestFixedByte("abcdeabcde")
	//0xb8b97ccc6162636465616263646500000000000000000000000000000000000000000000
	//auto P10 = FABIBytesProperty(false);
	//if(!EncoderTest(&P9, FString("TestBool(bool)"), FString("05aca3060000000000000000000000000000000000000000000000000000000000000000"))) return false;


	//Test 11
	//TestStaticFixedArray(uint256[5])
	//TestStaticFixedArray([15, 200, 12, 20, 20])
	//0x33bd6a09000000000000000000000000000000000000000000000000000000000000000f00000000000000000000000000000000000000000000000000000000000000c8000000000000000000000000000000000000000000000000000000000000000c00000000000000000000000000000000000000000000000000000000000000140000000000000000000000000000000000000000000000000000000000000014

	//Test 12
	//TestDynamicFixedArray(string[5])
	//TestDynamicFixedArray(["a", "ab", "abc", "yz", "z"])
	//0xac355f05000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000e00000000000000000000000000000000000000000000000000000000000000120000000000000000000000000000000000000000000000000000000000000016000000000000000000000000000000000000000000000000000000000000001a00000000000000000000000000000000000000000000000000000000000000001610000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000026162000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000361626300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002797a00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000017a00000000000000000000000000000000000000000000000000000000000000
	
	//Test 13
	//TestString(string)
	//TestString("abcdef")
	//0x4979abcc000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000066162636465660000000000000000000000000000000000000000000000000000
	auto P13 = FABIStringProperty("abcdef");
	if(!EncoderTest(&P13, FString("TestString(string)"), FString("4979abcc000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000066162636465660000000000000000000000000000000000000000000000000000"))) return false;

	
	//Test 14
	//TestBytes(bytes)
	//TestBytes("abcdeabcde")
	//0x229bb9e90000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000000a6162636465616263646500000000000000000000000000000000000000000000
	char s[] = "abcdeabcde";
	auto P14 = FABIBytesProperty(FNonUniformData((uint8*)s, 10));
	if(!EncoderTest(&P14, FString("TestBytes(bytes)"), FString("229bb9e90000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000000a6162636465616263646500000000000000000000000000000000000000000000"))) return false;

	//Test 15
	//TestStaticNonFixedArray(uint256[])
	//TestStaticNonFixedArray([110000, 34, 3])
	//0x00903ba500000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000001adb000000000000000000000000000000000000000000000000000000000000000220000000000000000000000000000000000000000000000000000000000000003

	//Test 16
	//TestDynamicNonFixedArray(string[])
	//TestDynamicNonFixedArray(["xyz", "abc"])
	//0x411548440000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000080000000000000000000000000000000000000000000000000000000000000000378797a000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000036162630000000000000000000000000000000000000000000000000000000000

	//Test 17
	//TestManyInputs(string,uint256,int256[],address,bool)
	//TestManyInputs("sequence", 55, [10, 2, 2], "0x71C7656EC7ab88b098defB751B7401B5f6d8976F", true)*/
	//0xc28546fd00000000000000000000000000000000000000000000000000000000000000a0000000000000000000000000000000000000000000000000000000000000003700000000000000000000000000000000000000000000000000000000000000e000000000000000000000000071c7656ec7ab88b098defb751b7401b5f6d8976f0000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000873657175656e63650000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000002




	

//test new Abi

	/*
	auto LOG_ENCODED = true;
	auto LOG_DECODED = true;

	// DATA
	
	uint8 Number1 = 1;
	auto NumberArg1 = FABIArg::New(Number1);

	uint8 Number2 = 2;
	auto NumberArg2 = FABIArg::New(Number2);

	uint8 Number3 = 3;
	auto NumberArg3 = FABIArg::New(Number3);

	FString String1 = "one";
	auto StringArg1 = FABIArg::New(String1);
	
	FString String2 = "two";
	auto StringArg2 = FABIArg::New(String2);
	
	FString String3 = "one";
	auto StringArg3 = FABIArg::New(String3);

	auto ArrayArg1 = FABIArg::New(new FABIArg[2]{NumberArg1, NumberArg2}, 2);
	auto ArrayArg2 = FABIArg::New(new FABIArg[1]{NumberArg3}, 1);
	auto ArrayArg3 = FABIArg::New(new FABIArg[2]{ArrayArg1, ArrayArg2}, 2);
	auto ArrayArg4 = FABIArg::New(new FABIArg[3]{StringArg1, StringArg2, StringArg3}, 3);
	
	FABIArg* Args = new FABIArg[2]{ArrayArg3, ArrayArg4};
	
	// ENCODING
	auto BlockNumInt = (Args[0]).GetBlockNum() + (Args[1]).GetBlockNum();
	FString BlockNum = FString::FromInt(BlockNumInt);
	auto Obj = ABI::EncodeArgs("test", Args, 2);
	
	if(LOG_ENCODED)
	{
		UE_LOG(LogTemp, Display, TEXT("HEADER: %s"), *HashToHexString(GMethodIdByteLength, &Obj.Arr[0]));
		for(auto i = 0; i < BlockNumInt; i++)
		{
			auto Addr = GMethodIdByteLength + GBlockByteLength * i;
			UE_LOG(LogTemp, Display, TEXT("%i %s"), Addr, *HashToHexString(GBlockByteLength, &Obj.Arr[Addr]));
		}
	}

	// DECODING STUBS
	auto DecodeNumberArg1 = FABIArg::Empty(STATIC);

	auto DecodeStringArg1 = FABIArg::Empty(STRING);

	auto DecodeArrayArg1 = FABIArg{ARRAY, 1, new FABIArg[2]{DecodeNumberArg1}};
	auto DecodeArrayArg3 = FABIArg{ARRAY, 1, new FABIArg[2]{DecodeArrayArg1}};
	auto DecodeArrayArg4 = FABIArg{ARRAY, 1, new FABIArg[3]{DecodeStringArg1}};
	
	FABIArg* DecodeArgs = new FABIArg[]{DecodeArrayArg3, DecodeArrayArg4};
	
	ABI::DecodeArgs(Obj, DecodeArgs, 2);

	auto Arr1 = DecodeArgs[0];
	auto Arr2 = DecodeArgs[1];

	UE_LOG(LogTemp, Display, TEXT(" Arr 1 of size %i at %i"), Arr1.Length, (uint64) Arr1.Data);
	UE_LOG(LogTemp, Display, TEXT(" Arr 2 of size %i at %i"), Arr2.Length, (uint64) Arr2.Data);

	for(auto i = 0; i < Arr1.Length; i++)
	{
		auto NumArr = Arr1.ToArr()[0];

		UE_LOG(LogTemp, Display, TEXT("Num Arr of size %i at %i"), NumArr.Length, (uint64) NumArr.Data);
		
		for(auto j = 0; j < NumArr.Length; j++)
		{
			auto Num = NumArr.ToArr()[0];

			UE_LOG(LogTemp, Display, TEXT("Num Arg at offset %i"), (uint64) Num.Data);
		}
	}

	for(auto i = 0; i < Arr2.Length; i++)
	{
		auto StringArg = Arr2.ToArr()[i];
		
		UE_LOG(LogTemp, Display, TEXT("String Arg %i is %s"), i, *StringArg.ToString());
	}
	// Make the test pass by returning true, or fail by returning false. 	*/
	return true;

}

