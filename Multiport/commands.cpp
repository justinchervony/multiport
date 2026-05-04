#include "Multiport.h"

bool Multiport::HandleCommand(int32_t mode, const char* command, bool injected)
{
    UNREFERENCED_PARAMETER(mode);
    UNREFERENCED_PARAMETER(injected);

    std::vector<std::string> args;
    int argcount = Ashita::Commands::GetCommandArgs(command, &args);

    if (_stricmp(args[0].c_str(), "/altport") != 0)
        return false;

    if (argcount < 2)
    {
        pOutput->message("Usage: /altport <index>");
        return true;
    }

    uint16_t index = (uint16_t)atoi(args[1].c_str());
    m_isFollower = injected;
    pOutput->message_f("altport received - index:%d isFollower:%s", index, injected ? "true" : "false");
    TeleportToIndex(index);
    return true;
}

void Multiport::TeleportToIndex(uint16_t index)
{
    pOutput->message_f("HP mask[0]:%08X [1]:%08X [2]:%08X [3]:%08X",
        m_homepointMasks[0], m_homepointMasks[1],
        m_homepointMasks[2], m_homepointMasks[3]);

    if (index >= g_HPTableSize)
    {
        pOutput->message_f("TeleportToIndex: index %d out of range (max %d)", index, g_HPTableSize - 1);
        m_isFollower = false;
        return;
    }

    // Check if homepoint is unlocked
    uint32_t word = m_homepointMasks[index / 32];
    if (!((1 << (index % 32)) & word))
    {
        pOutput->message_f("TeleportToIndex: homepoint %d is not unlocked on this character.", index);
        m_isFollower = false;
        return;
    }

    auto* entity = m_AshitaCore->GetMemoryManager()->GetEntity();
    auto* target = m_AshitaCore->GetMemoryManager()->GetTarget();

    uint16_t actIndex = (uint16_t)target->GetTargetIndex(0);
    uint32_t uniqueNo = entity->GetServerId(actIndex);
    uint32_t playerUniqueNo = entity->GetServerId(0);

    if (uniqueNo == 0 || actIndex == 0)
    {
        pOutput->message("TeleportToIndex: No valid target. Please target a homepoint crystal.");
        m_isFollower = false;
        return;
    }

    pOutput->message_f("TeleportToIndex: index:%d actIndex:%04X uniqueNo:%08X", index, actIndex, uniqueNo);

    // Send 0x001A - Interact with crystal
    uint8_t action[28] = { 0 };
    *(uint32_t*)(action + 4) = uniqueNo;
    *(uint16_t*)(action + 8) = actIndex;
    *(uint16_t*)(action + 10) = 0x0000;
    pPacket->addOutgoingPacket_s(0x001A, 28, action);

    // Store values for delayed sends in Direct3DPresent
    m_pendingActIndex = actIndex;
    m_pendingUniqueNo = playerUniqueNo;
    m_pendingIndex = index;
    m_pendingSelection = true;
    m_pendingSelectionTick = m_tickCount + 30;
    m_pendingCrystalUniqueNo = uniqueNo;
    m_pendingCrystalActIndex = actIndex;
}

bool Multiport::HandleOutgoingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked)
{
    UNREFERENCED_PARAMETER(size);
    UNREFERENCED_PARAMETER(modified);
    UNREFERENCED_PARAMETER(sizeChunk);
    UNREFERENCED_PARAMETER(dataChunk);
    UNREFERENCED_PARAMETER(blocked);

    if (injected)
        return false;

    if (id == 0x05B)
    {
        pOutput->message_f("0x05B UniqueNo:%08X EndPara:%08X ActIndex:%04X Mode:%04X EventNum:%04X EventPara:%04X",
            *(uint32_t*)(data + 4), *(uint32_t*)(data + 8),
            *(uint16_t*)(data + 12), *(uint16_t*)(data + 14),
            *(uint16_t*)(data + 16), *(uint16_t*)(data + 18));

        if (data[8] != 0x02)
            return false;

        uint16_t index = *(uint16_t*)(data + 10);
        pOutput->message_f("0x05B confirm caught - index:%d", index);

        if (!m_isFollower) {
            char cmd[64];
            sprintf_s(cmd, "/mso /altport %d", index);
            m_AshitaCore->GetChatManager()->QueueCommand(1, cmd);
        }
        else {
            pOutput->message_f("0x05B confirm suppressed on follower - index:%d", index);
        }
        m_isFollower = false;
    }

    return false;
}

