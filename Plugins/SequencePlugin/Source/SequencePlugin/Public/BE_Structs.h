#pragma once

#include "CoreMinimal.h"
#include "BE_Enums.h"
#include "Indexer_Enums.h"
#include "BE_Structs.generated.h"


/*
* Used to store connected network settings
* info
*/
USTRUCT(BlueprintType)
struct FNetworkProfileSetting_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Indexer_URL = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Relayer_URL = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Block_Explorer_URL = "";
};

/*
* Stores all of a networks data including a
* FNetwork_Profile_BE
*/
USTRUCT(BlueprintType)
struct FNetwork_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString network_name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        bool is_default = false;//default selected network need at least 1
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FNetworkProfileSetting_BE default_network_settings;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FNetworkProfileSetting_BE current_network_settings;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* network_icon = nullptr;
};

/*
* Subject to change but stores
* a system sessions data this seems to be
* specific to the computing system ie) wintel or arm mac os for example
*/
USTRUCT(BlueprintType)
struct FSystemSession_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString System_Session = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString System_Session_Key = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* System_Session_Icon = nullptr;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FNetwork_BE Network;
};


/*
* Stores active session data not sure how this differs from system session data
* outside that it may be independent of platform specific details
*/
USTRUCT(BlueprintType)
struct FActiveSession_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Current_Session = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Session_Key = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Last_Seen_Date_Time = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Platform_Data = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Session_Icon = nullptr;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FNetwork_BE Network;
};

/*
* The data struct I use to store contact data
* critical data is the public address
*/
USTRUCT(BlueprintType)
struct FContact_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Nickname = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Public_Address = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Profile_Icon = nullptr;
};

/*
* This stores data about a currency
* note for the various currency symbols there maybe some
* manual input of utf-8 or utf-16 char symbols in order for those to appear correctly in
* the front
*/
USTRUCT(BlueprintType)
struct FSelectableCurrency_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Long_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Short_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Symbol = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Mult = 0.0;//NOP value for right now until I know how currency data is structured
};

/*
* Used to store a generic entry in a history txn,
* a single history txn may have many of these!
* NOTE: the follow entries may not always need to be populated in the case of coin based history items
* 1) token_collection_name (coins aren't apart of collections)
* 2) Collection_Short_name (coins aren't apart of collections) double note short names are abreviated main names
* 3) collection icon
* 
* NOTE: this is used more so for frontend use rather than backend use
* 
*/
USTRUCT(BlueprintType)
struct FHistoryElement_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Item_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Item_Short_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Token_Collection_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Collection_Short_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Collection_Icon = nullptr;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Item_Amount = 0.0;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Value_Per_Item = 0.0;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Item_Icon = nullptr;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TEnumAsByte<EHistoryItemType> History_Item_Type = EHistoryItemType::Token_HIT;
};

/*
* This will probably change to something more accurate in the future but 
* good enough for now
*/
USTRUCT(BlueprintType)
struct FMonthDayYear_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int32 Month = -1;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int32 Day = -1;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int32 Year = -1;
};

/*
* This is history specific to token collections
* this is used more so for frontend use rather than backend parsing
*/
USTRUCT(BlueprintType)
struct FTokenHistoryCollection_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Collection_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Collection_Short_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FHistoryElement_BE> Token_Data;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Collection_Icon = nullptr;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        bool From_To = false;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TEnumAsByte<ETXNType> TXN_Type = ETXNType::TXN_Receive;//this enum consists of send,receive,swap the base txnTransfer type only has send and receive!
};

USTRUCT(BlueprintType)
struct FID_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString contractAddress = "";//used to uniquely identify this item TYPE
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int64 chainID = -1;//used for getting updated price data!
};

/*
* Used for updating the front with up to date pricing data
*/
USTRUCT(BlueprintType)
struct FPrice_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float value = 0.0;//new value
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString currency = "";//not sure what this is for?
};

/*
* Used for updating the front with up to date pricing data
* This is also meant to mirror what we receive from the sequence side
* hence the differing names!
*/
USTRUCT(BlueprintType)
struct FItemPrice_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FID_BE Token;//unique item identifier pairs to itemID but sequence calls it Token
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FPrice_BE price;
};

/*
* This is how coin data is stored in system
*/
USTRUCT(BlueprintType)
struct FCoin_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Coin_Symbol = nullptr;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Coin_Long_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Coin_Short_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Coin_Amount = 0.0;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Coin_Value = 0.0;//per 1 coin!
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Formatted_Amount = "";//parser in frontend ignore in backend
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Formatted_Total_Value = "";//perser in frontend ignore in backend
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Formatted_Single_Value = "";//parser in frontend ignore in backend
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TEnumAsByte<EContractType> Coin_Standard = EContractType::ERC1155;//ERC 1155, etc
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FID_BE itemID;//used to uniquely identify this item TYPE
};

/*
* Specific details about nft's
*/
USTRUCT(BlueprintType)
struct FNFTDetails_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString External_URL = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString token_id = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Contract_Address = "";//used to uniquely identify this item TYPE
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FID_BE itemID;//used to uniquely identify this item TYPE
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TEnumAsByte<EContractType> Token_Standard = EContractType::ERC1155;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Network = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Network_Icon = nullptr;
};

/*
* this is how nft data is stored in system
*/
USTRUCT(BlueprintType)
struct FNFT_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* NFT_Icon = nullptr;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString NFT_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString NFT_Short_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Collection_Long_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Collection_Short_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Collection_Icon = nullptr;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int32 Amount = -1;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Value = 0.0;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Formatted_Total_Value = "";//Parser in frontend ignore in backend
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Formatted_Single_Value = "";//Parser in frontend ignore in backend
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Description = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TMap<FString, FString> Properties;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FNFTDetails_BE NFT_Details;

};

