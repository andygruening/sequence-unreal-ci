#include "Sequence/SequenceAPI.h"
#include "Eth/Crypto.h"
#include "RequestHandler.h"
#include "Indexer/IndexerSupport.h"
#include "Util/JsonBuilder.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "AES/aes.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "JsonUtils/JsonPointer.h"
#include "Types/ContractCall.h"
#include "Misc/Base64.h"
#include "Util/HexUtility.h"

FString SortOrderToString(ESortOrder SortOrder)
{
	return UEnum::GetValueAsString(SortOrder);
}

ESortOrder StringToSortOrder(FString String)
{
	return String == "ASC" ? ESortOrder::ASC : ESortOrder::DESC;
}

FString FPage_Sequence::ToJson()
{
	FJsonBuilder Json = FJsonBuilder();

	if(PageSize.IsSet())
	{
		Json.AddInt("pageSize", PageSize.GetValue());
	}
	if(PageNum.IsSet())
	{
		Json.AddInt("page", PageNum.GetValue());
	}
	if(TotalRecords.IsSet())
	{
		Json.AddInt("totalRecords", TotalRecords.GetValue());
	}
	if(Column.IsSet())
	{
		Json.AddString("column", Column.GetValue());
	}
	if(Sort.IsSet())
	{
		FJsonArray Array = Json.AddArray("sort");

		for(FSortBy SortBy : Sort.GetValue())
		{
			Array.AddValue(SortBy.GetJsonString());
		}
	}
	
	return Json.ToString();
}

FPage_Sequence FPage_Sequence::Convert(FPage Page,int64 TotalRecords)
{
	FPage_Sequence Ret;
	Ret.Column = Page.column;
	Ret.PageNum = Page.page;
	Ret.PageSize = Page.pageSize;
	Ret.Sort = Page.sort;
	Ret.TotalRecords = TotalRecords;
	return Ret;
}

FPage_Sequence FPage_Sequence::From(TSharedPtr<FJsonObject> Json)
{
	FPage_Sequence page = FPage_Sequence{};

	if(Json->HasField("pageSize"))
	{
		page.PageSize = static_cast<uint64>(Json->GetIntegerField("pageSize")); 
	}
	if(Json->HasField("page"))
	{
		page.PageNum =  static_cast<uint64>(Json->GetIntegerField("page")); 
	}
	if(Json->HasField("totalRecords"))
	{
		page.TotalRecords = static_cast<uint64>(Json->GetIntegerField("totalRecords")); 
	}
	if(Json->HasField("Column"))
	{
		page.Column = Json->GetStringField("Column");
	}
	if(Json->HasField("Sort"))
	{
		TArray<TSharedPtr<FJsonValue>> JsonArray = Json->GetArrayField("Sort");
		TArray<FSortBy> Array;

		for(TSharedPtr<FJsonValue> JsonVal : JsonArray)
		{
			FSortBy Result;
			if (!FJsonObjectConverter::JsonObjectToUStruct<FSortBy>(JsonVal->AsObject().ToSharedRef(), &Result))
				Array.Push(Result);
		}
		
		page.Sort = Array;
	}

	return page;
}

//Hide for now
FTransaction_Sequence FTransaction_Sequence::Convert(FTransaction_FE Transaction_Fe)
{
	return FTransaction_Sequence{
		static_cast<uint64>(Transaction_Fe.chainId),
		FAddress::From(Transaction_Fe.From),
		FAddress::From(Transaction_Fe.To),
		Transaction_Fe.AutoGas == "" ? TOptional<FString>() : TOptional(Transaction_Fe.AutoGas),
		Transaction_Fe.Nonce < 0 ? TOptional<uint64>() : TOptional(static_cast<uint64>(Transaction_Fe.Nonce)),
		Transaction_Fe.Value == "" ? TOptional<FString>() : TOptional(Transaction_Fe.Value),
		Transaction_Fe.CallData == "" ? TOptional<FString>() : TOptional(Transaction_Fe.CallData),
		Transaction_Fe.TokenAddress == "" ? TOptional<FString>() : TOptional(Transaction_Fe.TokenAddress),
		Transaction_Fe.TokenAmount == "" ? TOptional<FString>() : TOptional(Transaction_Fe.TokenAmount),
		Transaction_Fe.TokenIds.Num() == 0 ? TOptional<TArray<FString>>() : TOptional(Transaction_Fe.TokenIds),
		Transaction_Fe.TokenAmounts.Num() == 0 ? TOptional<TArray<FString>>() : TOptional(Transaction_Fe.TokenAmounts),
	};
}

