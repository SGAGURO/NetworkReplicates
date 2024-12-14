#include "CServerRow.h"
#include "Components/Button.h"
#include "CMainMenu.h"

void UCServerRow::Setup(UCMainMenu* InParent, uint32 InIndex)
{
	Parent = InParent;
	SelfIndex = InIndex;

	RowButton->OnClicked.AddDynamic(this, &UCServerRow::OnClicked);
}

void UCServerRow::OnClicked()
{
	Parent->SetSelectedIndex(SelfIndex);
}
