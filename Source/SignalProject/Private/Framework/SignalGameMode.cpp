#include "Framework/SignalGameMode.h"

#include "Player/SignalPlayerCharacter.h"
#include "Player/SignalPlayerController.h"

ASignalGameMode::ASignalGameMode()
{
    PlayerControllerClass = ASignalPlayerController::StaticClass();
    DefaultPawnClass = ASignalPlayerCharacter::StaticClass();
}