const FString FTransaction_Sequence::ToJson()
{
	FJsonBuilder Json = FJsonBuilder();

	Json.AddInt("chainId", ChainId);
	Json.AddString("from", "0x" + From.ToHex());
	Json.AddString("to", "0x" + To.ToHex());

	if(this->Value.IsSet()) Json.AddString("value", this->Value.GetValue());

	return Json.ToString();
}

const TransactionID FTransaction_Sequence::ID()
{
	FUnsizedData Data = StringToUTF8(ToJson());
	return GetKeccakHash(Data).ToHex();
}

FPartnerWallet FPartnerWallet::From(TSharedPtr<FJsonObject> Json)
{
	return FPartnerWallet{
		static_cast<uint64>(Json->GetIntegerField("number")),
		static_cast<uint64>(Json->GetIntegerField("partnerId")),
		static_cast<uint64>(Json->GetIntegerField("walletIndex")),
		Json->GetStringField("walletAddress")
	};
}

FString USequenceWallet::Url(const FString Name) const
{
	return this->Hostname + this->Path + Name;
}

void USequenceWallet::SendRPC(FString Url, FString Content, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
	NewObject<URequestHandler>()
			->PrepareRequest()
			->WithUrl(Url)
			->WithHeader("Content-type", "application/json")
			->WithHeader("Authorization", this->AuthToken)
			->WithVerb("POST")
			->WithContentAsString(Content)
			->ProcessAndThen(OnSuccess, OnFailure);
}

USequenceWallet::USequenceWallet()
{
	this->Indexer = NewObject<UIndexer>();
}

USequenceWallet* USequenceWallet::Make(const FCredentials_BE& CredentialsIn)
{
	USequenceWallet * Wallet = NewObject<USequenceWallet>();
	Wallet->Init(CredentialsIn);
	return Wallet;
}

USequenceWallet* USequenceWallet::Make(const FCredentials_BE& CredentialsIn, const FString& ProviderURL)
{
	USequenceWallet * Wallet = NewObject<USequenceWallet>();
	Wallet->Init(CredentialsIn,ProviderURL);
	return Wallet;
}

void USequenceWallet::Init(const FCredentials_BE& CredentialsIn)
{
	this->Credentials = CredentialsIn;
	this->Indexer = NewObject<UIndexer>();
	this->AuthToken = "Bearer " + this->Credentials.GetIDToken();
}

void USequenceWallet::Init(const FCredentials_BE& CredentialsIn,const FString& ProviderURL)
{
	this->Credentials = CredentialsIn;
	this->Indexer = NewObject<UIndexer>();
	this->AuthToken = "Bearer " + this->Credentials.GetIDToken();
	this->ProviderUrl = ProviderURL;
}

FString USequenceWallet::GetWalletAddress()
{
	return this->Credentials.GetWalletAddress();
}

void USequenceWallet::RegisterSession(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		const TSharedPtr<FJsonObject> * Data = nullptr;
		if (Json.Get()->TryGetObjectField("data",Data))
		{
			FString RegisteredSessionId, RegisteredWalletAddress;
			if (Data->Get()->TryGetStringField("sessionId",RegisteredSessionId) && Data->Get()->TryGetStringField("wallet",RegisteredWalletAddress))
			{
				this->Credentials.RegisterSessionData(RegisteredSessionId,RegisteredWalletAddress);
				const UAuthenticator * TUAuth = NewObject<UAuthenticator>();
				TUAuth->StoreCredentials(this->Credentials);
				FString Creds = UIndexerSupport::structToString(this->Credentials);
				UE_LOG(LogTemp,Display,TEXT("Creds: %s"),*Creds);
				OnSuccess("Session Registered");
			}
			else
			{
				OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
			}
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
		}
	};
	if (this->Credentials.Valid())
		this->SequenceRPC("https://dev-waas.sequence.app/rpc/WaasAuthenticator/RegisterSession",this->BuildRegisterSessionIntent(),OnResponse,OnFailure);
	else
		OnFailure(FSequenceError(RequestFail, "[Invalid Credentials please login first]"));
}

void USequenceWallet::ListSessions(const TSuccessCallback<TArray<FSession>>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
		TArray<FSession> Sessions;
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		TArray<TSharedPtr<FJsonValue>> Arr;
		const TArray<TSharedPtr<FJsonValue>>* Data = &Arr;
		if (Json.Get()->TryGetArrayField("sessions",Data))
		{
			for (int i = 0; i < Arr.Num(); i++) {
				TSharedPtr<FJsonValue> SessionJson = Arr[i];
				FSession Session = UIndexerSupport::jsonStringToStruct<FSession>(SessionJson.Get()->AsString());
				Sessions.Push(Session);
			}

			OnSuccess(Sessions);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
		}
	};
	
	if (this->Credentials.IsRegistered() && this->Credentials.Valid())
		this->SequenceRPC("https://dev-waas.sequence.app/rpc/WaasAuthenticator/SendIntent",this->BuildListSessionIntent(),OnResponse,OnFailure);
	else
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
}

