#include "engine/controls/enginecontrol.h"

#include "engine/enginebuffer.h"
#include "engine/enginemaster.h"
#include "engine/sync/enginesync.h"
#include "mixer/playermanager.h"
#include "moc_enginecontrol.cpp"

EngineControl::EngineControl(const QString& group,
        UserSettingsPointer pConfig)
        : m_group(group),
          m_pConfig(pConfig),
          m_pEngineMaster(nullptr),
          m_pEngineBuffer(nullptr) {
    setFrameInfo(mixxx::audio::kStartFramePos,
            mixxx::audio::kInvalidFramePos,
            mixxx::audio::SampleRate());
}

EngineControl::~EngineControl() {
}

void EngineControl::process(const double dRate,
        mixxx::audio::FramePos currentPosition,
        const int iBufferSize) {
    Q_UNUSED(dRate);
    Q_UNUSED(currentPosition);
    Q_UNUSED(iBufferSize);
}

void EngineControl::trackLoaded(TrackPointer pNewTrack) {
    Q_UNUSED(pNewTrack);
}

void EngineControl::trackBeatsUpdated(mixxx::BeatsPointer pBeats) {
    Q_UNUSED(pBeats);
}

void EngineControl::hintReader(HintVector*) {
}

void EngineControl::setEngineMaster(EngineMaster* pEngineMaster) {
    m_pEngineMaster = pEngineMaster;
}

void EngineControl::setEngineBuffer(EngineBuffer* pEngineBuffer) {
    m_pEngineBuffer = pEngineBuffer;
}

void EngineControl::setFrameInfo(mixxx::audio::FramePos currentPosition,
        mixxx::audio::FramePos trackEndPosition,
        mixxx::audio::SampleRate sampleRate) {
    m_frameInfo.setValue(FrameInfo{currentPosition, trackEndPosition, sampleRate});
}

QString EngineControl::getGroup() const {
    return m_group;
}

UserSettingsPointer EngineControl::getConfig() {
    return m_pConfig;
}

EngineMaster* EngineControl::getEngineMaster() {
    return m_pEngineMaster;
}

EngineBuffer* EngineControl::getEngineBuffer() {
    return m_pEngineBuffer;
}

void EngineControl::setBeatLoop(mixxx::audio::FramePos startPosition, bool enabled) {
    if (m_pEngineBuffer) {
        return m_pEngineBuffer->setBeatLoop(startPosition, enabled);
    }
}

void EngineControl::setLoop(mixxx::audio::FramePos startPosition,
        mixxx::audio::FramePos endPosition,
        bool enabled) {
    if (m_pEngineBuffer) {
        return m_pEngineBuffer->setLoop(startPosition, endPosition, enabled);
    }
}

void EngineControl::seekAbs(mixxx::audio::FramePos position) {
    if (m_pEngineBuffer) {
        m_pEngineBuffer->seekAbs(position);
    }
}

void EngineControl::seekExact(mixxx::audio::FramePos position) {
    std::cout << "seekExact" << std::endl;
    if (m_pEngineBuffer) {
        m_pEngineBuffer->seekExact(position);
    }
}

void EngineControl::seek(double fractionalPosition) {
    std::cout << "seek" << std::endl;

    if (m_pEngineBuffer) {
        m_pEngineBuffer->slotControlSeek(fractionalPosition);
    }
}

EngineBuffer* EngineControl::pickSyncTarget() {
    EngineMaster* pMaster = getEngineMaster();
    if (!pMaster) {
        return nullptr;
    }

    EngineSync* pEngineSync = pMaster->getEngineSync();
    if (!pEngineSync) {
        return nullptr;
    }

    EngineChannel* pThisChannel = pMaster->getChannel(getGroup());
    Syncable* pSyncable = pEngineSync->pickNonSyncSyncTarget(pThisChannel);
    // pickNonSyncSyncTarget can return nullptr, but if it doesn't the Syncable
    // definitely has an EngineChannel.
    if (pSyncable) {
        return pSyncable->getChannel()->getEngineBuffer();
    }
    return nullptr;
}
