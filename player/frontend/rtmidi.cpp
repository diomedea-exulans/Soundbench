/*
	This file is part of Soundbench.

	Soundbench is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Soundbench is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Soundbench.  If not, see <http://www.gnu.org/licenses/>.

	Copyright 2012  Amaya S.
*/

#include "rtmidi.h"

namespace sb {
#ifndef NO_RTMIDI
	bool RtMidiFrontend::instantiable() {
		try {
#if defined(SB_ENV_LINUX) & !defined(NO_JACK)
			std::vector<RtMidi::Api> vec;
			RtMidiIn::getCompiledApi(vec);
			for (RtMidi::Api apii : vec) {
				if (apii != RtMidi::UNIX_JACK) {
					RtMidiIn inne(apii,"Soundbench Probe");
					if (inne.getPortCount() > 0)
						return true;
				}
			}
			if(vec.size() == 1 && vec.back() == RtMidi::UNIX_JACK)
				std::cerr << "Only JACK support exists for the RtMidi frontend and the JACK frontend has been compiled. Skipping this frontend...\n";
			else if(vec.size() > 1)
				std::cerr << "All of the supported APIs for the RtMidi frontend failed. Skipping this frontend...\n";
			return false;
#else
			RtMidiIn inne(RtMidi::UNSPECIFIED,"Soundbench Probe");
			return (inne.getPortCount() > 0);
#endif
		}
		catch (RtError& e) {
			std::cerr << "RtMidi threw an exception: " << e.getMessage() << ". Skipping this frontend...\n";
			return false;
		}
	}

	RtMidiFrontend::RtMidiFrontend(Synth *s, size_t porte) : MidiFrontend(s, porte) {
		usevport = false;
		udata.syn = s;
		udata.nodeiter= foist;
		udata.recording = false;

		try {
#if defined(SB_ENV_LINUX) & !defined(NO_JACK)
			rtm = new RtMidiIn(RtMidi::LINUX_ALSA,"Soundbench");
#else
			rtm = new RtMidiIn(RtMidi::UNSPECIFIED,"Soundbench");
#endif
		}
		catch (RtError& e) {
			ready = false;
			return;
		}
		if (rtm->getPortCount() > 1 && portnum == 0)
			portnum = 1;

		rtm->setCallback(callback,reinterpret_cast<void*>(&udata));
		rtm->ignoreTypes();
		ready = true;
	}

	void RtMidiFrontend::record(bool b) {
		udata.mutt.lock();
		if (b) {
			if (!udata.recording) {
				udata.nodeiter = foist;
				udata.nodeiter->chainDestroy();
				udata.starttime = std::chrono::high_resolution_clock::now();
				udata.recording = true;
			}
		}
		else {
			if (udata.recording) {
				midi::nodes::MIDIEventNode* nod = new midi::nodes::EndNode;
				udata.nodeiter->attachNext(nod);
				udata.recording = false;
			}
		}
		udata.mutt.unlock();
	}
	void RtMidiFrontend::stop() {
		rtm->closePort();
		running = false;
	}
	void RtMidiFrontend::start() {
		if (usevport)
			rtm->openVirtualPort("Soundbench MIDI Input");
		else
			rtm->openPort(portnum,"Soundbench MIDI Input");
		running = true;
	}

	void RtMidiFrontend::callback(double, std::vector<unsigned char>* message, void* userdata) {
		using namespace std::chrono;
		RtUserData& rtd = *reinterpret_cast<RtUserData*>(userdata);
		uint8_t byte = (*message)[0];
		if(byte & Bit1) {
			rtd.eve.evtype = static_cast<midi::MidiEvents>(byte & Nibble1); //Mask out the channel bits.
			rtd.eve.chan = byte & Nibble2; //Mask out the event type bits.

			if (rtd.eve.evtype != midi::System) {
				rtd.eve.params.first = (*message)[1];
				if (rtd.eve.evtype != midi::ProgramChange && rtd.eve.evtype != midi::ChannelPressure)
					rtd.eve.params.second = (*message)[2];
			}
		}
		else { //Running status.
			rtd.eve.params.first = byte;
			if (rtd.eve.evtype != midi::ProgramChange && rtd.eve.evtype != midi::ChannelPressure)
				rtd.eve.params.second = (*message)[1];
		}
		midi::doEvent(rtd.syn,rtd.eve);
		rtd.mutt.lock();
		if (rtd.recording) {
			rtd.eve.delay = duration_cast<microseconds>(high_resolution_clock::now()-rtd.starttime).count();
			rtd.nodeiter->attachNext(midi::nodes::makeNode(rtd.eve));
			rtd.nodeiter = rtd.nodeiter->returnNext();
			rtd.starttime = high_resolution_clock::now();
		}
		rtd.mutt.unlock();
	}

#else
	bool RtMidiFrontend::instantiable() {
		return false;
	}
#endif
}