void USequenceWallet::CloseSession(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		if(Json.IsValid() && Json->GetBoolField("ok"))
		{
			this->Credentials.UnRegisterCredentials();
			UAuthenticator *Auth = NewObject<UAuthenticator>();
			Auth->StoreCredentials(this->Credentials);//update credentials to ensure we remember that this session is now closed
			OnSuccess("[SessionClosed]");
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Malformed response: " + Response));
		}
	};
	if (this->Credentials.IsRegistered() && this->Credentials.Valid())
		this->SequenceRPC("https://dev-waas.sequence.app/rpc/WaasAuthenticator/SendIntent",this->BuildCloseSessionIntent(),OnResponse,OnFailure);
	else
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
}

void USequenceWallet::SessionValidation(const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	OnSuccess("[RPC_NotActive]");
}

FString USequenceWallet::GeneratePacketSignature(const FString& Packet) const
{
	//keccakhash of the packet first
	const FHash256 SigningHash = FHash256::New();
	const FUnsizedData EncodedSigningData = StringToUTF8(Packet);
	Keccak256::getHash(EncodedSigningData.Arr.Get()->GetData(), EncodedSigningData.GetLength(), SigningHash.Ptr());
	TArray<uint8> SigningBytes;
	SigningBytes.Append(SigningHash.Ptr(),SigningHash.GetLength());
	const FString Signature = "0x" + this->Credentials.SignMessageWithSessionWallet(SigningBytes,32);
	return Signature;
}

void USequenceWallet::SignMessage(const FString& Message, const TSuccessCallback<FSignedMessage>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TSuccessCallback<FString> OnResponse = [this,OnSuccess,OnFailure](const FString& Response)
	{
		const TSharedPtr<FJsonObject> Json = UIndexerSupport::JsonStringToObject(Response);
		const TSharedPtr<FJsonObject> * Data = nullptr;
		FSignedMessage Msg;
		if (Json.Get()->TryGetObjectField("data",Data))
		{
			if (FJsonObjectConverter::JsonObjectToUStruct<FSignedMessage>(Data->ToSharedRef(), &Msg))
			{
				OnSuccess(Msg);
			}
			else
			{
				OnFailure(FSequenceError(RequestFail, "Malformed Response: " + Response));
			}
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
		}
	};
	if (this->Credentials.IsRegistered() && this->Credentials.Valid())
		this->SequenceRPC("https://dev-waas.sequence.app/rpc/WaasAuthenticator/SendIntent",this->BuildSignMessageIntent(Message),OnResponse,OnFailure);
	else
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
}

void USequenceWallet::SendTransaction(TArray<TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction>> Transactions, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
{
	FString TransactionsPayload = "[";
	
	for (TUnion<FRawTransaction, FERC20Transaction, FERC721Transaction, FERC1155Transaction> Transaction : Transactions)
	{
		switch(Transaction.GetCurrentSubtypeIndex())
		{
		case 0: //RawTransaction
			TransactionsPayload += Transaction.GetSubtype<FRawTransaction>().GetJsonString() + ",";
			break;
		case 1: //ERC20
			TransactionsPayload += Transaction.GetSubtype<FERC20Transaction>().GetJsonString() + ",";
			break;
		case 2: //ERC721
			TransactionsPayload += Transaction.GetSubtype<FERC721Transaction>().GetJsonString() + ",";
			break;
		case 3: //ERC1155
			TransactionsPayload += Transaction.GetSubtype<FERC1155Transaction>().GetJsonString() + ",";
			break;
		default: //Doesn't match
			break;
		}
	}
	TransactionsPayload.RemoveAt(TransactionsPayload.Len() - 1);
	TransactionsPayload += "]";
	TSuccessCallback<FString> OnResponse = [=](FString Response)
	{
		TSharedPtr<FJsonObject> jsonObj;
		if(FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Response), jsonObj))
		{
			OnSuccess(jsonObj);
		}
		else
		{
			OnFailure(FSequenceError(RequestFail, "Request failed: " + Response));
		}
	};
	
	if (this->Credentials.IsRegistered() && this->Credentials.Valid())
		this->SequenceRPC("https://dev-waas.sequence.app/rpc/WaasAuthenticator/SendIntent", BuildSendTransactionIntent(TransactionsPayload), OnResponse, OnFailure);
	else
		OnFailure(FSequenceError(RequestFail, "[Session Not Registered Please Register Session First]"));
}

