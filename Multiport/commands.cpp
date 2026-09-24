#include "Multiport.h"

bool Multiport::HandleCommand(int32_t mode, const char* command, bool injected)
{
    UNREFERENCED_PARAMETER(mode);
    UNREFERENCED_PARAMETER(injected);

    std::vector<std::string> args;
    int argcount = Ashita::Commands::GetCommandArgs(command, &args);

    if (argcount < 1 || args.empty())
        return false;

    if (_stricmp(args[0].c_str(), "/multiport") != 0 && _stricmp(args[0].c_str(), "/mp") != 0)
        return false;

    if (argcount >= 2 && _stricmp(args[1].c_str(), "stop") == 0)
    {
        m_pendingSelection = false;
        m_pendingEventEnd = false;
        m_pendingFollowerClear = false;
        m_pendingZoneConfirm = false;
        m_retryCount = 0;
        m_isFollower = false;
        pOutput->message("Multiport: teleport sequence cancelled.");
        return true;
    }

    if (argcount < 2)
    {
        pOutput->message("Usage: /multiport <index>");
        return true;
    }

    if (argcount >= 2 && _stricmp(args[1].c_str(), "debug") == 0)
    {
        m_debugMode = !m_debugMode;
        pOutput->message_f("Debug mode %s", m_debugMode ? "enabled" : "disabled");
        return true;
    }

    if (!injected)
    {
        pOutput->message("Use the homepoint crystal directly. /multiport is for alts only.");
        return true;
    }

    uint16_t index = (uint16_t)atoi(args[1].c_str());
    if (argcount >= 3)
        m_lastEventPara = (uint16_t)strtol(args[2].c_str(), nullptr, 16);

    m_isFollower = injected;
    if (m_debugMode) {
        pOutput->message_f("multiport received - index:%d isFollower:%s", index, injected ? "true" : "false");
    }
    TeleportToIndex(index);
    return true;
}

void Multiport::TeleportToIndex(uint16_t index, bool isRetry)
{
    m_pendingSelection = false;
    m_pendingEventEnd = false;
    m_pendingFollowerClear = false;
    m_pendingZoneConfirm = false;
    m_broadcastSent = false;
    m_sameZoneSent = false;

    uint16_t actIndex = 0;
    uint32_t uniqueNo = 0;
    uint32_t playerUniqueNo = 0;

    auto* entity = m_AshitaCore->GetMemoryManager()->GetEntity();
    auto* target = m_AshitaCore->GetMemoryManager()->GetTarget();

    if (!isRetry) {
        m_retryCount = 0;
        m_retryIndex = index;

        actIndex = (uint16_t)target->GetTargetIndex(0);
        uniqueNo = entity->GetServerId(actIndex);
        playerUniqueNo = (uint32_t)m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberServerId(0);

        if (uniqueNo == 0 || actIndex == 0)
        {
            pOutput->message("TeleportToIndex: No valid target. Please target a homepoint crystal.");
            m_isFollower = false;
            return;
        }
    }

    else {
        actIndex = m_pendingActIndex;
        uniqueNo = m_pendingCrystalUniqueNo;
        playerUniqueNo = m_pendingUniqueNo;
    }

    if (m_debugMode) {
        pOutput->message_f("TeleportToIndex: index:%d actIndex:%04X uniqueNo:%08X", index, actIndex, uniqueNo);
    }

    // Send 0x001A - Interact with crystal
    uint8_t action[28] = { 0 };
    *(uint32_t*)(action + 4) = uniqueNo;
    *(uint16_t*)(action + 8) = actIndex;
    *(uint16_t*)(action + 10) = 0x0000;
    pPacket->addOutgoingPacket_s(0x001A, 28, action);
    if (m_debugMode) {
        pOutput->message_f("Sending 0x001A: actIndex:%04X uniqueNo:%08X", actIndex, uniqueNo);
    }

    // Store values for delayed sends in Direct3DPresent
    m_pendingActIndex = actIndex;
    m_pendingUniqueNo = playerUniqueNo;
    m_pendingIndex = index;
    uint16_t currentZone = (uint16_t)m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberZone(0);
    m_sameZoneTeleport = (g_HPTable[index].zone == currentZone);
    m_pendingCrystalUniqueNo = uniqueNo;
    m_pendingCrystalActIndex = actIndex;

    if (m_debugMode) {
        pOutput->message_f("sameZone:%s currentZone:%d targetZone:%d",
            m_sameZoneTeleport ? "true" : "false", currentZone, g_HPTable[index].zone);
    }

    // Same-zone followers are driven entirely by the incoming-0x052 handler in
    // HandleIncomingPacket, which sends 0x05C + confirm using the crystal/index data set
    // above. Arming the generic tick-driven second interact + selection below would race
    // that handler and re-trigger the crystal menu on the alt, so skip it here.
    if (m_isFollower && m_sameZoneTeleport)
        return;

    m_pendingSelection = true;
    const char* name = m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberName(0);
    uint32_t nameHash = 0;
    for (int i = 0; name[i] != '\0'; i++)
        nameHash = nameHash * 31 + name[i];
    uint32_t staggerOffset = (nameHash % 16) * 15;

    if (m_debugMode) {
        pOutput->message_f("Set m_pendingIndex to %d", m_pendingIndex);
        pOutput->message_f("Stagger: name:%s hash:%u offset:%d finalTick:%d",
            name, nameHash, staggerOffset, m_tickCount + 30 + staggerOffset);
    }
    m_pendingSelectionTick = m_tickCount + 30 + staggerOffset;
}