bool Multiport::HandleIncomingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked)
{
    UNREFERENCED_PARAMETER(modified);
    UNREFERENCED_PARAMETER(sizeChunk);
    UNREFERENCED_PARAMETER(dataChunk);
    UNREFERENCED_PARAMETER(injected);
    UNREFERENCED_PARAMETER(blocked);

    // Cache homepoint masks regardless of follower state
    if (id == 0x063)
    {
        uint16_t type = *(uint16_t*)(data + 4);
        if (type == 0x06)
        {
            memcpy(m_homepointMasks, data + 8, sizeof(m_homepointMasks));
            pOutput->message_f("Homepoint masks cached.");
        }

        // Only block the menu packet on followers
        if (m_isFollower)
            return true;
    }

    if (m_isFollower && id == 0x034)
        return true;

    return false;
}

bool Multiport::Direct3DInitialize(IDirect3DDevice8* device)
{
    UNREFERENCED_PARAMETER(device);
    return true;
}

void Multiport::Direct3DPresent(const RECT* a, const RECT* b, HWND c, const RGNDATA* d)
{
    UNREFERENCED_PARAMETER(a);
    UNREFERENCED_PARAMETER(b);
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(d);

    m_tickCount++;

    if (m_pendingSelection && m_tickCount >= m_pendingSelectionTick)
    {
        // Second send 0x001A - Interact with crystal again
        uint8_t action[28] = { 0 };
        *(uint32_t*)(action + 4) = m_pendingCrystalUniqueNo;
        *(uint16_t*)(action + 8) = m_pendingCrystalActIndex;
        *(uint16_t*)(action + 10) = 0x0000;
        pPacket->addOutgoingPacket_s(0x001A, 28, action);

        m_pendingSelection = false;

        uint8_t selection[20] = { 0 };
        *(uint32_t*)(selection + 4) = m_pendingUniqueNo;
        *(uint32_t*)(selection + 8) = 0x00000008;
        *(uint16_t*)(selection + 12) = m_pendingActIndex;
        *(uint16_t*)(selection + 14) = 0x0001; // Mode
        *(uint16_t*)(selection + 16) = (uint16_t)m_AshitaCore->GetMemoryManager()->GetEntity()->GetZoneId(0);
        *(uint16_t*)(selection + 18) = 0x21FC;

        pOutput->message_f("Sending selection 0x05B for index:%d", m_pendingIndex);
        pPacket->addOutgoingPacket_s(0x05B, 20, selection);

        m_pendingEventEnd = true;
        m_pendingEndTick = m_tickCount + 1;
    }

    if (m_pendingEventEnd && m_tickCount >= m_pendingEndTick)
    {
        m_pendingEventEnd = false;

        uint8_t eventend[20] = { 0 };
        *(uint32_t*)(eventend + 4) = m_pendingUniqueNo;
        *(uint32_t*)(eventend + 8) = (uint32_t)m_pendingIndex << 16 | 0x0002;
        *(uint16_t*)(eventend + 12) = m_pendingActIndex;
        *(uint16_t*)(eventend + 14) = 0x0000; // Mode should be 0 not 1
        *(uint16_t*)(eventend + 16) = (uint16_t)m_AshitaCore->GetMemoryManager()->GetEntity()->GetZoneId(0);
        *(uint16_t*)(eventend + 18) = 0x21FC;

        pOutput->message_f("Sending confirm 0x05B for index:%d", m_pendingIndex);
        pPacket->addOutgoingPacket_s(0x05B, 20, eventend);

        m_isFollower = false; // Sequence complete, restore manual control
    }
}