FString USequenceWallet::BuildSignMessageIntent(const FString& message)
{//updated to match new RPC setup
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	
	const FString Wallet = this->Credentials.GetWalletAddress();
	//eip-191 and keccak hashing the message
	const FString LeadingByte = "\x19";//leading byte
	FString Payload = LeadingByte + "Ethereum Signed Message:\n";
	Payload.AppendInt(message.Len());
	Payload += message;
	const FUnsizedData PayloadBytes = StringToUTF8(Payload);
	const FString EIP_Message = "0x" + BytesToHex(PayloadBytes.Ptr(),PayloadBytes.GetLength());
	UE_LOG(LogTemp,Display,TEXT("EIP_191: %s"),*EIP_Message);
	
	//EIP-191
	const FString Data = "{\"message\":\""+EIP_Message+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"}";
	const FString SigIntent = "{\"intent\":{\"data\":"+Data+",\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"signMessage\",\"version\":\""+this->Credentials.GetWaasVersin()+"\"}}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":"+Data+",\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"signMessage\",\"signatures\":{[\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"]},\"version\":\""+this->Credentials.GetWaasVersin()+"\"}}";

	UE_LOG(LogTemp,Display,TEXT("SignMessage Intent: %s"),*Intent);
	return Intent;
}

/*
FString USequenceWallet::BuildSignMessageIntent(const FString& message)
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	
	const FString Wallet = this->Credentials.GetWalletAddress();
	//eip-191 and keccak hashing the message
	const FString LeadingByte = "\x19";//leading byte
	FString Payload = LeadingByte + "Ethereum Signed Message:\n";
	Payload.AppendInt(message.Len());
	Payload += message;
	const FUnsizedData PayloadBytes = StringToUTF8(Payload);
	const FString EIP_Message = "0x" + BytesToHex(PayloadBytes.Ptr(),PayloadBytes.GetLength());
	UE_LOG(LogTemp,Display,TEXT("EIP_191: %s"),*EIP_Message);
	//EIP-191
	
	const FString Packet = "{\\\"code\\\":\\\"signMessage\\\",\\\"expires\\\":"+expiresString+",\\\"issued\\\":"+issuedString+",\\\"message\\\":\\\""+EIP_Message+"\\\",\\\"network\\\":\\\""+this->Credentials.GetNetworkString()+"\\\",\\\"wallet\\\":\\\""+Wallet+"\\\"}";
	const FString PacketRaw = "{\"code\":\"signMessage\",\"expires\":"+expiresString+",\"issued\":"+issuedString+",\"message\":\""+EIP_Message+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"wallet\":\""+Wallet+"\"}";
	const FString Signature = this->GeneratePacketSignature(PacketRaw);
	FString Intent = "{\\\"version\\\":\\\""+this->Credentials.GetWaasVersin()+"\\\",\\\"packet\\\":"+Packet+",\\\"signatures\\\":[{\\\"session\\\":\\\""+this->Credentials.GetSessionId()+"\\\",\\\"signature\\\":\\\""+Signature+"\\\"}]}";
	UE_LOG(LogTemp,Display,TEXT("SignMessageIntent: %s"),*Intent);
	return Intent;
}
*/


//{"intent":{"data":{"identifier":"","network":"137","transactions":[{"to":"0x9766bf76b2E3e7BCB8c61410A3fC873f1e89b43f","type":"transaction","value":"1000000047497451"}],"wallet":"0x75700a9dC31ff38b93EafDC380c28e1B816f6799"},"expiresAt":1708466035,"issuedAt":1708466005,"name":"sendTransaction","signatures":[{"sessionId":"0x00bab99b6f4a25e44e9654515fc0b9232f6f87fa3b","signature":"0x56444d46815935d3e2420bc5aaa5060016efbf060c9f002f0dd6bd64c82162e06b4ebc46f6b6d991c6a1abe7de356f05b61f673c43fb0a50da0116a0b7d7ff4e1c"}],"version":"1.0.0"}}'
FString USequenceWallet::BuildSendTransactionIntent(const FString& Txns)
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString identifier = "unreal-sdk-" + FDateTime::UtcNow().ToString() + "-" + this->Credentials.GetWalletAddress();
	//                         {"intent":   {"data": {"identifier":""                  ,"network":"137"                                         ,"transactions":     []   ,"wallet":"0x75700a9dC31ff38b93EafDC380c28e1B816f6799"} ,"expiresAt":1708466035         ,"issuedAt":1708466005         ,"name":"sendTransaction"    ,"version":"1.0.0"}}
	const FString SigIntent = "{\"intent\":{\"data\":{\"identifier\":\""+identifier+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"transactions\":"+Txns+",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"sendTransaction\",\"version\":\""+this->Credentials.GetWaasVersin()+"\"}}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	FString Intent = "{\"intent\":{\"data\":{\"identifier\":\""+identifier+"\",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"transactions\":"+Txns+",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"sendTransaction\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersin()+"\"}}";
	UE_LOG(LogTemp,Display,TEXT("SendTransactionIntent: %s"),*Intent);
	return Intent;
}

