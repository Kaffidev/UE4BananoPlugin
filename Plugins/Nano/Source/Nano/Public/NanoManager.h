#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "NanoTypes.h"
#include "NanoWebsocket.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformMisc.h"
#include "Runtime/Online/HTTP/Public/Http.h"

#include <functional>
#include <string>
#include <unordered_map>

#include "NanoManager.generated.h"

class PrvKeyAutomateDelegate {
public:
	PrvKeyAutomateDelegate() = default;
	PrvKeyAutomateDelegate(const FString& prv_key, const FAutomateResponseReceivedDelegate& delegate, const FString& minimum)
		: prv_key(prv_key), delegate(delegate), minimum(minimum) {
	}

	PrvKeyAutomateDelegate(const PrvKeyAutomateDelegate&) = delete;
	PrvKeyAutomateDelegate& operator=(const PrvKeyAutomateDelegate&) = delete;

	FString prv_key;
	FAutomateResponseReceivedDelegate delegate;
	FString minimum;
	FTimerHandle timerHandle;
};

// Use for send/receive block listeners
template <class ResponseData, class ResponseReceiveDelegate>
class BlockListenerDelegate {
public:
	BlockListenerDelegate() = default;
	BlockListenerDelegate(const ResponseData& data, const ResponseReceiveDelegate& delegate_) : delegate(delegate_), data(data) {
	}

	BlockListenerDelegate(const BlockListenerDelegate&) = delete;
	BlockListenerDelegate& operator=(const BlockListenerDelegate&) = delete;

	ResponseReceiveDelegate delegate;
	ResponseData data;
	FTimerHandle timerHandle;
};

struct ListeningPayment {
	FListenPaymentDelegate delegate;
	FString account;
	FString amount;
	FTimerHandle timerHandle;
};

UCLASS(BlueprintType, Blueprintable)
class NANO_API UNanoManager : public UObject {
	GENERATED_BODY()

public:
	// Gets confirmed account balance and pending block balance
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void GetWalletBalance(FGetBalanceResponseReceivedDelegate delegate, FString nanoAddress);

	// Generate work for this block hash
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void WorkGenerate(FWorkGenerateResponseReceivedDelegate delegate, FString hash);

	// Process this block
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void Process(FProcessResponseReceivedDelegate delegate, FBlock block);

	// Process this block, only calls event when the block is confirmed.
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void ProcessWaitConfirmation(FProcessResponseReceivedDelegate delegate, FBlock block);

	// This relies on "request_nano" action being available on the rpc server
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void RequestNano(FReceivedNanoDelegate delegate, FString nanoAddress);

	// Get the frontier of this block. If the account doesn't exist, it will be filled in with some default values.
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void AccountFrontier(FAccountFrontierResponseReceivedDelegate delegate, FString account);

	// Get pending blocks for an account. Currently default to getting 5.
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void Pending(FPendingResponseReceivedDelegate delegate, FString account, FString threshold = "0", int32 maxCount = 100);

	// Check if this block hash is confirmed by the network
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void BlockConfirmed(FBlockConfirmedResponseReceivedDelegate delegate, FString hash);

	// Create a send block and publish it. Calls delegate if there's no errors but doesn't wait for confirmation on the network (see
	// SendWaitConfirmation)
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void Send(FProcessResponseReceivedDelegate delegate, FString const& private_key, FString const& account, FString const& amount);

	// Create a send block and publish it, only calls event when there is confirmation on the network.
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void SendWaitConfirmation(
		FProcessResponseReceivedDelegate delegate, FString const& private_key, FString const& account, FString const& amount);

	// Pass in a constructed send block and publish it, only calls event when there is confirmation on the network.
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void SendWaitConfirmationBlock(FProcessResponseReceivedDelegate delegate, FBlock block);

	// Can only be called once with this private key. A websocket registration will happen automatically
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void AutomaticallyPocketRegister(
		FAutomateResponseReceivedDelegate delegate, UNanoWebsocket* websocket, FString const& private_key, FString minimum = "0");

	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void AutomaticallyPocketUnregister(const FString& account, UNanoWebsocket* websocket);

	// Registers an account for watching on websocket (needed for a variety of other functions). If there are other watchers it will
	// just increment an id.
	UFUNCTION(BlueprintCallable, Category = "NanoManager", meta = (AutoCreateRefTerm = "delegate"))
	int32 Watch(const FWatchAccountReceivedDelegate& delegate, FString const& account, UNanoWebsocket* websocket);

	// Unregisters an account for watching on websocket. Will only remove from websocket if there are no other watchers.
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void Unwatch(FString const& account, const int32& id, UNanoWebsocket* websocket);

	// This internally sets up the listeners so that all the delegates requiring them get called. Very important!
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void SetupConfirmationMessageWebsocketListener(UNanoWebsocket* websocket);

	// Checks pending blocks for a payment of a certain amount.
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void SingleUseAccountListenForPaymentWaitConfirmation(
		FListenPaymentDelegate delegate, FString const& account, FString const& amount);

