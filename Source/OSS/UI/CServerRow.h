#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CServerRow.generated.h"

class UTextBlock;
class UCMainMenu;
class UButton;

UCLASS()
class OSS_API UCServerRow : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(UCMainMenu* InParent, uint32 InIndex);

private:
	UFUNCTION()
	void OnClicked();
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HostUser;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ConnectionFraction;

	UPROPERTY(BlueprintReadOnly)
	bool bSelected;

private:
	UPROPERTY(meta = (BindWidget))
	UButton* RowButton;

private:
	UPROPERTY()
	UCMainMenu* Parent;

	uint32 SelfIndex;
};