/*FString USequenceWallet::BuildSendTransactionIntent(const FString& Txns)
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString identifier = "unreal-sdk-" + FDateTime::UtcNow().ToString() + "-" + this->Credentials.GetWalletAddress();
	FString PaddedTxns = Txns;
	const FString srch_slash = TEXT("\"");
	const FString replace = TEXT("\\\"");
	const TCHAR* srch_ptr_slash = *srch_slash;
	const TCHAR* rep_ptr = *replace;
	(PaddedTxns).ReplaceInline(srch_ptr_slash, rep_ptr, ESearchCase::IgnoreCase);
	
	const FString Packet = "{\\\"code\\\":\\\"sendTransaction\\\",\\\"expires\\\":"+expiresString+",\\\"identifier\\\":\\\""+identifier+"\\\",\\\"issued\\\":"+issuedString+",\\\"network\\\":\\\""+this->Credentials.GetNetworkString()+"\\\",\\\"transactions\\\":"+PaddedTxns+",\\\"wallet\\\":\\\""+this->Credentials.GetWalletAddress()+"\\\"}";
	const FString RawPacket = "{\"code\":\"sendTransaction\",\"expires\":"+expiresString+",\"identifier\":\""+identifier+"\",\"issued\":"+issuedString+",\"network\":\""+this->Credentials.GetNetworkString()+"\",\"transactions\":"+Txns+",\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"}";
	const FString Signature = this->GeneratePacketSignature(RawPacket);
	FString Intent = "{\\\"version\\\":\\\""+this->Credentials.GetWaasVersin()+"\\\",\\\"packet\\\":"+Packet+",\\\"signatures\\\":[{\\\"session\\\":\\\""+this->Credentials.GetSessionId()+"\\\",\\\"signature\\\":\\\""+Signature+"\\\"}]}";
	UE_LOG(LogTemp,Display,TEXT("SendTransactionIntent: %s"),*Intent);
	return Intent;
}*/

FString USequenceWallet::BuildRegisterSessionIntent()
{//updated to match new RPC setup
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString Data = "{\"idToken\":\""+this->Credentials.GetIDToken()+"\",\"sessionId\":\""+this->Credentials.GetSessionId()+"\"}";
	const FString GUID = FGuid::NewGuid().ToString();
	const FString SigIntent = "{\"intent\":{\"data\":"+Data+"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"openSession\",\"version\":\""+this->Credentials.GetWaasVersin()+"\",\"friendlyName\":\""+GUID+"\"}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":"+Data+"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"openSession\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersin()+"\",\"friendlyName\":\""+GUID+"\"}";
	UE_LOG(LogTemp,Display,TEXT("RegisterSession Intent: %s"),*Intent);
	return Intent;
}

/*FString USequenceWallet::BuildRegisterSessionIntent()
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString Packet = "{\\\"code\\\":\\\"openSession\\\",\\\"expires\\\":"+expiresString+",\\\"issued\\\":"+issuedString+",\\\"session\\\":\\\""+this->Credentials.GetSessionId()+"\\\",\\\"proof\\\":{\\\"idToken\\\":\\\""+this->Credentials.GetIDToken()+"\\\"}}";
	const FString Intent = "{\\\"version\\\":\\\""+this->Credentials.GetWaasVersin()+"\\\",\\\"packet\\\":"+Packet+"}";
	UE_LOG(LogTemp,Display,TEXT("RegisterSessionIntent: %s"),*Intent);
	return Intent;
}*/

FString USequenceWallet::BuildListSessionIntent()
{
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);
	const FString SigIntent = "{\"intent\":{\"data\":{\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"listSessions\",\"version\":\""+this->Credentials.GetWaasVersin()+"\"}}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":{\"wallet\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"listSessions\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersin()+"\"}}";
	UE_LOG(LogTemp,Display,TEXT("ListSessionIntent: %s"),*Intent);
	return Intent;
}

/*FString USequenceWallet::BuildListSessionIntent()
{
	const FString Intent = "{\"sessionId\":\""+this->Credentials.GetSessionId()+"\"}";
	UE_LOG(LogTemp,Display,TEXT("ListSessionIntent: %s"),*Intent);
	return Intent;
}*/

