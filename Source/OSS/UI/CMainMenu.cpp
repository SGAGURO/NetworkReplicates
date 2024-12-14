#include "CMainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "CServerRow.h"

UCMainMenu::UCMainMenu()
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowClass_Asset(TEXT("/Game/UI/WB_ServerRow"));
	if (ServerRowClass_Asset.Succeeded())
	{
		ServerRowClass = ServerRowClass_Asset.Class;
	}
}


bool UCMainMenu::Initialize()
{
	bool bSuccess = Super::Initialize();
	if (!bSuccess) return false;
	
	//Main Menu
	if (!HostButton) return false;
	HostButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenHostMenu);

	if (!JoinButton) return false;
	JoinButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenJoinMenu);

	if (QuitButton == nullptr) return false;
	QuitButton->OnClicked.AddDynamic(this, &UCMainMenu::QuitGame);

	//Join Menu
	if (!CancelJoinButton) return false;
	CancelJoinButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenMainMenu);

	if (ConfirmJoinButton == nullptr) return false;
	ConfirmJoinButton->OnClicked.AddDynamic(this, &UCMainMenu::JoinServer);

	//Host Menu
	if (CancelHostButton == nullptr) return false;
	CancelHostButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenMainMenu);

	if (ConfirmHostButton == nullptr) return false;
	ConfirmHostButton->OnClicked.AddDynamic(this, &UCMainMenu::HostServer);

	return true;
}

void UCMainMenu::HostServer()
{
	FString ServerName = ServerHostName->Text.ToString();
	OwningInstance->Host(ServerName);
}

void UCMainMenu::OpenMainMenu()
{
	if (!MenuSwitcher) return;
	if (!MainMenu) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UCMainMenu::OpenJoinMenu()
{
	if (!MenuSwitcher) return;
	if (!JoinMenu) return;
	MenuSwitcher->SetActiveWidget(JoinMenu);

	if (OwningInstance)
	{
		OwningInstance->RefreshServerList();
	}
}

void UCMainMenu::OpenHostMenu()
{
	if (!MenuSwitcher) return;
	if (!MainMenu) return;
	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UCMainMenu::JoinServer()
{
	if (SelectedIndex.IsSet() && OwningInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index is %d."), SelectedIndex.GetValue());

		OwningInstance->Join(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index is not set."));
	}
}

void UCMainMenu::QuitGame()
{
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;

	PC->ConsoleCommand("quit");
}

void UCMainMenu::SetServerList(TArray<FServerData> InServerDatas)
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (!ServerList) return;
	ServerList->ClearChildren();

	uint32 i = 0;
	for (const FServerData& ServerData : InServerDatas)
	{
		UCServerRow* ServerRow = CreateWidget<UCServerRow>(World, ServerRowClass);
		if (!ServerRow) return;

		ServerRow->ServerName->SetText(FText::FromString(ServerData.Name));
		ServerRow->HostUser->SetText(FText::FromString(ServerData.HostUserName));
		
		FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		ServerRow->ConnectionFraction->SetText(FText::FromString(FractionText));

		ServerRow->Setup(this, i++);

		ServerList->AddChild(ServerRow);
	}

}

void UCMainMenu::SetSelectedIndex(uint32 InIndex)
{
	SelectedIndex = InIndex;

	for (int32 i = 0; i < ServerList->GetChildrenCount(); i++)
	{
		auto serverRow = Cast<UCServerRow>(ServerList->GetChildAt(i));
		if (serverRow)
		{
			serverRow->bSelected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
		}
	}
}
