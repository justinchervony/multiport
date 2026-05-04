#ifndef __ASHITA_Multiport_H_INCLUDED__
#define __ASHITA_Multiport_H_INCLUDED__

#include "Ashita.h"
#include "Output.h"
#include "safePacketInjector.h"
#include "Utilities.h"
#include "HPTable.h"

class Multiport : IPlugin
{
private:
    IAshitaCore* m_AshitaCore;
    ILogManager* m_LogManager;
    uint32_t m_PluginId;
    OutputHelpers* pOutput;
    safePacketInjector* pPacket;

    bool m_pendingSelection;
    bool m_pendingEventEnd;
    uint16_t m_pendingIndex;
    uint16_t m_pendingActIndex;
    uint32_t m_pendingUniqueNo;
    uint32_t m_pendingSelectionTick;
    uint32_t m_pendingEndTick;
    uint32_t m_tickCount;

    uint32_t m_pendingCrystalUniqueNo;
    uint16_t m_pendingCrystalActIndex;

    bool m_isFollower;
    uint32_t m_homepointMasks[4];

public:
    const char* GetName(void) const override { return "Multiport"; }
    const char* GetAuthor(void) const override { return "Ushikai"; }
    const char* GetDescription(void) const override { return "Syncs homepoint teleports across multibox clients."; }
    const char* GetLink(void) const override { return ""; }
    double GetVersion(void) const override { return 1.0f; }
    int32_t GetPriority(void) const override { return 0; }
    uint32_t GetFlags(void) const override { return (uint32_t)Ashita::PluginFlags::LegacyDirect3D; }

    bool Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id) override;
    void Release(void) override;
    bool HandleCommand(int32_t mode, const char* command, bool injected) override;
    bool HandleOutgoingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked) override;
    bool HandleIncomingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked) override;
    bool Direct3DInitialize(IDirect3DDevice8* device) override;
    void Direct3DPresent(const RECT* a, const RECT* b, HWND c, const RGNDATA* d) override;
    void TeleportToIndex(uint16_t index);
};

#endif