//'{"intent":{"data":{"sessionId":"0x00fea7a37d92b02c31258dc3a3732a1fc62ac37e65"},"expiresAt":1708548573,"issuedAt":1708548543,"name":"closeSession","signatures":[{"sessionId":"0x00fea7a37d92b02c31258dc3a3732a1fc62ac37e65","signature":"0x15bb79be1d1a1e692c225b47a8689cdafea5a6722743f0f24158d0830ba0af6f68f78603c31963fe5d851b1706a71e0af81b6b41f4e9a9a261281b1137b76f9f1b"}],"version":"1.0.0"}}'
FString USequenceWallet::BuildCloseSessionIntent()
{//updated
	const int64 issued = FDateTime::UtcNow().ToUnixTimestamp() - 30;
	const int64 expires = issued + 86400;
	const FString issuedString = FString::Printf(TEXT("%lld"),issued);
	const FString expiresString = FString::Printf(TEXT("%lld"),expires);

	const FString SigIntent = "{\"intent\":{\"data\":{\"sessionId\":\""+this->Credentials.GetSessionId()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"closeSession\",\"version\":\""+this->Credentials.GetWaasVersin()+"\"}}";
	const FString Signature = this->GeneratePacketSignature(SigIntent);
	const FString Intent = "{\"intent\":{\"data\":{\"sessionId\":\""+this->Credentials.GetWalletAddress()+"\"},\"expiresAt\":"+expiresString+",\"issuedAt\":"+issuedString+",\"name\":\"listSessions\",\"signatures\":[{\"sessionId\":\""+this->Credentials.GetSessionId()+"\",\"signature\":\""+Signature+"\"}],\"version\":\""+this->Credentials.GetWaasVersin()+"\"}}";
	UE_LOG(LogTemp,Display,TEXT("CloseSessionIntent: %s"),*Intent);
	return Intent;
}

FString USequenceWallet::BuildSessionValidationIntent()
{
	const FString Intent = "{\\\"sessionId\\\":\\\""+this->Credentials.GetSessionId()+"\\\"}";
	UE_LOG(LogTemp,Display,TEXT("SessionValidationIntent: %s"),*Intent);
	return Intent;
}

template <typename T> void USequenceWallet::SequenceRPC(FString Url, FString Content, TSuccessCallback<T> OnSuccess, FFailureCallback OnFailure)
{
	NewObject<URequestHandler>()
	->PrepareRequest()
	->WithUrl(Url)
	->WithHeader("Content-type", "application/json")
	->WithHeader("Accept", "application/json")
	->WithHeader("X-Access-Key", Credentials.GetProjectAccessKey())
	->WithVerb("POST")
	->WithContentAsString(Content)
	->ProcessAndThen(OnSuccess, OnFailure);
}

FString USequenceWallet::getSequenceURL(FString endpoint)
{
	return this->sequenceURL + endpoint;
}

TArray<FContact_BE> USequenceWallet::buildFriendListFromJson(FString json)
{
	TArray<FContact_BE> friendList;
	TSharedPtr<FJsonObject> jsonObj;

	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(json), jsonObj))
	{
		const TArray<TSharedPtr<FJsonValue>>* storedFriends;
		if (jsonObj.Get()->TryGetArrayField("friends", storedFriends))
		{
			for (TSharedPtr<FJsonValue> friendData : *storedFriends)
			{
				const TSharedPtr<FJsonObject>* fJsonObj;
				if (friendData.Get()->TryGetObject(fJsonObj))//need it as an object
				{
					FContact_BE newFriend;
					newFriend.Public_Address = fJsonObj->Get()->GetStringField("userAddress");
					newFriend.Nickname = fJsonObj->Get()->GetStringField("nickname");
					friendList.Add(newFriend);
				}
			}
		}
	}
	else
	{//failure
		UE_LOG(LogTemp, Error, TEXT("Failed to convert String: %s to Json object"), *json);
	}
	return friendList;
}

//DEPRECATED
/*
* Gets the friend data from the given username!
* This function appears to require some form of authentication (perhaps all of the sequence api does)
* @Deprecated
*/
void USequenceWallet::getFriends(FString username, TSuccessCallback<TArray<FContact_BE>> OnSuccess, FFailureCallback OnFailure)
{
	FString json_arg = "{}";
	
	SendRPC(getSequenceURL("friendList"), json_arg, [=](FString Content)
		{
			OnSuccess(buildFriendListFromJson(Content));
		}, OnFailure);
}

TArray<FItemPrice_BE> USequenceWallet::buildItemUpdateListFromJson(FString json)
{
	UE_LOG(LogTemp, Error, TEXT("Received json: %s"), *json);
	TSharedPtr<FJsonObject> jsonObj;
	FUpdatedPriceReturn updatedPrices;

	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(json), jsonObj))
	{
		if (FJsonObjectConverter::JsonObjectToUStruct<FUpdatedPriceReturn>(jsonObj.ToSharedRef(), &updatedPrices))
		{
			return updatedPrices.tokenPrices;
		}
	}
	else
	{//failure
		UE_LOG(LogTemp, Error, TEXT("Failed to convert String: %s to Json object"), *json);
	}
	TArray<FItemPrice_BE> updatedItems;
	return updatedItems;
}

