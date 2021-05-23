#include "main.hpp"
#include "Settings.hpp"

#include "questui/shared/QuestUI.hpp"
#include "custom-types/shared/register.hpp"
#include "extern/config-utils/shared/config-utils.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "GlobalNamespace/ScoreController.hpp"
#include "GlobalNamespace/RelativeScoreAndImmediateRankCounter.hpp"
#include "GlobalNamespace/GameEnergyUIPanel.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "ModConfig.hpp"

#include "Server.hpp"

static ModInfo modInfo;
DEFINE_CONFIG(ModConfig);
using namespace GlobalNamespace;

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getLogger().info("Completed setup!");
}

static LevelStatsManager* manager = nullptr;

MAKE_HOOK_OFFSETLESS(UpdateScore, void, ScoreController* self)   {
    manager->combo = self->combo;
    UpdateScore(self);
}

MAKE_HOOK_OFFSETLESS(RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank, void, RelativeScoreAndImmediateRankCounter* self, int score, int modifiedscore, int maxscore, int maxmodfifiedscore) {
    RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank(self, score, modifiedscore, maxscore, maxmodfifiedscore);
    manager->score = modifiedscore;
    manager->rank = to_utf8(csstrtostr(RankModel::GetRankName(self->get_immediateRank())));
    manager->percentage = self->get_relativeScore();
}

MAKE_HOOK_OFFSETLESS(GameEnergyUIPanel_HandleGameEnergyDidChange, void, GameEnergyUIPanel* self, float energy) {
    GameEnergyUIPanel_HandleGameEnergyDidChange(self, energy);
    manager->energy = energy;
}

MAKE_HOOK_OFFSETLESS(SongStart, void,
        Il2CppObject* self,
        Il2CppString* gameMode,
        Il2CppObject* difficultyBeatmap,
        Il2CppObject* previewBeatmapLevel,
        Il2CppObject* overrideEnvironmentSettings,
        Il2CppObject* overrideColorScheme,
        Il2CppObject* gameplayModifiers,
        Il2CppObject* playerSpecificSettings,
        PracticeSettings* practiceSettings,
        Il2CppString* backButtonText,
        bool useTestNoteCutSoundEffects) {
    manager->difficulty = CRASH_UNLESS(il2cpp_utils::GetPropertyValue<int>(difficultyBeatmap, "difficulty"));
    SongStart(self, gameMode, difficultyBeatmap, previewBeatmapLevel, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects);
}

MAKE_HOOK_OFFSETLESS(RefreshContent, void, StandardLevelDetailView* self) {
    RefreshContent(self);
    

    
    IBeatmapLevel* level = self->level;
    manager->levelName = to_utf8(csstrtostr((Il2CppString*) CRASH_UNLESS(il2cpp_utils::GetPropertyValue(level, "songName"))));
    manager->levelSub = to_utf8(csstrtostr((Il2CppString*)CRASH_UNLESS(il2cpp_utils::GetPropertyValue(level, "songSubName"))));
    manager->songAuthor = to_utf8(csstrtostr((Il2CppString*) CRASH_UNLESS(il2cpp_utils::GetPropertyValue(level, "songAuthorName"))));
    manager->id = to_utf8(csstrtostr((Il2CppString*)CRASH_UNLESS(il2cpp_utils::GetPropertyValue(level, "levelID"))));
    bool CustomLevel = manager->id.find("custom_level_") != std::string::npos;
    manager->njs = CustomLevel ? self->selectedDifficultyBeatmap->get_noteJumpMovementSpeed() : 0.0f;
    manager->mapper = to_utf8(csstrtostr((Il2CppString*) CRASH_UNLESS(il2cpp_utils::GetPropertyValue(level, "levelAuthorName"))));
    manager->bpm = CRASH_UNLESS(il2cpp_utils::GetPropertyValue<float>(level, "beatsPerMinute"));
    //manager->njs = (float) CRASH_UNLESS(il2cpp_utils::GetPropertyValue(level, "noteJumpMovementSpeed"));
}

MAKE_HOOK_OFFSETLESS(AudioUpdate, void, Il2CppObject* self) {
    AudioUpdate(self);

    float time = CRASH_UNLESS(il2cpp_utils::RunMethodUnsafe<float>(self, "get_songTime"));
    float endTime = CRASH_UNLESS(il2cpp_utils::RunMethodUnsafe<float>(self, "get_songEndTime"));

    manager->timePlayed = (int) time;
    manager->totalTime = (int) endTime;
}

extern "C" void load() {
    getLogger().info("Installing hooks...");
    il2cpp_functions::Init();

    // Use this for Quest UI
    //QuestUI::Init();

    LoggerContextObject logger = getLogger().WithContext("load");

    // Register our mod settings menu
    //QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);

    // Install our hooks
    INSTALL_HOOK_OFFSETLESS(logger, UpdateScore, il2cpp_utils::FindMethodUnsafe("", "ScoreController", "Update", 0));
    INSTALL_HOOK_OFFSETLESS(logger, RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank, il2cpp_utils::FindMethodUnsafe("", "RelativeScoreAndImmediateRankCounter", "UpdateRelativeScoreAndImmediateRank", 4));
    INSTALL_HOOK_OFFSETLESS(logger, GameEnergyUIPanel_HandleGameEnergyDidChange, il2cpp_utils::FindMethodUnsafe("", "GameEnergyUIPanel", "HandleGameEnergyDidChange", 1));
    INSTALL_HOOK_OFFSETLESS(logger, SongStart, il2cpp_utils::FindMethodUnsafe("", "StandardLevelScenesTransitionSetupDataSO", "Init", 10));
    INSTALL_HOOK_OFFSETLESS(logger, RefreshContent, il2cpp_utils::FindMethodUnsafe("", "StandardLevelDetailView", "RefreshContent", 0));
    INSTALL_HOOK_OFFSETLESS(logger, AudioUpdate, il2cpp_utils::FindMethodUnsafe("", "AudioTimeSyncController", "Update", 0));
    getLogger().info("Installed all hooks!");

    manager = new LevelStatsManager(getLogger());
}