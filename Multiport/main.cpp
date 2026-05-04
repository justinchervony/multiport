#include "Multiport.h"

__declspec(dllexport) IPlugin* __stdcall expCreatePlugin(const char* args)
{
    UNREFERENCED_PARAMETER(args);
    return (IPlugin*)(new Multiport());
}

__declspec(dllexport) double __stdcall expGetInterfaceVersion(void)
{
    return ASHITA_INTERFACE_VERSION;
}

bool Multiport::Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id)
{
    m_AshitaCore = core;
    m_LogManager = logger;
    m_PluginId = id;
    pOutput = new OutputHelpers(core, logger, GetName());
    pPacket = new safePacketInjector(core->GetPacketManager());

    m_pendingSelection = false;
    m_pendingEventEnd = false;
    m_pendingIndex = 0;
    m_pendingActIndex = 0;
    m_pendingUniqueNo = 0;
    m_pendingSelectionTick = 0;
    m_pendingEndTick = 0;
    m_tickCount = 0;
    m_isFollower = false;

    pOutput->message("Multiport loaded successfully.");
    return true;
}

void Multiport::Release(void)
{
    delete pPacket;
    delete pOutput;
}