void USequenceWallet::getUpdatedCoinPrice(FID_BE itemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	TArray<FID_BE> items;
	items.Add(itemToUpdate);
	getUpdatedCoinPrices(items, OnSuccess, OnFailure);
}

void USequenceWallet::getUpdatedCoinPrices(TArray<FID_BE> itemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	FString args = "{\"tokens\":";
	FString jsonObjString = "";
	TArray<FString> parsedItems;
	for (FID_BE item : itemsToUpdate)
	{
		if (FJsonObjectConverter::UStructToJsonObjectString<FID_BE>(item, jsonObjString))
			parsedItems.Add(jsonObjString);
	}
	args += UIndexerSupport::stringListToSimpleString(parsedItems);
	args += "}";

	SendRPC(getSequenceURL("getCoinPrices"), args, [=](FString Content)
		{
			OnSuccess(buildItemUpdateListFromJson(Content));
		}, OnFailure);
}

void USequenceWallet::getUpdatedCollectiblePrice(FID_BE itemToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	TArray<FID_BE> items;
	items.Add(itemToUpdate);
	getUpdatedCollectiblePrices(items, OnSuccess, OnFailure);
}

void USequenceWallet::getUpdatedCollectiblePrices(TArray<FID_BE> itemsToUpdate, TSuccessCallback<TArray<FItemPrice_BE>> OnSuccess, FFailureCallback OnFailure)
{
	FString args = "{\"tokens\":";
	FString jsonObjString = "";
	TArray<FString> parsedItems;
	for (FID_BE item : itemsToUpdate)
	{
		if (FJsonObjectConverter::UStructToJsonObjectString<FID_BE>(item, jsonObjString))
			parsedItems.Add(jsonObjString);
	}
	args += UIndexerSupport::stringListToSimpleString(parsedItems);
	args += "}";

	SendRPC(getSequenceURL("getCollectiblePrices"), args, [=](FString Content)
		{
			OnSuccess(buildItemUpdateListFromJson(Content));
		}, OnFailure);
}

FString USequenceWallet::buildQR_Request_URL(FString walletAddress,int32 size)
{
	FString urlSize = "/";
	urlSize.AppendInt(size);

	return sequenceURL_QR + encodeB64_URL(walletAddress) + urlSize;
}

//we only need to encode base64URL we don't decode them as we receive the QR code
FString USequenceWallet::encodeB64_URL(FString data)
{
	FString ret;
	UE_LOG(LogTemp, Display, TEXT("Pre encoded addr: [%s]"), *data);
	ret = FBase64::Encode(data);
	UE_LOG(LogTemp, Display, TEXT("Post encoded addr: [%s]"), *ret);
	//now we just gotta do some swaps to make it base64 URL compliant
	// + -> -
	// / -> _ 

	FString srch_plus = TEXT("+");
	FString rep_plus = TEXT("-");
	FString srch_slash = TEXT("/");
	FString rep_slash = TEXT("_");

	const TCHAR* srch_ptr_plus = *srch_plus;
	const TCHAR* rep_ptr_plus = *rep_plus;
	const TCHAR* srch_ptr_slash = *srch_slash;
	const TCHAR* rep_ptr_slash = *rep_slash;

	ret.ReplaceInline(srch_ptr_plus, rep_ptr_plus, ESearchCase::IgnoreCase);//remove + and replace with -
	ret.ReplaceInline(srch_ptr_slash, rep_ptr_slash, ESearchCase::IgnoreCase);//remove / and replace with _

	UE_LOG(LogTemp, Display, TEXT("B64-URL encoded addr: [%s]"), *ret);

	return ret;
}

//Indexer Calls

void USequenceWallet::Ping(int64 chainID, TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->Ping(chainID, OnSuccess, OnFailure);
}

void USequenceWallet::Version(int64 chainID, TSuccessCallback<FVersion> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->Version(chainID,OnSuccess,OnFailure);
}

void USequenceWallet::RunTimeStatus(int64 chainID, TSuccessCallback<FRuntimeStatus> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->RunTimeStatus(chainID, OnSuccess, OnFailure);
}

void USequenceWallet::GetChainID(int64 chainID, TSuccessCallback<int64> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetChainID(chainID, OnSuccess, OnFailure);
}

