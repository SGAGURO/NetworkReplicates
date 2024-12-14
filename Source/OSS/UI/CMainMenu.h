#pragma once

#include "CoreMinimal.h"
#include "CMenuBase.h"
#include "CMainMenu.generated.h"

class UButton;
class UWidgetSwitcher;
class UWidget;
class UPanelWidget;
class UEditableTextBox;

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

public:
	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUserName;
};

UCLASS()
class OSS_API UCMainMenu : public UCMenuBase
{
	GENERATED_BODY()

public:
	UCMainMenu();

protected:
	virtual bool Initialize() override;

private:
	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void OpenHostMenu();

	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void QuitGame();

public:
	void SetServerList(TArray<FServerData> InServerDatas);
	void SetSelectedIndex(uint32 InIndex);

private:
	//Menu Switcher
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	UWidget* JoinMenu;

	//Main Menu
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	//Join Menu
	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ServerList;

	TSubclassOf<UUserWidget> ServerRowClass;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelJoinButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmJoinButton;

	//Host Menu
	UPROPERTY(meta = (BindWidget))
	UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* ServerHostName;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelHostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmHostButton;

private:
	TOptional<uint32> SelectedIndex;
};