	// Utility to construct a send block
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void MakeSendBlock(FMakeBlockDelegate delegate, FString const& prvKey, FString const& amount, FString const& destination_account);

	// Utility to construct a receive block
	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void MakeReceiveBlock(FMakeBlockDelegate delegate, FString const& private_key, FString source_hash, FString const& amount);

	// Utility to receive a pending block
	UFUNCTION(BlueprintCallable, Category = "NanoManager", meta = (AutoCreateRefTerm = "delegate"))
	void Receive(
		const FProcessResponseReceivedDelegate& delegate, FString const& private_key, FString source_hash, FString const& amount);

	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void SetDataSubdirectory(FString const& subdir);

	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	FString GetPlainSeed(FString const& seedFileName, FString const& password) const;

	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	void SaveSeed(FString const& plainSeed, FString const& seedFileName, FString const& password);

	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	int32 GetNumSeedFiles() const;

	UFUNCTION(BlueprintCallable, Category = "NanoManager")
	TArray<FString> GetSeedFiles() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NanoManager")
	FString rpcUrl;

	UPROPERTY(EditAnywhere, Category = "NanoManager")
	FString defaultRepresentative{"nano_1iuz18n4g4wfp9gf7p1s8qkygxw7wx9qfjq6a9aq68uyrdnningdcjontgar"};

private:
	std::unordered_map<std::string, PrvKeyAutomateDelegate> keyDelegateMap;
	TMap<FString, TMap<int32, FWatchAccountReceivedDelegate>> watchers;
	int32 watcherId{0};
	std::unordered_map<std::string, BlockListenerDelegate<FProcessResponseData, FProcessResponseReceivedDelegate>> sendBlockListener;
	std::unordered_map<std::string, BlockListenerDelegate<FAutomateResponseData, FAutomateResponseReceivedDelegate>>
		receiveBlockListener;
	ListeningPayment listeningPayment;

	struct ReqRespJson {
		TSharedPtr<FJsonObject> request;
		TSharedPtr<FJsonObject> response;
	};

	static ReqRespJson GetResponseJson(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);
	static bool RequestResponseIsValid(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);

	void MakeRequest(TSharedPtr<FJsonObject> JsonObject,
		TFunction<void(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)> delegate);

	TSharedRef<IHttpRequest> CreateHttpRequest(TSharedPtr<FJsonObject> JsonObject);

	TSharedPtr<FJsonObject> GetAccountFrontierJsonObject(FString const& account);
	FAccountFrontierResponseData GetAccountFrontierResponseData(
		FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful) const;

	void AccountFrontier(
		FString account, TFunction<void(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)> const& d);

	TSharedPtr<FJsonObject> GetWorkGenerateJsonObject(FString hash);
	static FWorkGenerateResponseData GetWorkGenerateResponseData(
		FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);

	void WorkGenerate(FString hash, TFunction<void(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)> const& d);

	void Send(FString const& private_key, FString const& account, FString const& amount,
		TFunction<void(FProcessResponseData)> const& delegate);

	template <class T, class T1>
	void RegisterBlockListener(std::string const& account, T const& responseData,
		std::unordered_map<std::string, BlockListenerDelegate<T, T1>>& blockListener, T1 delegate);

	void MakeSendBlock(FString const& prvKey, FString const& amount, FString const& destination_account,
		TFunction<void(FMakeBlockResponseData)> const& delegate);

	TSharedPtr<FJsonObject> GetPendingJsonObject(FString account, FString threshold, int32 maxCount);
	static FPendingResponseData GetPendingResponseData(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);
	static FGetBalanceResponseData GetBalanceResponseData(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);
	static FProcessResponseData GetProcessResponseData(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);
	static FRequestNanoResponseData GetRequestNanoData(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);

	void BlockConfirmed(
		FString hash, TFunction<void(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)> const& d);
	TSharedPtr<FJsonObject> GetBlockConfirmedJsonObject(FString const& hash);
	static FBlockConfirmedResponseData GetBlockConfirmedResponseData(
		FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);

	void Pending(FString account, FString threshold, int32 maxCount,
		TFunction<void(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)> const& d);

	void Process(
		FBlock block, TFunction<void(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)> const& delegate);

	void AutomateWorkGenerateLoop(FAccountFrontierResponseData frontierData, TArray<FPendingBlock> pendingBlocks);
	void AutomatePocketPendingUtility(const FString& account, const FString& minimum);

	void GetFrontierAndFire(const FString& amount, const FString& hash, FString const& account, FConfType type);

	void GetFrontierAndFireWatchers(const FString& amount, const FString& hash, FString const& account, FConfType type);
	FAutomateResponseData GetWebsocketResponseData(const FString& amount, const FString& hash, FString const& account, FConfType type,
		FAccountFrontierResponseData const& frontierData);

	void MakeReceiveBlock(FString const& private_key, FString source_hash, FString const& amount,
		TFunction<void(FMakeBlockResponseData)> const& delegate);

	UFUNCTION()
	void OnConfirmationReceiveMessage(const FWebsocketConfirmationResponseData& data);

	FString getDefaultDataPath() const;
	FString dataPath{getDefaultDataPath()};
};