USTRUCT(BlueprintType)
struct FNFT_UData_BE//this is the unique data of an nft!
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString token_id = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString contract_address = "";
};


/*
* This data struct is specially crafted to maximally reduce the amount of 
* redundent data stored within the system and vastly improving the memory footprint!
*/
USTRUCT(BlueprintType)
struct FNFT_Master_BE
{
    GENERATED_USTRUCT_BODY()
public:
    //Duplicate Data that all NFT's of this type will have!
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* NFT_Icon = nullptr;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString NFT_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString NFT_Short_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Collection_Long_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Collection_Short_Name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* Collection_Icon = nullptr;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float Value = 0.0;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString Description = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TMap<FString, FString> Properties;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FNFTDetails_BE NFT_Details;//we need also these details EXCEPT token ID and Contract addr in here
    //we can get the correct contract & token_id hashes from the FNFT_UData_BE list below!
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FNFT_UData_BE> nft_data;//this is the list of unique data for each NFT!
};
/*
* this is how coin based transactions can get stored in history!
*/
USTRUCT(BlueprintType)
struct FCoinTxn_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FCoin_BE coin;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float amount = 0.0;
};

/*
* this is how nft based txn's are stored in history!
*/
USTRUCT(BlueprintType)
struct FNFTTxn_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FNFT_BE nft;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int32 amount = -1;//can't have half a card so int32 maybe 64?
};

/*
* this is how a single transaction is stored
* we build out the specifics of date time etc,
* then store a list of coinTXN's and tokenTXN's as members of this struct
*/
USTRUCT(BlueprintType)
struct FTransactionHistoryItem_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString network_name = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* network_icon = nullptr;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString other_public_address = "";//the other person involved in the transaction
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* other_icon = nullptr;//the other persons icon
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TEnumAsByte<ETXNType> transaction_type = ETXNType::TXN_Receive;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FMonthDayYear_BE transaction_date;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FCoinTxn_BE> txn_history_coins;//the list of coins in the txn history item!
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FNFTTxn_BE> txn_history_nfts;//the list of nfts in the txn history item!
};

/*
* Used to store fee data that the user
* must pay if they wish to conduct an on chain txn
*/
USTRUCT(BlueprintType)
struct FFee_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FCoin_BE fee;//here we use FCoin_BE struct BUT for coin amount is the fee amount. everything else is the same!
};

USTRUCT(BlueprintType)
struct FUserData_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString username = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString email = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int32 account_id = -1;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString public_address = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        UTexture2D* profile_image = nullptr;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString email_service = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FContact_BE> contacts;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        bool hide_unlisted_tokens = false;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        bool hide_unlisted_collectibles = false;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FNFT_Master_BE> nfts;//we send the NFT_Master_BE to initialize NFT data upfront!
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FCoin_BE> coins;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FNetwork_BE> networks;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FTransactionHistoryItem_BE> transaction_history;
};

/*
* I keep session data seperate from user data in this case
* as when we hit the frontend this needs to get seperated anyway
*/
USTRUCT(BlueprintType)
struct FSystemData_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FUserData_BE user_data;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FActiveSession_BE> active_sessions_list;//the first item in this list is the current session!
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FSystemSession_BE> system_sessions_list;//the first item in this list is the current system session!
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FSelectableCurrency_BE> currency_list;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FSelectableCurrency_BE selected_currency;//the selected currency the user is currently using!
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FFee_BE> fee_list;
};

USTRUCT(BlueprintType)
struct FStoredState_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString public_address = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString email = "";//not sure if we actually need this
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FActiveSession_BE> active_Sessions;//we may only need the one session?
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FSystemSession_BE> system_sessions;
};

/*
* Used by the frontend to keep track of a pending txn's
* state! The idea is when a txn is initiated the UI will update to a 
* state where it will assume the TXN went through. A request will go out
* to the backend with this TXN data gathered from the UI / backend requests for getting a txn hash!
* then at some point in the future the backend will let us know whether to commit the TXN if it went
* through OR Undo the TXN if it failed to process
*/
USTRUCT(BlueprintType)
struct FPendingTxn_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString txn_hash_id = "";//we use this to uniquely identify this transaction!
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float value = false;//true state value of the txn for a send this should be negative!
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FNFT_UData_BE> nft_u_data_list;
};


/*
* Used to let the frontend know what the final state of a given pending txn is!
*/
USTRUCT(BlueprintType)
struct FTxnCallback_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString txn_hash_id = "";//the unique txn_id_hash of a pending txn!
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        bool good_txn = false;//whether our txn went through or not!
};

/*
* This is specific for NFT / Token based transactions
* this will get paired off with a dedicated call in the Sequence_Backend_Manager.h/.cpp
* for initiating the transaction
*/
USTRUCT(BlueprintType)
struct FNFT_Send_Txn_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString txn_hash_id = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString contact_public_addr = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        int32 amount = -1;//nft's deal in postive int's S.T. {Z^+}/{0}
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FFee_BE selected_fee;//the fee the user's chose to pay to for this TXN
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TEnumAsByte<EContractType> item_type = EContractType::ERC1155;//the type of item we are transacting on
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TArray<FNFT_UData_BE> nft_u_data;//for nft based txn's! we need this too!
};

USTRUCT(BlueprintType)
struct FCoin_Send_Txn_BE
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FString txn_hash_id = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        float amount = false;//Coins deal in Positve non Zero Real Numbers {R^+}/{0.0}
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        FFee_BE selected_fee;//the fee the user's chose to pay to for this TXN
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
        TEnumAsByte<EContractType> item_type = EContractType::ERC1155;//the type of item we are transacting on
};