void USequenceWallet::GetEtherBalance(int64 chainID, FString accountAddr, TSuccessCallback<FEtherBalance> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetEtherBalance(chainID, accountAddr, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenBalances(int64 chainID, FGetTokenBalancesArgs args, TSuccessCallback<FGetTokenBalancesReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetTokenBalances(chainID, args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenSupplies(int64 chainID, FGetTokenSuppliesArgs args, TSuccessCallback<FGetTokenSuppliesReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetTokenSupplies(chainID, args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTokenSuppliesMap(int64 chainID, FGetTokenSuppliesMapArgs args, TSuccessCallback<FGetTokenSuppliesMapReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetTokenSuppliesMap(chainID, args, OnSuccess, OnFailure);
}

void USequenceWallet::GetBalanceUpdates(int64 chainID, FGetBalanceUpdatesArgs args, TSuccessCallback<FGetBalanceUpdatesReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetBalanceUpdates(chainID, args, OnSuccess, OnFailure);
}

void USequenceWallet::GetTransactionHistory(int64 chainID, FGetTransactionHistoryArgs args, TSuccessCallback<FGetTransactionHistoryReturn> OnSuccess, FFailureCallback OnFailure)
{
	if (this->Indexer)
		this->Indexer->GetTransactionHistory(chainID, args, OnSuccess, OnFailure);
}

void USequenceWallet::BlockByNumber(uint64 Number, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).BlockByNumber(Number, OnSuccess, OnFailure);
}

void USequenceWallet::BlockByNumber(EBlockTag Tag, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).BlockByNumber(Tag,OnSuccess,OnFailure);
}

void USequenceWallet::BlockByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).BlockByHash(Hash, OnSuccess, OnFailure);
}

void USequenceWallet::BlockNumber(TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).BlockNumber(OnSuccess, OnFailure);
}

void USequenceWallet::HeaderByNumber(uint64 Id, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).HeaderByNumber(Id, OnSuccess, OnFailure);
}

void USequenceWallet::HeaderByNumber(EBlockTag Tag, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).HeaderByNumber(Tag, OnSuccess, OnFailure);
}

void USequenceWallet::HeaderByHash(FHash256 Hash, TSuccessCallback<FHeader> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).HeaderByHash(Hash,OnSuccess,OnFailure);
}

void USequenceWallet::TransactionByHash(FHash256 Hash, TSuccessCallback<TSharedPtr<FJsonObject>> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).TransactionByHash(Hash,OnSuccess,OnFailure);
}

void USequenceWallet::TransactionCount(FAddress Addr, uint64 Number, TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).TransactionCount(Addr,Number,OnSuccess,OnFailure);
}

void USequenceWallet::TransactionCount(FAddress Addr, EBlockTag Tag, TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).TransactionCount(Addr,Tag,OnSuccess,OnFailure);
}

void USequenceWallet::TransactionReceipt(FHash256 Hash, TSuccessCallback<FTransactionReceipt> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).TransactionReceipt(Hash,OnSuccess,OnFailure);
}

void USequenceWallet::GetGasPrice(TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).GetGasPrice(OnSuccess, OnFailure);
}

void USequenceWallet::EstimateContractCallGas(FContractCall ContractCall, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).EstimateContractCallGas(ContractCall,OnSuccess,OnFailure);
}

void USequenceWallet::EstimateDeploymentGas(FAddress From, FString Bytecode, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).EstimateDeploymentGas(From,Bytecode,OnSuccess,OnFailure);
}

void USequenceWallet::DeployContract(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallback<FAddress> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).DeployContract(Bytecode, PrivKey, ChainId, OnSuccess, OnFailure);
}

void USequenceWallet::DeployContractWithHash(FString Bytecode, FPrivateKey PrivKey, int64 ChainId, TSuccessCallbackTuple<FAddress, FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).DeployContractWithHash(Bytecode,PrivKey,ChainId,OnSuccess,OnFailure);
}

void USequenceWallet::NonceAt(uint64 Number, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).NonceAt(Number,OnSuccess,OnFailure);
}

void USequenceWallet::NonceAt(EBlockTag Tag, TSuccessCallback<FBlockNonce> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).NonceAt(Tag,OnSuccess,OnFailure);
}

void USequenceWallet::SendRawTransaction(FString Data, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).SendRawTransaction(Data,OnSuccess,OnFailure);
}

void USequenceWallet::ChainId(TSuccessCallback<uint64> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).ChainId(OnSuccess,OnFailure);
}

void USequenceWallet::Call(FContractCall ContractCall, uint64 Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).Call(ContractCall,Number,OnSuccess,OnFailure);
}

void USequenceWallet::Call(FContractCall ContractCall, EBlockTag Number, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).Call(ContractCall,Number,OnSuccess,OnFailure);
}

void USequenceWallet::NonViewCall(FEthTransaction transaction, FPrivateKey PrivateKey, int ChainID, TSuccessCallback<FUnsizedData> OnSuccess, FFailureCallback OnFailure)
{
	Provider(this->ProviderUrl).NonViewCall(transaction, PrivateKey, ChainID, OnSuccess, OnFailure);
}