/*
   Copyright (c) 2019 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#pragma once

#include "JuceHeader.h"

#include "IncludeFFMeters.h"

#include "Pool.h"

#include "JammerService.h"

#include "PacketStreamQueue.h"
#include "Recorder.h"
#include "Tuner.h"
#include "MidiRecorder.h"
#include "MidiPlayAlong.h"

#include "ApplicationState.h"

#include <chrono>

class AudioCallback : public AudioIODeviceCallback {
public:
	AudioCallback();
	virtual ~AudioCallback();

	virtual void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels, float** outputChannelData, int numOutputChannels, int numSamples) override;
	virtual void audioDeviceAboutToStart(AudioIODevice* device) override;
	virtual void audioDeviceStopped() override;

	void newServer();
	void setChannelSetup(JammerNetzChannelSetup const &channelSetup);

	FFAU::LevelMeterSource* getMeterSource();
	FFAU::LevelMeterSource* getSessionMeterSource();
	FFAU::LevelMeterSource* getOutputMeterSource();
	std::weak_ptr<MidiClocker> getClocker();
	MidiPlayAlong *getPlayalong();

	// Statistics
	int64 numberOfUnderruns() const;
	uint64 currentBufferSize() const;
	int currentPacketSize();
	uint64 currentPlayQueueSize() const;
	int currentDiscardedPackageCounter() const;
	double currentToPlayLatency() const;

	std::string currentReceptionQuality() const;
	double currentSampleRate() const;
	bool isReceivingData();
	double currentRTT();
	float channelPitch(int channel) const;
	float sessionPitch(int channel);

	std::shared_ptr<Recorder> getMasterRecorder() const;
	std::shared_ptr<Recorder> getLocalRecorder() const;
	std::shared_ptr<JammerNetzClientInfoMessage> getClientInfo();
	JammerNetzChannelSetup getSessionSetup();

private:
	void clearOutput(float** outputChannelData, int numOutputChannels, int numSamples);
	void samplesPerTime(int numSamples);

	JammerService jammerService_;

	PacketStreamQueue playBuffer_;
	std::atomic_bool isPlaying_;
	std::atomic_int64_t playUnderruns_;
	std::atomic_uint64_t minPlayoutBufferLength_;
	std::atomic_uint64_t maxPlayoutBufferLength_;
	std::atomic_int64_t currentPlayQueueLength_;
	std::string currentText_;
	std::atomic_int64_t numSamplesSinceStart_;
	int discardedPackageCounter_;
	double toPlayLatency_;
	JammerNetzChannelSetup channelSetup_;
	FFAU::LevelMeterSource meterSource_; // This is for peak metering
	FFAU::LevelMeterSource sessionMeterSource_; // This is to display the complete session peak meters
	FFAU::LevelMeterSource outMeterSource_; // This is for peak metering the output

	std::shared_ptr<Recorder> uploadRecorder_;
	std::shared_ptr<Recorder> masterRecorder_;
	std::unique_ptr<MidiRecorder> midiRecorder_;
	std::unique_ptr<MidiPlayAlong> midiPlayalong_;

	std::unique_ptr<Tuner> tuner_;

	std::chrono::time_point<std::chrono::steady_clock> startTime_;
	std::chrono::time_point<std::chrono::steady_clock> lastTime_;

	Pool<AudioBuffer<float>> bufferPool_;

	// Generic listeners, required to maintain the lifetime of the Values and their listeners
	std::vector<std::unique_ptr<ValueListener>> listeners_;
};


