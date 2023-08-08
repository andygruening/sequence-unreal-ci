#pragma once

#include "CoreMinimal.h"
#include "AttributeMap.h"
#include "JsonObjectConverter.h"
#include "IndexerSupport.h"
#include "TokenMetaData.generated.h"

USTRUCT(BlueprintType)
struct FTokenMetaData
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int64 tokenId;
    UPROPERTY()
        FString contractAddress;
    UPROPERTY()
        FString name;
    UPROPERTY()
        FString description;
    UPROPERTY()
        FString image;
    UPROPERTY()
        float decimals;
    UPROPERTY()
        FString video;
    UPROPERTY()
        FString audio;
    UPROPERTY()
        FString image_data;
    UPROPERTY()
        FString external_url;
    UPROPERTY()
        FString background_color;
    UPROPERTY()
        FString animation_url;

    TMap<FString, FString> properties;

    TArray<FAttributeMap> attributes;

    //used for testing!
    TSharedPtr<FJsonObject> Get()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetNumberField("tokenId", tokenId);
        ret.Get()->SetStringField("contractAddress", contractAddress);
        ret.Get()->SetStringField("name", name);
        ret.Get()->SetStringField("description", description);
        ret.Get()->SetStringField("image", image);
        ret.Get()->SetNumberField("decimals", decimals);
        ret.Get()->SetStringField("video", video);
        ret.Get()->SetStringField("audio", audio);
        ret.Get()->SetStringField("image_data", image_data);
        ret.Get()->SetStringField("external_url", external_url);
        ret.Get()->SetStringField("background_color", background_color);
        ret.Get()->SetStringField("animation_url", animation_url);

        //need to build properties object now!
        TSharedPtr<FJsonObject> propObj = MakeShareable<FJsonObject>(new FJsonObject);

        TArray<FString> keys;
        properties.GetKeys(keys);
        for (FString key : keys)
        {
            propObj.Get()->SetStringField(key,*properties.Find(key));
        }

        ret.Get()->SetObjectField("properties", propObj);

        //need to build attribute list now! easier to work this in raw string form then UE json
        FString attributeString = "[";
        for (FAttributeMap aMap : attributes)
        {
            attributeString.Append(UIndexerSupport::simplifyString(aMap.Get()));
            attributeString.Append(",");
        }
        if (attributes.Num() > 0)
        {
            attributeString.RemoveAt(attributeString.Len() - 1);//remove the last index as it'll be a wrongly placed ,
        }
        attributeString.Append("]");
        ret.Get()->SetStringField("attributes", attributeString);

        return ret;
    };

    void setup(FJsonObject json_in)
    {//the json object we expect here will be a mirror of what is above EXCEPT we will be snipping out what we need!
        const TSharedPtr<FJsonObject>* refPtr;
        if (json_in.TryGetObjectField("properties", refPtr))
        {
            properties = UIndexerSupport::jsonObjectParser(*refPtr);
        }

        //now we create the attribute map!
        const TArray<TSharedPtr<FJsonValue>>* lstPtr;
        if (json_in.TryGetArrayField("attributes",lstPtr))//attempt to assign!
        {
            for (TSharedPtr<FJsonValue> i : *lstPtr)
            {
                //we need to convert i to an object! then will be able to proceed!
                const TSharedPtr<FJsonObject>* iObjPtr;
                if (i.Get()->TryGetObject(iObjPtr))
                {//failed to convert
                    FAttributeMap attr;
                    attr.setup(*iObjPtr);
                    attributes.Add(attr);
                }
            }
        }
    }
};