bool Multiport::HandleOutgoingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked)
{
    UNREFERENCED_PARAMETER(modified);
    UNREFERENCED_PARAMETER(sizeChunk);
    UNREFERENCED_PARAMETER(dataChunk);
    UNREFERENCED_PARAMETER(blocked);

    if (injected)
        return false;

    if (id == 0x05B && *(uint16_t*)(data + 14) == 0x0001)
        m_broadcastSent = false;

    if (m_debugMode && id == 0x05B)
        pOutput->message_f("0x05B out: UniqueNo:%08X EndPara:%08X ActIndex:%04X Mode:%04X EventNum:%04X EventPara:%04X",
            *(uint32_t*)(data + 4), *(uint32_t*)(data + 8),
            *(uint16_t*)(data + 12), *(uint16_t*)(data + 14),
            *(uint16_t*)(data + 16), *(uint16_t*)(data + 18));

    // Real destination selection (Mode 0x0001, EndPara low word 0x0002): capture the
    // index here for the eventual broadcast. The completion packet below never carries it.
    if (id == 0x05B && data[8] == 0x02 && *(uint16_t*)(data + 14) == 0x0001)
    {
        m_pendingBroadcastIndex = *(uint16_t*)(data + 10);
    }

    // Completion / broadcast trigger. Cross-zone completions carry data[8]==0x02 with
    // EndPara identical to the selection packet (only Mode flips 0x0001 -> 0x0000).
    // Same-zone completions carry data[8]==0x03 with EndPara zeroed out (no index).
    // Both broadcast using the index captured from the selection packet above.
    if (id == 0x05B && (data[8] == 0x02 || data[8] == 0x03) && *(uint16_t*)(data + 14) == 0x0000)
    {
        if (m_broadcastSent)
            return false;
        m_broadcastSent = true;
        m_lastEventPara = *(uint16_t*)(data + 18);
        if (!m_isFollower)
        {
            if (m_debugMode) {
                pOutput->message_f("Completion 0x05B (data[8]=%02X) - broadcasting index:%d", data[8], m_pendingBroadcastIndex);
            }
            char cmd[64];
            sprintf_s(cmd, "/mso /multiport %d %04X", m_pendingBroadcastIndex, m_lastEventPara);
            m_AshitaCore->GetChatManager()->QueueCommand(1, cmd);
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

    if (m_debugMode && m_isFollower)
        pOutput->message_f("IN id:%03X size:%d", id, size);

    if (m_debugMode && id == 0x05B)
        pOutput->message_f("Incoming 0x05B - isFollower:%s sameZone:%s",
            m_isFollower ? "true" : "false",
            m_sameZoneTeleport ? "true" : "false");

    if (id == 0x00A && m_pendingZoneConfirm)
    {
        uint16_t zoneNo = *(uint16_t*)(data + 48);
        if (m_debugMode) pOutput->message_f("0x00A zone:%d target:%d", zoneNo, g_HPTable[m_retryIndex].zone);
        if (zoneNo == g_HPTable[m_retryIndex].zone)
        {
            m_pendingZoneConfirm = false;
            m_retryCount = 0;
            if (m_debugMode) pOutput->message_f("Zone confirmed - teleport successful.");
        }
    }

    if (id == 0x063)
    {
        if (m_debugMode)
            pOutput->message_f("0x063 received - isFollower:%s", m_isFollower ? "true" : "false");

        uint16_t type = *(uint16_t*)(data + 4);
        if (type == 0x06)
        {
            memcpy(m_homepointMasks, data + 8, sizeof(m_homepointMasks));
            if (m_debugMode)
                pOutput->message_f("Masks: %08X %08X %08X %08X",
                    m_homepointMasks[0], m_homepointMasks[1],
                    m_homepointMasks[2], m_homepointMasks[3]);
        }

        if (m_isFollower)
            return true;
    }

    if (m_isFollower && id == 0x034)
        return true;

    if (m_isFollower && id == 0x052 && m_sameZoneTeleport && !m_sameZoneSent)
    {
        m_sameZoneSent = true;
        auto* entity = m_AshitaCore->GetMemoryManager()->GetEntity();

        if (m_debugMode) {
            pOutput->message_f("Pending location for teleport: x:%.2f y:%.2f z:%.2f index:%d",
                g_HPTable[m_pendingIndex].x,
                g_HPTable[m_pendingIndex].y,
                g_HPTable[m_pendingIndex].z,
                m_pendingIndex);
        }

        // Send 0x05C first
        uint8_t warp[32] = { 0 };
        *(float*)(warp + 4) = g_HPTable[m_pendingIndex].x;
        *(float*)(warp + 8) = g_HPTable[m_pendingIndex].y;
        *(float*)(warp + 12) = g_HPTable[m_pendingIndex].z;
        *(uint32_t*)(warp + 16) = m_pendingUniqueNo;
        *(uint32_t*)(warp + 20) = (uint32_t)m_pendingIndex << 16 | 0x0003;
        *(uint16_t*)(warp + 24) = (uint16_t)m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberZone(0);
        *(uint16_t*)(warp + 26) = m_lastEventPara;
        *(uint16_t*)(warp + 28) = m_pendingActIndex;
        *(uint8_t*)(warp + 30) = 0x01;
        *(int8_t*)(warp + 31) = (int8_t)g_HPTable[m_pendingIndex].rot;
        pPacket->addOutgoingPacket_s(0x05C, 32, warp);

        // Then send 0x05B confirm
        uint8_t eventend[20] = { 0 };
        *(uint32_t*)(eventend + 4) = m_pendingUniqueNo;
        *(uint32_t*)(eventend + 8) = 0x00000003;
        *(uint16_t*)(eventend + 12) = m_pendingActIndex;
        *(uint16_t*)(eventend + 14) = 0x0000;
        *(uint16_t*)(eventend + 16) = (uint16_t)m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberZone(0);
        *(uint16_t*)(eventend + 18) = m_lastEventPara;
        pPacket->addOutgoingPacket_s(0x05B, 20, eventend);

        if (m_debugMode)
            pOutput->message_f("Sent 0x05C + 0x05B confirm for same-zone");

        // The generic Direct3DPresent flow is skipped for same-zone followers (see
        // TeleportToIndex), so schedule the follower-clear here instead of relying on
        // m_pendingEventEnd, which never gets armed on this path.
        m_pendingFollowerClear = true;
        m_pendingFollowerClearTick = m_tickCount + 600;

        return true;
    }

    if (m_isFollower && id == 0x05B)
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
        if (m_debugMode) {
            pOutput->message_f("Sending 0x001A: actIndex:%04X uniqueNo:%08X", m_pendingCrystalActIndex, m_pendingCrystalUniqueNo);
        }

        m_pendingSelection = false;

        uint8_t selection[20] = { 0 };
        *(uint32_t*)(selection + 4) = m_pendingUniqueNo;
        *(uint32_t*)(selection + 8) = m_sameZoneTeleport ? ((uint32_t)m_pendingIndex << 16 | 0x0002) : 0x00000008;
        *(uint16_t*)(selection + 12) = m_pendingActIndex;
        *(uint16_t*)(selection + 14) = 0x0001;
        *(uint16_t*)(selection + 16) = (uint16_t)m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberZone(0);
        *(uint16_t*)(selection + 18) = m_lastEventPara;

        if (m_debugMode) {
            pOutput->message_f("Sending selection 0x05B for index:%d", m_pendingIndex);
            pOutput->message_f("selection: UniqueNo:%08X EndPara:%08X ActIndex:%04X Mode:%04X EventNum:%04X EventPara:%04X",
                *(uint32_t*)(selection + 4), *(uint32_t*)(selection + 8),
                *(uint16_t*)(selection + 12), *(uint16_t*)(selection + 14),
                *(uint16_t*)(selection + 16), *(uint16_t*)(selection + 18));
        }

        pPacket->addOutgoingPacket_s(0x05B, 20, selection);

        m_pendingEventEnd = true;
        m_pendingEndTick = m_tickCount + 1;
    }

    if (m_pendingEventEnd && m_tickCount >= m_pendingEndTick)
    {
        m_pendingEventEnd = false;

        if (!m_sameZoneTeleport)
        {
            uint8_t eventend[20] = { 0 };
            *(uint32_t*)(eventend + 4) = m_pendingUniqueNo;
            *(uint32_t*)(eventend + 8) = (uint32_t)m_pendingIndex << 16 | 0x0002;
            *(uint16_t*)(eventend + 12) = m_pendingActIndex;
            *(uint16_t*)(eventend + 14) = 0x0000;
            *(uint16_t*)(eventend + 16) = (uint16_t)m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberZone(0);
            *(uint16_t*)(eventend + 18) = m_lastEventPara;

            if (m_debugMode)
                pOutput->message_f("eventend: UniqueNo:%08X EndPara:%08X ActIndex:%04X Mode:%04X EventNum:%04X EventPara:%04X",
                    *(uint32_t*)(eventend + 4), *(uint32_t*)(eventend + 8),
                    *(uint16_t*)(eventend + 12), *(uint16_t*)(eventend + 14),
                    *(uint16_t*)(eventend + 16), *(uint16_t*)(eventend + 18));

            pPacket->addOutgoingPacket_s(0x05B, 20, eventend);
            m_pendingZoneConfirm = true;
            m_zoneTimeoutTick = m_tickCount + 300;
        }

        m_pendingFollowerClear = true;
        m_pendingFollowerClearTick = m_tickCount + 600;
    }

    if (m_pendingFollowerClear && m_tickCount >= m_pendingFollowerClearTick)
    {
        m_pendingFollowerClear = false;
        m_isFollower = false;
    }

    if (m_pendingZoneConfirm && m_tickCount >= m_zoneTimeoutTick)
    {
        if (m_retryCount >= 5)
        {
            pOutput->message("Teleport failed after 5 attempts.");
            m_pendingZoneConfirm = false;
            m_retryCount = 0;
            m_isFollower = false;
            return;
        }
        pOutput->message_f("Retrying attempt:%d - If stuck, homepoint may not be unlocked. Type '/multiport stop' to cancel.", m_retryCount + 1);
        m_retryCount++;
        TeleportToIndex(m_retryIndex, true);
    }
}
